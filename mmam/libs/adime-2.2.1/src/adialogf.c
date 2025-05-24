/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   adialogf.c:
   The adime_dialogf() function and its friends.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* Custom callback. */
void (*adime_callback)(DIALOG *d) = NULL;

/* The dialog. */
_ADIME_DIALOGF_DIALOG *_adime_dialog = NULL;

/* Internal complicated stuff. */
int _adime_dialogf_final_pass, _adime_dialogf_pass_index;



/* d_dialogf_window_proc:
*/
int d_dialogf_window_proc(int msg, DIALOG *d, int c)
{
   if (msg == MSG_IDLE) {
      if (adime_yield)
         yield_timeslice();
      if (adime_callback)
         adime_callback(_adime_dialog->dialog);
      return D_O_K;
   }
   else if (adime_window_visible)
      return adime_d_window_proc(msg, d, c);
   else
      return D_O_K;
}



/* _adime_ok_cancel_button_callback:
*/
int _adime_ok_cancel_button_callback(DIALOG *d, int button)
{
   _ADIME_UNUSED_ARG(d);
   if (button == 1)
      return -1;
   else {
      _ADIME_ASSERT(button == 2, ("INTERNAL ERROR"));
      return 2;
   }
}



/* _adime_dialogf_create_desc:
   Creates a description dialog object.
*/
void _adime_dialogf_create_desc(DIALOG *d, int y, ADIME_CONST char *desc)
{
   d->proc = adime_d_multiline_text_proc;
   _adime_multiline_text_size(_ADIME_FONT, desc, &d->w, &d->h);
   d->x = -adime_window_description_spacing - d->w;
   d->y = y;
   d->fg = _ADIME_TEXT_COLOR;
   d->bg = -1;
   d->key = 0;
   d->flags = 0;
   d->d1 = ADIME_ALIGN_RIGHT;
   d->d2 = 0;
   d->dp = (char *)desc;
   d->dp2 = _ADIME_FONT;
   d->dp3 = NULL;
}



/* my_strcmp:
   Returns NULL if the first strlen(name) characters of string are different
   from name or not followed by '[', otherwise returns a pointer to the end
   of `name' within `string' otherwise. `string' should be in the current
   Unicode format, and `name' in pure 7 bit ascii.
*/
static ADIME_CONST char *my_strcmp(ADIME_CONST char *string,
                                   ADIME_CONST char *name)
{
   int c1, c2;

   while (1) {
      c1 = ugetxc(&string);
      c2 = *name++;
      if (c2 == 0) {
         if (c1 == '[')
            return string;
         else
            return NULL;
      }
      if (c1 != c2)
         return NULL;
   }
}



