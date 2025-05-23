/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dbutrow.c:
   adime_dialogf() functions for a row of buttons.

   By Sven Sandberg

   See readme.txt for more information about Adime.


   int my_callback(DIALOG *d, int button);
   adime_dialogf(..., "%buttonrow[&OK;ENTER;ALT+O,&Cancel;ALT+C;ESC]",
                 my_callback);
*/
#include <stdarg.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



typedef struct SHORTCUT
{
   int n_shortcuts;
   int *scancodes;
   int *modifiers_on;
   int *modifiers_off;
   void *extra;
} SHORTCUT;



/* _adime_create_shortcut:
   Creates an _ADIME_SHORTCUT object, given the text describing it.
*/
static SHORTCUT *create_shortcut(char *text)
{
   int c;
   char *buf;
   char *p;
   SHORTCUT *ret;
   int n;
   int flag;

   if (ugetc(text))
      n = 1;
   else
      n = 0;

   p = text;
   for (c = ugetx(&p); c; c = ugetx(&p))
      if (c == ';')
         n++;

   _ADIME_MALLOC(ret, 1, SHORTCUT);
   ret->n_shortcuts = n;
   ret->scancodes = NULL;
   if (n == 0)
      return ret;
   _ADIME_MALLOC(ret->scancodes, 3*n, int);
   ret->modifiers_on = ret->scancodes + n;
   ret->modifiers_off = ret->scancodes + 2*n;

   _ADIME_MALLOC(buf, ustrsizez(text), char);
   p = buf;
   n = 0;
   while (1) {
      c = ugetx(&text);
      if ((c == 0) || (c == ';')) {
         p += usetc(p, 0);
         ret->scancodes[n] = adime_short_name_to_scancode(buf);
         ret->modifiers_on[n] = 0;
         ret->modifiers_off[n] = 0;
         _ADIME_ASSERT(ret->scancodes[n] != -1,
                       ("Invalid scancode name: %s", buf));
         n++;
         if (c == 0)
            break;
         p = buf;
      }
      else if (c == '+') {
         usetc(p, 0);
         p = buf;
         if (ugetc(buf) == '-')
            p += uwidth(buf);
         flag = adime_short_name_to_keyflag(p);
         _ADIME_ASSERT(flag != -1,
                       ("Invalid modifier name: %s", p));
         if (p == buf)
            ret->modifiers_on[n] |= flag;
         else
            ret->modifiers_off[n] |= flag;
         p = buf;
      }
      else
         p += usetc(p, c);
   }
   free(buf);

   return ret;
}



/* destroy_shortcut:
*/
static void destroy_shortcut(SHORTCUT *s)
{
   free(s->scancodes);
   free(s);
}



/* check_shortcut:
   Given scancode and key modifiers, determines if that corresponds to a
   shortcut in the SHORTCUT object. Returns nonzero if it does, zero if it
   does not.
*/
static int check_shortcut(SHORTCUT *sc, int scancode, int flags)
{
   int i;
   for (i=0; i<sc->n_shortcuts; i++) {
      if ((sc->scancodes[i] == scancode) &&
          ((flags & sc->modifiers_on[i]) == sc->modifiers_on[i]) &&
          ((flags & sc->modifiers_off[i]) == 0))
         return TRUE;
   }
   return FALSE;
}



/* _adime_count_buttonrow:
*/
int _adime_count_buttonrow(ADIME_CONST char *desc,
                           ADIME_CONST char *modifiers, adime_va_list args,
                           void **args_out)
{
   int n = 1;
   int c;

   _ADIME_UNUSED_ARG(desc);
   _ADIME_UNUSED_ARG(args);

   _ADIME_ASSERT(ugetc(modifiers),
                 ("Error in \"%s%%buttonrow[%s]\": "
                  "You must specify at least one button for "
                  "the %%buttonrow[] format.",
                  desc, modifiers));

   *args_out = adime_va_arg(args, void *);

   for (c = ugetxc(&modifiers); c; c = ugetxc(&modifiers)) {
      if (c == '%') {
         c = ugetxc(&modifiers);
         _ADIME_ASSERT(c,
                       ("Error in \"%s%%buttonrow[%s]\": "
                        "Button format string ended unexpectedly inside "
                        "an escape sequence ('%%').",
                        desc, modifiers));
      }
      else if (c == ',')
         n++;
   }

   /* an extra d_dummy_proc */
   return n+1;
}