/* _adime_dialogf_count_objects:
   Count number of DIALOGs to allocate, check validity of format string and
   find width of description texts. If `dd' is NULL, then it does nothing
   except call the custom routines to update the `args' pointer.
*/
void _adime_dialogf_count_objects(_ADIME_DIALOGF_DIALOG *dd,
                                  ADIME_CONST char *format,
                                  adime_va_list args)
{
   int gotchar;
   const char *t, *format_end;
   char *arg_buf, *desc_buf, *arg_buf_p, *desc_buf_p;
   int c;
   int found;
   int num_objects;
   void **saved_args;
   _ADIME_DIALOGF_FORMAT *df;
   _ADIME_DIALOGF_OBJECT *fo;

   _ADIME_MALLOC(arg_buf, ustrsizez(format), char);
   _ADIME_MALLOC(desc_buf, ustrsizez(format), char);

   t = format;
   gotchar = FALSE;
   format_end = t;
   desc_buf_p = desc_buf;
   for (c = ugetxc(&t); c; c = ugetxc(&t)) {
      if (c == '%') {
         if ((ugetc(t) == '%') || (ugetc(t) == '[') || (ugetc(t) == ']')) {
            /* An escaped percent sign or square bracket. */
            c = ugetxc(&t);
            desc_buf_p += usetc(desc_buf_p, c);
            gotchar = TRUE;
         }
         else {
            /* Found an object! */

            /* Finish desc buf. */
            usetc(desc_buf_p, 0);
            desc_buf_p = desc_buf;

            /* Find out which format this is. */
            found = FALSE;
            for (df = _adime_dialogf_formats; df != NULL; df = df->next) {
               format_end = my_strcmp(t, df->specifier);
               if (format_end != NULL) {
                  found = TRUE;
                  break;
               }
            }
            _ADIME_ASSERT(found, ("Error after \"%s\": "
                                  "Invalid format specifier (or "
                                  "specifier not followed by '['): %s",
                                  desc_buf, t));
            t = format_end;

            arg_buf_p = arg_buf;
            found = FALSE;
            /* See if argument list is ok. */
            for (c = ugetxc(&format_end); c; c = ugetxc(&format_end)) {
               if (c == '%') {
                  c = ugetxc(&format_end);
                  _ADIME_ASSERT(c != 0,
                                ("Error after \"%s%%%s[\": "
                                 "Format string terminated "
                                 "unexpectedly inside escape "
                                 "sequence in modifier: %s.",
                                 desc_buf, df->specifier, t));
                  if (c != ']')
                     arg_buf_p += usetc(arg_buf_p, '%');
               }
               else if (c == ']') {
                  found = TRUE;
                  break;
               }
               arg_buf_p += usetc(arg_buf_p, c);
            }
            _ADIME_ASSERT(found, ("Error in \"%%%s%s[%s\": "
                                  "Format string terminated in the middle "
                                  "of modifier list.",
                                  desc_buf, df->specifier, t));
            t = format_end;
            usetc(arg_buf_p, 0);
            arg_buf_p = arg_buf;

            fo = NULL;
            if (dd) {
               if (dd->fo == NULL) {
                  _ADIME_MALLOC(dd->objects, 1, _ADIME_DIALOGF_OBJECT);
                  dd->fo = dd->objects;
               }
               else {
                  _ADIME_MALLOC(dd->fo->next, 1, _ADIME_DIALOGF_OBJECT);
                  dd->fo = dd->fo->next;
               }
               fo = dd->fo;
               fo->type = df;
               fo->desc = ustrdup(desc_buf);
               fo->again = TRUE;
               fo->modifiers = ustrdup(arg_buf);
               fo->args = NULL;
               fo->next = NULL;
               saved_args = &fo->args;
            }
            else {
               _ADIME_ASSERT(0, ("NOTREACHED"));
	       /* need to set this to null to avoid warning */
               saved_args = NULL;
            }

            num_objects = df->count_dialog(fo->desc, fo->modifiers,
                                           args, saved_args);
            if (dd)
               fo->num_dialogs = num_objects;

            gotchar = FALSE;
         }
      }
      else {
         /* A normal character. */
         desc_buf_p += usetc(desc_buf_p, c);
         gotchar = TRUE;
      }
   }

   free(arg_buf);
   free(desc_buf);
   _ADIME_ASSERT(!gotchar,
                 ("Garbage after last specifier in format string: %s",
                  format));
}



/* _adime_dialogf_create_objects:
*/
void _adime_dialogf_create_objects(_ADIME_DIALOGF_DIALOG *dd)
{
   int object_top, min_y, max_y;
   int i;
   int font_h;
   int y_diff;
   _ADIME_DIALOGF_OBJECT *fo, *d;

   font_h = text_height(_ADIME_FONT);

   while (dd->fo != NULL) {
      fo = dd->fo;
      /* Found an object! */
      object_top = dd->bottom;

      /* Create description object. */
      if (!fo->type->handle_desc)
         if (_adime_dialogf_pass_index == 0)
            _adime_dialogf_create_desc(fo->dialogs - 1, dd->bottom,
                                       fo->desc);
      /* Create object. */
      if (_adime_dialogf_pass_index == 0) {
         for (i = 0; i < fo->num_dialogs; i++) {
            fo->dialogs[i].proc = d_yield_proc;
            fo->dialogs[i].x = 0;
            fo->dialogs[i].y = object_top;
            fo->dialogs[i].w = dd->edit_w;
            fo->dialogs[i].h = font_h;
            fo->dialogs[i].fg = fo->dialogs[i].bg = 0;
            fo->dialogs[i].key = 0;
            fo->dialogs[i].flags = 0;
            fo->dialogs[i].d1 = fo->dialogs[i].d2 = 0;
            fo->dialogs[i].dp = fo->dialogs[i].dp2 =
                                fo->dialogs[i].dp3 = NULL;
         }
      }

      if (fo->type->create_dialog != NULL) {
         if ((fo->again & 1) ||
             ((fo->again & 2) && _adime_dialogf_final_pass)) {
            fo->again = fo->type->create_dialog(fo->dialogs, fo->desc,
                                                fo->modifiers, fo->args);
            if (fo->again & 1)
               dd->more_passes = TRUE;
            if (fo->again & 2)
               dd->need_final_pass = TRUE;
         }
      }
      else
         fo->again = 0;

      /* Align objects vertically. */
      if (fo->num_dialogs > 0) {
         min_y = INT_MAX;
         max_y = -INT_MAX;

         for (i = 0; i < fo->num_dialogs; i++) {
            if (fo->dialogs[i].y < min_y)
               min_y = fo->dialogs[i].y;
            if (fo->dialogs[i].y + fo->dialogs[i].h > max_y)
               max_y = fo->dialogs[i].y + fo->dialogs[i].h;
         }

         if (fo->type->handle_desc) {
            y_diff = object_top - min_y;
            for (i = 0; i < fo->num_dialogs; i++)
               fo->dialogs[i].y += y_diff;
            dd->bottom = object_top + max_y - min_y;
         }
         else {
            if (fo->dialogs[-1].h > max_y - min_y) {
               fo->dialogs[-1].y = object_top;
               y_diff = object_top +
                        (fo->dialogs[-1].h - (max_y-min_y)) / 2 -
                        min_y;
               for (i = 0; i < fo->num_dialogs; i++)
                  fo->dialogs[i].y += y_diff;

               dd->bottom = object_top + fo->dialogs[-1].h;
            }
            else {
               fo->dialogs[-1].y = object_top + ((max_y-min_y) -
                                   fo->dialogs[-1].h) / 2;
               y_diff = object_top - min_y;
               for (i = 0; i < fo->num_dialogs; i++)
                  fo->dialogs[i].y += y_diff;

               dd->bottom = object_top + max_y - min_y;
            }
         }

         if (_adime_dialogf_pass_index == 0) {
            fo->bottom = dd->bottom - dd->top;
         }
         else if (dd->bottom - dd->top != fo->bottom) {
            /* Move the rest of the objects down accordingly. */
            y_diff = (dd->bottom - dd->top) - fo->bottom;
            for (i = fo->num_dialogs; fo->dialogs[i].proc != NULL; i++)
               fo->dialogs[i].y += y_diff;
            for (d = fo->next; d != NULL; d = d->next)
               d->bottom += y_diff;
            fo->bottom = dd->bottom - dd->top;
         }
      }

      if (fo->num_dialogs >= 0) {
         /* Move on to next object. */
         dd->bottom += adime_window_line_spacing;
      }
      if (!(fo->again & 4))
         dd->fo = dd->fo->next;
   }
}