/* button_proc:
*/
static int button_proc(int msg, DIALOG *d, int c)
{
   if (msg == MSG_XCHAR)
      if (check_shortcut(d->dp3, c >> 8, key_shifts))
         return adime_d_button_proc(_ADIME_MSG_PRESS, d, 0) | D_USED_CHAR;
   return adime_d_button_proc(msg, d, c);
}



/* dummy_proc:
*/
static int dummy_proc(int msg, DIALOG *d, int c)
{
   _ADIME_UNUSED_ARG(msg);
   _ADIME_UNUSED_ARG(d);
   _ADIME_UNUSED_ARG(c);
   return D_O_K;
}



/* button_callback:
*/
static int button_callback(DIALOG *d)
{
   int ret = 0;
   int (*callback)(DIALOG *, int);

   if (d->dp3) {
      if (((SHORTCUT *)d->dp3)->extra) {
         callback = ((SHORTCUT *)d->dp3)->extra;
         ret = callback(d, d->d2);
      }
   }
   if (ret == ADIME_REDRAW)
      return D_REDRAW;
   else if (ret) {
      _adime_dialog->return_value = ret;
      return D_CLOSE;
   }
   else
      return D_O_K;
}



/* _adime_create_buttonrow:
   d1 = ADIME_BUTTON_CALLBACK
   d2 = button index
   dp = button text
   dp2 = adime's callback
   dp3 = shortcut
   shortcut->extra = user callback
*/
int _adime_create_buttonrow(DIALOG *d, ADIME_CONST char *desc,
                            ADIME_CONST char *modifiers, void *args)
{
   int c;
   char *button_text;
   char *p, *last_p;
   int state;
   int n;
   void *callback;
   int len, longest_button = 0;
   int i;

   _ADIME_UNUSED_ARG(desc);

   _ADIME_ASSERT(ustrcmp(desc, empty_string) == 0,
                 ("Error in \"%s%%buttonrow[%s]\": "
                  "You must not give any description text for %%buttonrow[]."
                  " (you gave: \"%s\").",
                  desc, modifiers, desc));

   if (_adime_dialogf_pass_index == 0) {
      _ADIME_ASSERT(ugetc(modifiers) != 0,
                    ("Error in \"%s%%buttonrow[%s]\": "
                     "You must specify at least one button in the "
                     "%%buttonrow[] format.",
                     desc, modifiers));

      n = 0;
      callback = args;
      do {
         state = _adime_get_string_arg(&modifiers, &button_text);
         _ADIME_ASSERT(state & _ADIME_ARG_READ,
                       ("Error in \"%s%%buttonrow[%s]\": "
                        "Button format string must not be empty.",
                        desc, modifiers));
         p = button_text;
         last_p = p;
         for (c = ugetx(&p); c && (c != ';'); c = ugetx(&p))
            last_p = p; /* do nothing */
         if (c == ';')
            d->dp3 = create_shortcut(p);
         else
            d->dp3 = create_shortcut(empty_string);
         usetc(last_p, 0);
         d->proc = button_proc;
         d->d1 = ADIME_BUTTON_CALLBACK;
         d->d2 = ++n;
         d->h = adime_window_button_h;
         d->dp = button_text;
         d->dp2 = button_callback;
         ((SHORTCUT *)d->dp3)->extra = callback;
         d++;
      } while (!(state & _ADIME_ARG_END));
      d->proc = dummy_proc;
      d->y -= adime_window_button_spacing - adime_window_line_spacing;
      d->h = 1;
      d->w = 1;
      return 1;
   }
   else if (_adime_dialogf_pass_index == 1) {
      i = 0;
      longest_button = adime_window_button_w;
      do {
         len = text_length(_ADIME_BUTTON_FONT, (char *)d[i].dp);
         if (len > longest_button)
            longest_button = len;
         i++;
      } while ((d[i].proc == button_proc) && (d[i].d2 != 1));

      i = 0;
      do {
         d[i].x = _adime_dialog->dialog->x + adime_window_border_thickness +
                  (longest_button + adime_window_between_button_spacing) * i;
         d[i].w = longest_button;
         i++;
      } while ((d[i].proc == button_proc) && (d[i].d2 != 1));

      return 0;
   }
   else
      _ADIME_ASSERT(0, ("NOTREACHED"));

   return 0;
}



/* _adime_destroy_buttonrow:
   Frees the texts.
*/
void _adime_destroy_buttonrow(DIALOG *d)
{
   do {
      free(d->dp);
      destroy_shortcut(d->dp3);
      d++;
   } while ((d->proc == button_proc) && (d->d2 > 1));
}