/* _adime_dialogf_start:
   Sets up a `_ADIME_DIALOGF_DIALOG *' struct ready to be used by
   `_adime_dialogf_run()'
*/
_ADIME_DIALOGF_DIALOG *_adime_dialogf_start(ADIME_CONST char *title,
                                            int x_pos, int y_pos, int edit_w,
                                            ADIME_CONST char *format,
                                            adime_va_list args)
{
   #define FIRST_CUSTOM (1)
   int x, w, x_diff;
   int title_w, dialog_w, dialog_x;
   int actual_x_pos, actual_y_pos;
   int y_diff;
   int dialog_h;
   _ADIME_DIALOGF_DIALOG *dd;
   DIALOG *d;
   int num_objects;
   DIALOG *pos;
   int title_h;

   _ADIME_DIALOGF_DIALOG *prev_adime_dialog;
   int prev_dialogf_final_pass, prev_dialogf_pass_index;

   _ADIME_ASSERT(_adime_inited, ("You must call adime_init() before anything"
				 " else."));

   /* Allocate the dialog info and setup global stuff. */
   _ADIME_MALLOC(dd, 1, _ADIME_DIALOGF_DIALOG);
   prev_adime_dialog = _adime_dialog;
   _adime_dialog = dd;

   dd->dialog = NULL;
   dd->objects = NULL;
   dd->fo = NULL;
   dd->return_value = 0;

   title_h = text_height(_ADIME_TITLE_FONT);
   title_w = text_length(_ADIME_TITLE_FONT, title);

   /* Count objects so that we know how much memory to allocate. */
   _adime_dialogf_count_objects(dd, format, args);

   /* window+NULL */
   num_objects = 2;
   for (dd->fo = dd->objects; dd->fo; dd->fo = dd->fo->next) {
      if (dd->fo->num_dialogs >= 0)
         num_objects += dd->fo->num_dialogs;
      if (!dd->fo->type->handle_desc)
         num_objects++;
   }

   /* Allocate memory for the dialog. */
   _ADIME_MALLOC(dd->dialog, num_objects, DIALOG);

   /* Set pointer to dialogs for all objects. */
   pos = dd->dialog + FIRST_CUSTOM;
   for (dd->fo = dd->objects; dd->fo; dd->fo = dd->fo->next) {
      if (!dd->fo->type->handle_desc)
         pos++;
      dd->fo->dialogs = pos;
      if (dd->fo->num_dialogs >= 0)
         pos += dd->fo->num_dialogs;
   }

   /* Construct the dialog. */
   prev_dialogf_final_pass = _adime_dialogf_final_pass;
   prev_dialogf_pass_index = _adime_dialogf_pass_index;
   _adime_dialogf_final_pass = FALSE;
   _adime_dialogf_pass_index = 0;
   dd->need_final_pass = FALSE;
   dd->edit_w = edit_w;
   /* This loop is a bit on the complex side: Some objects need to be
      reconfigured when the other objects have been created. Therefore we
      loop as long as there exist objects that request more passes. An object
      can tell `adime_dialogf()' that it wants more passes by returning
      nonzero from the `create_dialog()' function.
      An example of such an object is "%line", which needs to know the total
      width of the dialog so that it can fill up the whole dialog width in
      a nice way.
   */
   do {
      dd->more_passes = FALSE;

      dd->fo = dd->objects;
      if (_adime_dialogf_pass_index == 0)
         dd->top = 0;
      else
         dd->top = dd->dialog[0].y + adime_window_border_thickness + title_h +
                   adime_window_title_spacing;
      dd->bottom = dd->top;

      /* Create all the objects. */
      _adime_dialogf_create_objects(dd);

      /* Set first non-generated object to NULL. */
      dd->dialog[num_objects-1].proc = NULL;

      /* Fixup alignment of all objects. */
      dd->bottom -= adime_window_line_spacing;
      dialog_h = adime_window_border_thickness +
                 title_h +
                 adime_window_title_spacing +
                 dd->bottom - dd->top +
                 adime_window_border_thickness;

      dialog_x = dd->dialog[FIRST_CUSTOM].x;
      dialog_w = dd->dialog[FIRST_CUSTOM].x + dd->dialog[FIRST_CUSTOM].w;
      for (d = dd->dialog + FIRST_CUSTOM; d->proc != NULL; d++) {
         x = d->x;
         if (x < dialog_x)
            dialog_x = x;
         w = d->x + d->w;
         if (w > dialog_w)
            dialog_w = w;
      }
      dialog_w -= dialog_x;
      if (title_w > dialog_w)
         dialog_w = title_w;
      if (adime_window_button_w * 2 + adime_window_between_button_spacing >
         dialog_w)
         dialog_w = adime_window_button_w * 2 +
                    adime_window_between_button_spacing;
      dialog_w += 2 * adime_window_border_thickness;
      if (x_pos == ADIME_ALIGN_CENTRE)
         actual_x_pos = (_ADIME_BMP_W - dialog_w) / 2;
      else if (x_pos == ADIME_ALIGN_RIGHT)
         actual_x_pos = _ADIME_BMP_W - dialog_w;
      else
         actual_x_pos = x_pos;
      x_diff = actual_x_pos + adime_window_border_thickness - dialog_x;

      if (y_pos == ADIME_ALIGN_CENTRE)
         actual_y_pos = (_ADIME_BMP_H - dialog_h) / 2;
      else if (y_pos == ADIME_ALIGN_BOTTOM)
         actual_y_pos = (_ADIME_BMP_H - dialog_h);
      else
         actual_y_pos = y_pos;
      y_diff = actual_y_pos + adime_window_border_thickness +
               title_h + adime_window_title_spacing - dd->top;

      for (d = dd->dialog + FIRST_CUSTOM; d->proc != NULL; d++) {
         d->x += x_diff;
         d->y += y_diff;
      }

      /* Create the rest of the objects. */

      /* Background. */
      dd->dialog->x = actual_x_pos;
      dd->dialog->y = actual_y_pos;
      dd->dialog->w = dialog_w;
      dd->dialog->h = dialog_h;
      if (_adime_dialogf_pass_index == 0) {
         dd->dialog->proc = d_dialogf_window_proc;
         dd->dialog->fg =  dd->dialog->bg = 0;
         dd->dialog->key = 0;
         dd->dialog->flags = 0;
         dd->dialog->d1 = dd->dialog->d2 = 0;
         dd->dialog->dp = (char *)title;
         dd->dialog->dp2 = _ADIME_TITLE_FONT;
         dd->dialog->dp3 = NULL;
      }

      /* The terminating NULL object */
      dd->dialog[num_objects - 1].proc = NULL;

      if (_adime_dialogf_final_pass)
         break;

      if (dd->need_final_pass && !dd->more_passes) {
         _adime_dialogf_final_pass = TRUE;
         dd->more_passes = TRUE;
      }
      _adime_dialogf_pass_index++;
   } while (dd->more_passes);

   /* Reset global variables. */
   _adime_dialog = prev_adime_dialog;
   _adime_dialogf_final_pass = prev_dialogf_final_pass;
   _adime_dialogf_pass_index = prev_dialogf_pass_index;

   return dd;
}



/* _adime_dialogf_run:
   Run a dialog once it has been set up by `_adime_dialogf_start()'.
*/
int _adime_dialogf_run(_ADIME_DIALOGF_DIALOG *dd)
{
   BITMAP *background_bmp = NULL;
   _ADIME_DIALOGF_DIALOG *prev_adime_dialog;
   int do_clean_screen;

   prev_adime_dialog = _adime_dialog;
   _adime_dialog = dd;

   if (adime_bmp == NULL)
      scroll_screen(0, 0);

   /* Store background. */
   if (adime_clean_screen) {
      background_bmp = create_bitmap(dd->dialog[0].w, dd->dialog[0].h);
      _ADIME_ASSERT(background_bmp != NULL, ("Out of memory!"));

      scare_mouse_area(dd->dialog[0].x, dd->dialog[0].y,
                       dd->dialog[0].w, dd->dialog[0].h);
      blit(_ADIME_BMP, background_bmp, dd->dialog[0].x, dd->dialog[0].y,
           0, 0, dd->dialog[0].w, dd->dialog[0].h);
      unscare_mouse();
   }
   do_clean_screen = adime_clean_screen;

   /* Run. */
   adime_do_dialog_find_focus_object(dd->dialog,
                                     dd->objects->dialogs - dd->dialog);

   /* Clean screen. */
   if (do_clean_screen) {
      scare_mouse_area(dd->dialog[0].x, dd->dialog[0].y,
                       dd->dialog[0].w, dd->dialog[0].h);
      blit(background_bmp, _ADIME_BMP, 0, 0,
                           dd->dialog[0].x, dd->dialog[0].y,
                           dd->dialog[0].w, dd->dialog[0].h);
      unscare_mouse();
      destroy_bitmap(background_bmp);
   }

   /* Restore global _adime_dialog. */
   _adime_dialog = prev_adime_dialog;

   _ADIME_ASSERT(dd->return_value != ADIME_REDRAW,
                 ("Internal error: "
                  "_ADIME_DIALOGF_DIALOG->return_value should not be "
                  "ADIME_REDRAW"));
   return dd->return_value;
}



/* _adime_dialogf_store_results:
   Saves the results after `_adime_dialogf_run()' has been called.
*/
void _adime_dialogf_store_results(_ADIME_DIALOGF_DIALOG *dd)
{
   _ADIME_DIALOGF_OBJECT *o;
   for (o = dd->objects; o; o = o->next)
      if (o->type->store_dialog != NULL)
         o->type->store_dialog(o->dialogs);
}



/* _adime_dialogf_reset_dialog:
   Saves the results after `_adime_dialogf_run()' has been called.
*/
void _adime_dialogf_reset_dialog(_ADIME_DIALOGF_DIALOG *dd)
{
   _ADIME_DIALOGF_OBJECT *o;
   for (o = dd->objects; o; o = o->next)
      if (o->type->reset_dialog != NULL)
         o->type->reset_dialog(o->dialogs);
}



/* _adime_dialogf_end:
   Frees the `_ADIME_DIALOGF_DIALOG *' struct and the other stuff that has
   been allocated.
*/
void _adime_dialogf_end(_ADIME_DIALOGF_DIALOG *dd)
{
   _ADIME_DIALOGF_OBJECT *o, *next_o;

   /* Custom clean up function. */
   for (o = dd->objects; o; o = next_o) {
      if (o->type->destroy_dialog != NULL)
         o->type->destroy_dialog(o->dialogs);
      free(o->desc);
      free(o->modifiers);
      next_o = o->next;
      free(o);
   }

   /* Free some more stuff. */
   free(dd->dialog);
   free(dd);
}



/* adime_lowlevel_vdialogf:
   Like adime_vdialogf(), but without the ok and cancel buttons.
*/
int adime_lowlevel_vdialogf(ADIME_CONST char *title, int x_pos, int y_pos,
                            int edit_w, ADIME_CONST char *format,
                            adime_va_list args)
{
   int ret;
   _ADIME_DIALOGF_DIALOG *dd;

   dd = _adime_dialogf_start(title, x_pos, y_pos, edit_w, format, args);
   _adime_dialogf_reset_dialog(dd);
   ret = _adime_dialogf_run(dd);
   if (ret < 0)
      _adime_dialogf_store_results(dd);
   _adime_dialogf_end(dd);

   return ret < 0 ? -ret : ret;
}



/* adime_lowlevel_dialogf:
   Like adime_dialogf(), but without the ok and cancel buttons.
*/
int adime_lowlevel_dialogf(ADIME_CONST char *title, int x, int y, int edit_w,
                           ADIME_CONST char *format, ...)
{
   int ret;
   adime_va_list ap;

   adime_va_start(ap, format);
   ret = adime_lowlevel_vdialogf(title, x, y, edit_w, format, ap);
   adime_va_end(ap);
   return ret;
}



/* adime_vdialogf:
   Like adime_dialogf(), but takes the extra arguments in a adime_va_list
   instead.
*/
int adime_vdialogf(ADIME_CONST char *title, int x_pos, int y_pos, int edit_w,
                   ADIME_CONST char *format, adime_va_list args)
{
   return adime_lowlevel_dialogf(title, x_pos, y_pos, edit_w,
                                 "%chain[]"
                                 ADIME_OK_CANCEL_BUTTON_FORMAT,
                                 format, args,
                                 _adime_ok_cancel_button_callback);
}



/* adime_dialogf:
   Given a format string, lets the user edit the arguments given using
   Allegro's dialogs. See the documentation for info about the format string.
   Returns 1 if the user pressed OK and 2 if he pressed Cancel.
*/
int adime_dialogf(ADIME_CONST char *title, int x, int y, int edit_w,
                  ADIME_CONST char *format, ...)
{
   int ret;
   adime_va_list ap;

   adime_va_start(ap, format);
   ret = adime_vdialogf(title, x, y, edit_w, format, ap);
   adime_va_end(ap);

   return ret;
}
