/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dstring.c:
   This file contains functions for handling input in edit boxes.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/
#include <ctype.h>
#include <string.h>
#include <allegro.h>
#if ALLEGRO_DATE < 20011116
#include <allegro/aintern.h>
#else
#include <allegro/internal/aintern.h>
#endif

#include "adime.h"
#include "adime/adimeint.h"


ADIME_CLIPBOARD *_adime_edit_clipboard = NULL;
int _adime_edit_insert = ADIME_EDIT_ONLY_INSERT;
int *_adime_edit_default_insert = NULL;

#define uisalpha(c)  isalpha(c)
#define uisupper(c)  isupper(c)
#define uislower(c)  islower(c)


ADIME_EDITBOX_OPTIONS _normal_edit_text;
ADIME_EDITBOX_FUNCTIONS _normal_edit_funcs =
{
   adime_normal_edit,
   adime_normal_edit_key_text,
   adime_normal_edit_key_nontext,
   adime_normal_edit_pos_move,
   adime_normal_edit_pos_left,
   adime_normal_edit_pos_right,
   adime_normal_edit_pos_word_left,
   adime_normal_edit_pos_word_right,
   adime_normal_edit_pos_home,
   adime_normal_edit_pos_end,
   adime_normal_edit_delete_left,
   adime_normal_edit_delete_right,
   adime_normal_edit_toggle_insertmode,
   adime_normal_edit_select_word_at_pos,
   adime_normal_edit_pos_at_pixel,
   adime_normal_edit_clip_cut,
   adime_normal_edit_clip_copy,
   adime_normal_edit_clip_paste,
   adime_normal_edit_set_text,
   adime_normal_edit_replace_text,
   adime_normal_edit_num_increase,
   adime_normal_edit_num_decrease,
   adime_normal_edit_num_big_increase,
   adime_normal_edit_num_big_decrease,
   adime_normal_edit_create,
   adime_normal_edit_destroy
};



/* API shortcut functions. */

int edit(ADIME_EDITBOX *ed, int key, int shifts)
{
   return ed->options->funcs->edit(ed, key, shifts);
}

int edit_set_pos(ADIME_EDITBOX *ed, int pos)
{
   return ed->options->funcs->pos_move(ed, pos, FALSE);
}

int edit_select_to_pos(ADIME_EDITBOX *ed, int pos)
{
   return ed->options->funcs->pos_move(ed, pos, TRUE);
}

int edit_set_text(ADIME_EDITBOX *ed, char *text)
{
   return ed->options->funcs->set_text(ed, text);
}

int edit_cut(ADIME_EDITBOX *ed)
{
   return ed->options->funcs->clip_cut(ed);
}

int edit_copy(ADIME_EDITBOX *ed)
{
   return ed->options->funcs->clip_copy(ed);
}

int edit_paste(ADIME_EDITBOX *ed)
{
   return ed->options->funcs->clip_paste(ed);
}

int edit_select_word_at_pos(ADIME_EDITBOX *ed, int pos)
{
   return ed->options->funcs->select_word_at_pos(ed, pos);
}

ADIME_EDITBOX *create_edit(ADIME_EDITBOX_OPTIONS *eo, char *text)
{
   return eo->funcs->create(eo, text);
}

void destroy_edit(ADIME_EDITBOX *ed)
{
   ed->options->funcs->destroy(ed);
}



/* char_at_pixel:
   Returns the index of the character that is occupying x-th pixel when the
   text is drawn.
*/
static int char_at_pixel(FONT *f, char *text, int x)
{
   int i, c;

   if (!f)
      return x;

   for (i = 0, c = ugetx(&text); c; i++, c = ugetx(&text)) {
      x -= adime_char_width(f, c);
      if (x < 0)
         break;
   }

   return i;
}



/* edit_resize_text:
   Resizes the text if needed. Afterwards you will be able to store `size'
   characters in it. The string will be NULL-terminated.
   It won't do anything unless `ed->options->dynamical' is set.
   Returns -1 if it did realloc, and 0 if it didn't.
*/
static int edit_resize_text(ADIME_EDITBOX *ed, int size)
{
   if (ed->options->dynamic) {
      /* Grow. */
      if (size > ed->alloced_size) {
         ed->alloced_size = size + ed->options->increase_bytes;
         ADIME_REALLOC(ed->text, ed->alloced_size, char);
         return -1;
      /* Shrink. */
      }
      else if (size < ed->alloced_size - 3 * ed->options->increase_bytes) {
         ed->alloced_size = ed->alloced_size +
                            ed->options->increase_bytes;
         ADIME_REALLOC(ed->text, ed->alloced_size, char);
         return -1;
      }
   }

   return 0;
}



/* edit_selenabled:
   Returns nonzero if it is possible to select text in the edit box.
*/
static int edit_selenabled(ADIME_EDITBOX *ed)
{
   return ed->options->clipboard_functions != EDIT_NOSELECT;
}



/* edit_hassel:
   Returns nonzero if text is selected in the edit box.
*/
static int edit_hassel(ADIME_EDITBOX *ed)
{
   ADIME_ASSERT(ed != NULL, ("The edit box was NULL"));
   return edit_selenabled(ed) && (ed->sellength > 0);
}



/* edit_cancopy:
   Returns nonzero if it is possible to copy text from the edit box.
*/
static int edit_cancopy(ADIME_EDITBOX *ed)
{
   ADIME_ASSERT(ed != NULL, ("The edit box was NULL"));
   return edit_selenabled(ed) && ed->options->clipboard;
}



/* edit_canpaste:
   Returns nonzero if it is possible to paste text from clipboard to the edit
   box.
*/
static int edit_canpaste(ADIME_EDITBOX *ed)
{
   ADIME_ASSERT(ed != NULL, ("The edit box was NULL"));
   return edit_cancopy(ed) && (!ed->options->write_protected) &&
          clipboard_has_type(ed->options->clipboard, CLIPBOARD_TYPE_TEXT);
}



/* adime_normal_edit:
UNOK
   The function that selects what action to take place.
*/
int adime_normal_edit(ADIME_EDITBOX *ed, int key, int shifts)
{
   int ret = 0;

   ADIME_ASSERT(ed != NULL, ("The edit box was NULL"));

   /* Normal ascii-keypresses. */
   if ((key & 255) >= 32)
      ret |= ed->options->funcs->key_text(ed, key, shifts);
   /* Non-text-keypresses, e.g. cursor movements, delete etc. */
   else
      ret |= ed->options->funcs->key_nontext(ed, key, shifts);

   return ret;
}



/* adime_normal_edit_nontext:
   Handles the usual cursor movement, with arrow keys and so on.
UNOK
*/
int adime_normal_edit_key_nontext(ADIME_EDITBOX *ed, int key, int shifts)
{
   ADIME_EDITBOX_FUNCTIONS *ef;
   int ret = 0;
   int select;

   ADIME_ASSERT(ed != NULL, ("The edit box was NULL"));

   ef = ed->options->funcs;
   select = shifts & KB_SHIFT_FLAG ? TRUE : FALSE;

   /* todo: unicode format of key? */
   switch (key >> 8) {
      /* Delete, clipboard and insert. */
      case KEY_BACKSPACE:
         ret |= ef->delete_left(ed);
         break;

      case KEY_DEL:
         if (shifts & KB_SHIFT_FLAG)
            ret |= ef->clip_cut(ed);
         else
            ret |= ef->delete_right(ed);
         break;

      case KEY_INSERT:
         if (shifts & KB_SHIFT_FLAG) {
            /* Both ctrl and shift is ambigous, so we'd better not do
               anything. */
            if (shifts & KB_CTRL_FLAG)
               ret |= ADIME_EDIT_CHANGE_KEYIGNORED;
            else
               ret |= ef->clip_paste(ed);
         }
         else if (shifts & KB_CTRL_FLAG)
            ret |= ef->clip_copy(ed);
         else
            ret |= ef->toggle_insertmode(ed);
         break;

      /* Cursor movement. */
      case KEY_LEFT:
         if (edit_hassel(ed) && !select) {
            ef->pos_move(ed, ed->selstart, FALSE);
            /* todo return value */
            ret |= ADIME_EDIT_CHANGE_SEL;
         }
         else {
            if (shifts & KB_CTRL_FLAG)
               ret |= ef->pos_word_left(ed, ed->pos, select);
            else
               ret |= ef->pos_left(ed, select);
         }
         break;

      case KEY_RIGHT:
         if (edit_hassel(ed) && !select) {
            ef->pos_move(ed, ed->selstart + ed->sellength, FALSE);
            /* todo return value */
            ret |= ADIME_EDIT_CHANGE_SEL;
         }
         else {
            if (shifts & KB_CTRL_FLAG)
               ret |= ef->pos_word_right(ed, ed->pos, select);
            else
               ret |= ef->pos_right(ed, select);
         }
         break;

      case KEY_HOME:
         if (edit_hassel(ed) && !select) {
            ed->pos = ed->selstart;
            /* todo return */
            ret |= ADIME_EDIT_CHANGE_POS;
         }
         else
            ret |= ef->pos_home(ed, select);
         break;

      case KEY_END:
         if (edit_hassel(ed) && !select) {
            ed->pos = ed->selstart + ed->sellength;
            /* todo return */
            ret |= ADIME_EDIT_CHANGE_POS;
         }
         else
            ret |= ef->pos_end(ed, select);
         break;

      /* For numedit: increment and decrement. */
      case KEY_UP:
         if (ed->options->type == ADIME_EDIT_TYPE_NUM)
            ret |= ef->num_increase(ed);
         else
            ret |= ADIME_EDIT_CHANGE_KEYIGNORED;
         break;

      case KEY_DOWN:
         if (ed->options->type == ADIME_EDIT_TYPE_NUM)
            ret |= ef->num_decrease(ed);
         else
            ret |= ADIME_EDIT_CHANGE_KEYIGNORED;
         break;

      case KEY_PGUP:
         if (ed->options->type == ADIME_EDIT_TYPE_NUM)
            ret |= ef->num_big_increase(ed);
         else
            ret |= ADIME_EDIT_CHANGE_KEYIGNORED;
         break;

      case KEY_PGDN:
         if (ed->options->type == ADIME_EDIT_TYPE_NUM)
            ret |= ef->num_big_decrease(ed);
         else
            ret |= ADIME_EDIT_CHANGE_KEYIGNORED;
         break;

      default:
         ret |= ADIME_EDIT_CHANGE_KEYIGNORED;
         break;
   }

   return ret;
}



#if 0
/* adime_unix_edit_nontext:
   Handles UNIX style cursor movement.
UNOK
*/
int adime_unix_edit_key_nontext(ADIME_EDITBOX *ed, int key, int shifts)
{
   ADIME_EDITBOX_FUNCTIONS *ef;
   int ret = 0;

   ADIME_ASSERT(ed != NULL, ("The edit box was NULL"));

   ef = ed->options->funcs;

   /* todo: implement */

   return ret;
}
#endif



/* adime_normal_edit_replace_text:
   Low level boss function that will always be called for inserting,
   overwriting or deleting text.
UOK
*/
int adime_normal_edit_replace_text(ADIME_EDITBOX *ed, char *text, int start, int length)
{
   int ret = 0;
   int before_bytes, replace_bytes, after_bytes, zero_bytes, newtext_bytes;
   int before_pixels, newtext_pixels;
   int newtext_length;
   int tmp, last;
   int original_bytes;

   /* Check everything is OK. */
   ADIME_ASSERT(ed != NULL, ("The edit box was NULL"));

   if (ed->options->write_protected)
      return ADIME_EDIT_CHANGE_WRITEPROTECTED;

   if (text == NULL)
      text = EMPTY_STRING;

   /* Initialize lots of variables. */
   before_bytes = uoffset(ed->text, start);
   replace_bytes = uoffset(ed->text + before_bytes, length);
   after_bytes = ustrsize(ed->text + before_bytes + replace_bytes);
   zero_bytes = ustrsizez(ed->text +
                          before_bytes + replace_bytes + after_bytes);
   newtext_bytes = ustrsize(text);

   original_bytes = before_bytes + replace_bytes + after_bytes;
   newtext_length = ustrlen(text);
   newtext_pixels = adime_substr_width(ed->options->font, text, 0,
                                      newtext_bytes);

   if (length > 0)
      ret |= ADIME_EDIT_CHANGE_TEXTOVERWRITTEN;

   /* Move selection and pos. */
   /* todo: return shit */

   if (ed->selstart > start) {
      /* Whole selection after replaced text. */
      if (ed->selstart >= start + length) {
         ed->selstart += newtext_length - length;
         ed->pos += newtext_length - length;
      /* Selection begins inside replaced text. */
      }
      else {
         if (ed->sellength) {
            /* Selection ends outside replaced text. */
            if (ed->selstart + ed->sellength > start + length) {
               ed->sellength = ed->selstart + ed->sellength +
                               newtext_length - length - start;
               if (ed->pos == ed->selstart)
                  ed->pos = start;
               else
                  ed->pos = start + ed->sellength;
               ed->selstart = start;
            /* Selection is contained in replaced text. */
            }
            else {
               if (ed->pos == ed->selstart)
                  ed->pos = start;
               else
                  ed->pos = start + newtext_length;
               ed->selstart = start;
               ed->sellength = newtext_length;
            }
         }
         else
            ed->pos = ed->selstart = start;
      }
   }
   else if (ed->selstart + ed->sellength > start) {
      /* Selection covers replaced text. */
      if (ed->selstart + ed->sellength > start + length) {
         ed->sellength += newtext_length - length;
         if (ed->pos != ed->selstart)
            ed->pos = ed->selstart + ed->sellength;
      /* Selection begins before and ends inside replaced text. */
      }
      else {
         ed->sellength = start + newtext_length - ed->selstart;
         if (ed->pos != ed->selstart)
            ed->pos = start + newtext_length;
      }
   }

   /* Check max character length. */
   if (ed->options->max_length >= 0) {
      if (start + newtext_length +
          ustrlen(ed->text + before_bytes + replace_bytes) >
          ed->options->max_length) {
         /* If even the new text is too long. */
         if (start + newtext_length > ed->options->max_length) {
            newtext_bytes = uoffset(text, ed->options->max_length - start);
            after_bytes = 0;
            replace_bytes = 0;
         /* Only old text lost. */
         }
         else {
            after_bytes = uoffset(ed->text + before_bytes + replace_bytes,
                                  ed->options->max_length - before_bytes -
                                                              newtext_bytes);
         }
         ret |= ADIME_EDIT_CHANGE_TEXTOVERFLOWN;
      }
   }

   /* Check max pixel width. */
   if (ed->options->max_pixel_width >= 0) {
      before_pixels = adime_substr_width(ed->options->font, ed->text, 0,
                                        start);

      if (before_pixels + newtext_pixels +
         adime_substr_width(ed->options->font,
                           ed->text + before_bytes + replace_bytes, 0,
                           after_bytes) > ed->options->max_pixel_width) {
         /* If even the new text is greater than permitted. */
         if (before_pixels + newtext_pixels > ed->options->max_pixel_width) {
            tmp = char_at_pixel(ed->options->font, text,
                                ed->options->max_pixel_width -before_pixels);
            newtext_bytes = uoffset(text, tmp);
            after_bytes = 0;
         /* Only old text lost. */
         }
         else {
            tmp = char_at_pixel(ed->options->font,
                                ed->text + before_bytes + replace_bytes,
                                ed->options->max_pixel_width -
                                 before_pixels - newtext_pixels);
            after_bytes = uoffset(ed->text + before_bytes + replace_bytes,
                                  tmp);
         }
         ret |= ADIME_EDIT_CHANGE_TEXTOVERFLOWN;
      }
   }

   /* Check max byte size. */
   if (ed->options->max_bytes >= 0) {
      if (before_bytes + newtext_bytes + after_bytes + zero_bytes >
          ed->options->max_bytes) {
         /* If even the new text is greater then permitted. */
         if (before_bytes + newtext_bytes + zero_bytes >
             ed->options->max_bytes) {
            last = 0;
            for (tmp = uwidth(text);
                 before_bytes + tmp + zero_bytes <= ed->options->max_bytes;
                 tmp += uwidth(text + tmp))
               last = tmp;
            newtext_bytes = last;
            after_bytes = 0;
         /* Only old text lost. */
         }
         else {
            last = 0;
            for (tmp = uwidth(ed->text + before_bytes + replace_bytes);
                 before_bytes + newtext_bytes + tmp + zero_bytes <=
                                                      ed->options->max_bytes;
                 tmp += uwidth(ed->text + before_bytes +
                               replace_bytes + tmp))
               last = tmp;
            after_bytes = last;
         }
         ret |= ADIME_EDIT_CHANGE_TEXTOVERFLOWN;
      }
   }

   /* Finally, do the actual memmoving and memcopying. */

   if (replace_bytes <= newtext_bytes)
      edit_resize_text(ed, before_bytes + newtext_bytes + after_bytes +
                           zero_bytes);

   memmove(ed->text + before_bytes + newtext_bytes,
           ed->text + before_bytes + replace_bytes,
           after_bytes);
   memcpy(ed->text + before_bytes, text, newtext_bytes);
   memcpy(ed->text + before_bytes + newtext_bytes + after_bytes,
          EMPTY_STRING, zero_bytes);

   if (replace_bytes > newtext_bytes)
      edit_resize_text(ed, before_bytes + newtext_bytes + after_bytes +
                           zero_bytes);

   /* todo Find out what has changed. */
/* ret |= ADIME_EDIT_CHANGE_TEXT;

   if (original_bytes != before_bytes + newtext_bytes + after_bytes)
      ret |= ADIME_EDIT_CHANGE_BYTES;
   if (ret & ADIME_EDIT_CHANGE_OVERFLOWN) {
      ret |= ADIME_EDIT_CHANGE_LENGTH;

      ret |= ADIME_EDIT_CHANGE_PIXELLENGTH;
   }
   else {
      if (length != newtext_length)
         ret |= ADIME_EDIT_CHANGE_LENGTH;
      if (newtext_pixels != replace_pixels)
         ret |= ADIME_EDIT_CHANGE_PIXELLENGTH;
   }
*/
   return ret;
}



/* adime_normal_edit_key_text:
OK
*/
int adime_normal_edit_key_text(ADIME_EDITBOX *ed, int c, int shifts)
{
   int ret = 0;
   char text[16];
   int length;
   int new_pos;

   ADIME_UNUSED_ARG(shifts);

   if (ed->options->write_protected)
      return ADIME_EDIT_CHANGE_WRITEPROTECTED;

   /* todo: Check if c is in the font, fix Unicode */
   c &= 255;

   if (edit_hassel(ed)) {
      new_pos = ed->selstart + 1;
      usetc(text + usetc(text, c), 0);
      ret |= ed->options->funcs->replace_text(ed, text, ed->selstart,
                                              ed->sellength);
      ret |= ed->options->funcs->pos_move(ed, new_pos, FALSE);
      ret |= ADIME_EDIT_CHANGE_SHOWCURSOR | ADIME_EDIT_CHANGE_TEXTOVERWRITTEN;
   }
   else {
      /* If in overwrite mode, find out how many characters to overwrite. */
      if ((*ed->insert == ADIME_EDIT_OVERWRITE) ||
         (*ed->insert == ADIME_EDIT_ONLY_OVERWRITE)) {
         if (ugetc(ed->text + uoffset(ed->text, ed->pos)))
            length = 1;
         else
            length = 0;
      }
      else
         length = 0;
      usetc(text + usetc(text, c), 0);
      ret |= ed->options->funcs->replace_text(ed, text, ed->pos, length);
      /* todo: make this better */
      ret |= ed->options->funcs->pos_move(ed, ed->pos + 1, FALSE);
      ret |= ADIME_EDIT_CHANGE_SHOWCURSOR;
   }

   return ret;
}



/* adime_normal_edit_pos_move:
   Low level boss function handling all kinds of cursor movement.
*/
int adime_normal_edit_pos_move(ADIME_EDITBOX *ed, int pos, int select)
{
   int ret = 0;
   int text_len;

   ADIME_ASSERT(ed != NULL, ("The edit box was NULL"));

   if (ed->pos != pos)
      ret |= ADIME_EDIT_CHANGE_POS;

   /* Make position be in range. */
   text_len = ustrlen(ed->text);
   if (pos < 0)
      pos = 0;
   else if (pos > text_len)
      pos = text_len;


   /* Billions of cases depending on where we have the selection. */
   if (select) {
      ret |= ADIME_EDIT_CHANGE_SEL;
      if (ed->pos == ed->selstart) {
         if (pos < ed->selstart) {
            ed->sellength += ed->pos - pos;
            ed->selstart = pos;
         }
         else if (pos == ed->selstart) {
            ret &= ~ADIME_EDIT_CHANGE_SEL;
         }
         else if (pos <= ed->selstart + ed->sellength) {
            ed->sellength -= pos - ed->selstart;
            ed->selstart = pos;
         }
         else {
            ed->selstart += ed->sellength;
            ed->sellength = pos - ed->selstart;
         }
      }
      else {
         if (pos < ed->selstart) {
            ed->sellength = ed->selstart - pos;
            ed->selstart = pos;
         }
         else if (pos < ed->selstart + ed->sellength)
            ed->sellength = pos - ed->selstart;
         else if (pos == ed->selstart + ed->sellength)
            ret &= ~ADIME_EDIT_CHANGE_SEL;
         else
            ed->sellength = pos - ed->selstart;
      }
      ed->pos = pos;
   }
   else {
      if (ed->sellength)
         ret |= ADIME_EDIT_CHANGE_SEL;
      ed->pos = ed->selstart = pos;
      ed->sellength = 0;
   }

   /* todo: returns wrong things. */
   return ret;
}



/* adime_normal_edit_pos_left:
UOK
*/
int adime_normal_edit_pos_left(ADIME_EDITBOX *ed, int select)
{
   if (ed->pos > 0)
      return ed->options->funcs->pos_move(ed, ed->pos - 1, select);
   else
      return ADIME_EDIT_CHANGE_BEEP;
}



/* adime_normal_edit_pos_right:
UOK
*/
int adime_normal_edit_pos_right(ADIME_EDITBOX  *ed, int select)
{
   if (ed->pos < ustrlen(ed->text))
      return ed->options->funcs->pos_move(ed, ed->pos + 1, select);
   else
      return ADIME_EDIT_CHANGE_BEEP;
}



/* adime_normal_edit_word_left:
UOK
*/
int adime_normal_edit_pos_word_left(ADIME_EDITBOX *ed, int from_pos, int select)
{
   int new_pos = 0;
   char *t = ed->text;
   int last_c, c;
   int i;

   /* todo: uis* don't exist in Allegro */
   if (from_pos == 0)
      return ADIME_EDIT_CHANGE_BEEP;

   c = ugetx(&t);
   for (i = 1; i < from_pos; i++) {
      last_c = c;
      c = ugetx(&t);
      if (uisspace(last_c) && !uisspace(c))
         new_pos = i;
      else if (!uisalpha(last_c) && uisalpha(c))
         new_pos = i;
      else if (uislower(last_c) && uisupper(c))
         new_pos = i;
   }

   return ed->options->funcs->pos_move(ed, new_pos, select);
}



/* adime_normal_edit_next_word_right:
UOK
   Calculates the position of the cursor after a word-right move.
*/
int adime_normal_edit_pos_word_right(ADIME_EDITBOX *ed, int from_pos, int select)
{
   char *t = ed->text + uoffset(ed->text, from_pos);
   int c, last_c;
   int new_pos;

   c = ugetx(&t);
   if (!c)
      return ADIME_EDIT_CHANGE_BEEP;

   new_pos = from_pos + 1;
   while (1) {
      last_c = c;
      c = ugetx(&t);
      if (!c)
         break;
      else if (!uisspace(last_c) && uisspace(c))
         break;
      else if (uisalpha(last_c) && !uisalpha(c))
         break;
      else if (uislower(last_c) && uisupper(c))
         break;
      new_pos++;
   }

   return ed->options->funcs->pos_move(ed, new_pos, select);
}



/* adime_normal_edit_pos_home:
UOK
*/
int adime_normal_edit_pos_home(ADIME_EDITBOX *ed, int select)
{
   int new_pos = 0;
   int c;
   char *t = ed->text;

   if (ed->pos == 0) {
      for (c = ugetx(&t); c == 32; c = ugetx(&t))
         new_pos++;
      if (new_pos == 0)
         return ADIME_EDIT_CHANGE_BEEP;
      else
         return ed->options->funcs->pos_move(ed, new_pos, select);
   }
   else
      return ed->options->funcs->pos_move(ed, 0, select);
}



/* adime_normal_edit_pos_end:
UOK
*/
int adime_normal_edit_pos_end(ADIME_EDITBOX *ed, int select)
{
   int c;
   int text_length = ustrlen(ed->text);
   char *t = ed->text;
   int i, last_nonempty = -1;

   if (ed->pos != text_length)
      return ed->options->funcs->pos_move(ed, text_length, select);
   else {
      for (i = 0; i < text_length; i++) {
         c = ugetx(&t);
         if (c != 32)
            last_nonempty = i;
      }
      if (last_nonempty == text_length - 1)
         return ADIME_EDIT_CHANGE_BEEP;
      else
         return ed->options->funcs->pos_move(ed, last_nonempty + 1, select);
   }
}



/* adime_normal_edit_delete_left:
UOK
*/
int adime_normal_edit_delete_left(ADIME_EDITBOX *ed)
{
   if (ed->options->write_protected)
      return ADIME_EDIT_CHANGE_WRITEPROTECTED;
   else if (edit_hassel(ed))
      return edit_delete_sel(ed);
   else if (ed->pos == 0)
      return ADIME_EDIT_CHANGE_BEEP;
   else
      return ed->options->funcs->replace_text(ed, NULL, ed->pos - 1, 1);
}



/* adime_normal_edit_delete_right:
UOK
*/
int adime_normal_edit_delete_right(ADIME_EDITBOX *ed)
{
   if (ed->options->write_protected)
      return ADIME_EDIT_CHANGE_WRITEPROTECTED;
   else if (edit_hassel(ed))
      return edit_delete_sel(ed);
   else if (ed->pos == ustrlen(ed->text))
      return ADIME_EDIT_CHANGE_BEEP;
   else
      return ed->options->funcs->replace_text(ed, NULL, ed->pos, 1);
}



/* adime_normal_edit_toggle_insertmode:
UOK
*/
int adime_normal_edit_toggle_insertmode(ADIME_EDITBOX *ed)
{
   if (*ed->insert == ADIME_EDIT_INSERT)
      *ed->insert = ADIME_EDIT_OVERWRITE;
   else if (*ed->insert == ADIME_EDIT_OVERWRITE)
      *ed->insert = ADIME_EDIT_INSERT;
   return ADIME_EDIT_CHANGE_INSERT;
}



/* adime_normal_edit_select_word_at_pos:
*/
int adime_normal_edit_select_word_at_pos(ADIME_EDITBOX *ed, int pos)
{
   if (edit_selenabled(ed)) {
      ed->options->funcs->pos_word_left(
       ed, ed->options->funcs->pos_at_pixel(ed, pos), FALSE);
      ed->options->funcs->pos_word_right(ed, ed->pos, TRUE);
      /* todo: make return value be good. */
      return ADIME_EDIT_CHANGE_SEL;
   }
   else
      return ADIME_EDIT_CHANGE_INVALID_OPERATION;
}



/* adime_normal_edit_pos_at_pixel:
*/
int adime_normal_edit_pos_at_pixel(ADIME_EDITBOX *ed, int pixel)
{
   return char_at_pixel(ed->options->font, ed->text, pixel);
}



/* adime_normal_edit_clip_copy:
*/
int adime_normal_edit_clip_copy(ADIME_EDITBOX *ed)
{
   int before_bytes, selection_bytes, zero_bytes;
   char *data;

   if (!edit_hassel(ed))
      return ADIME_EDIT_CHANGE_INVALID_OPERATION;

   before_bytes = uoffset(ed->text, ed->selstart);
   selection_bytes = uoffset(ed->text + before_bytes, ed->sellength);
   zero_bytes = ucwidth(0);

   ADIME_MALLOC(data, selection_bytes + zero_bytes, char);
   memcpy(data, ed->text + before_bytes, selection_bytes);
   memcpy(data + selection_bytes, EMPTY_STRING, zero_bytes);

   clipboard_put_text(ed->options->clipboard, data);

   /* todo: return the right stuff */
   return ADIME_EDIT_CHANGE_CLIPBOARD_OPERATION | ADIME_EDIT_CHANGE_CLIPBOARD_CONTENTS;
}



/* adime_normal_edit_cut:
*/
int adime_normal_edit_clip_cut(ADIME_EDITBOX *ed)
{
   ed->options->funcs->clip_copy(ed);
   edit_delete_sel(ed);
   /* todo: what to return */
   return 0;
}



/* adime_normal_edit_clip_paste:
*/
int adime_normal_edit_clip_paste(ADIME_EDITBOX *ed)
{
   int ret = 0;
   char *clipboard_text;

   ADIME_ASSERT(ed != NULL, ("The edit box was NULL"));

   if (ed->options->write_protected)
      return ADIME_EDIT_CHANGE_WRITEPROTECTED;
   else if (!edit_canpaste(ed))
      return ADIME_EDIT_CHANGE_INVALID_OPERATION;

   clipboard_text = clipboard_get_data(ed->options->clipboard,
                                       CLIPBOARD_TYPE_TEXT);
   ret |= ed->options->funcs->replace_text(ed, clipboard_text,
                                           ed->selstart, ed->sellength);
   ret |= ed->options->funcs->pos_move(ed, ed->selstart +
                                           ustrlen(clipboard_text), FALSE);

   return ret;
}



/* adime_normal_edit_set_text:
*/
int adime_normal_edit_set_text(ADIME_EDITBOX *ed, char *text)
{
   ed->options->funcs->replace_text(ed, text, 0, ustrlen(ed->text));
   ed->selstart = ed->pos = ustrlen(ed->text);
   ed->sellength = 0;
   /* todo: return value must be good */
   return ADIME_EDIT_CHANGE_TEXT | ADIME_EDIT_CHANGE_POS;
}



/* adime_normal_edit_create:
*/
ADIME_EDITBOX *adime_normal_edit_create(ADIME_EDITBOX_OPTIONS *eo, char *text)
{
   ADIME_EDITBOX *ret;

   ADIME_ASSERT(eo != NULL, ("The edit box options was NULL"));

   ADIME_MALLOC(ret, 1, ADIME_EDITBOX);

   if (text != NULL) {
      ret->text = text;
      ret->alloced_size = 0;
   }
   else {
      ADIME_MALLOC(ret->text, eo->default_bytes, char);
      ret->alloced_size = eo->default_bytes;
      memset(ret->text, 0, ret->alloced_size);
   }
   /* Not implemented yet.
   ret->num = NULL; */
   ret->selstart = ret->pos = ret->sellength = 0;
   ret->insert = _edit_default_insert;
   ret->options = eo;

   return ret;
}



/* adime_normal_edit_destroy:
*/
void adime_normal_edit_destroy(ADIME_EDITBOX *ed)
{
   if (ed != NULL) {
      if (ed->options->free_text_on_destroy)
         free(ed->text);
      free(ed);
   }
}



/* adime_normal_edit_num_increase:
*/
int adime_normal_edit_num_increase(ADIME_EDITBOX *ed)
{
   ADIME_UNUSED_ARG(ed);
   ADIME_ASSERT(0, ("Not yet implemented"));
   return 0;
}



/* adime_normal_edit_num_decrease:
*/
int adime_normal_edit_num_decrease(ADIME_EDITBOX *ed)
{
   ADIME_UNUSED_ARG(ed);
   ADIME_ASSERT(0, ("Not yet implemented"));
   return 0;
}



/* adime_normal_edit_num_big_increase:
*/
int adime_normal_edit_num_big_increase(ADIME_EDITBOX *ed)
{
   ADIME_UNUSED_ARG(ed);
   ADIME_ASSERT(0, ("Not yet implemented"));
   return 0;
}



/* adime_normal_edit_num_big_decrease:
*/
int adime_normal_edit_num_big_decrease(ADIME_EDITBOX *ed)
{
   ADIME_UNUSED_ARG(ed);
   ADIME_ASSERT(0, ("Not yet implemented"));
   return 0;
}



/* uninstall_edit:
   Automatically called atexit if install_edit() has been called.
*/
void uninstall_edit(void)
{
   destroy_clipboard(_edit_clipboard);
   _remove_exit_func(uninstall_edit);
}



/* install_edit:
   Must be called before using the edit box stuff. Automatically called by
   adime_init(). This can't be statically initialized since it contains a
   union (even though DJGPP permits that, it won't be portable).
*/
int install_edit(void)
{
   _edit_default_insert = &_edit_insert;

   _edit_clipboard = create_clipboard();

   _adime_normal_edit_text.type = ADIME_EDIT_TYPE_TEXT;
   _adime_normal_edit_text.data.text.available_chars = ADIME_EDIT_TEXT_ALL;
   _adime_normal_edit_text.data.text.force_case = TRUE;
   _adime_normal_edit_text.max_length = -1;
   _adime_normal_edit_text.max_pixel_width = -1;
   _adime_normal_edit_text.max_bytes = -1;
   _adime_normal_edit_text.font = font;
   _adime_normal_edit_text.write_protected = FALSE;
   _adime_normal_edit_text.dynamic = TRUE;
   _adime_normal_edit_text.free_text_on_destroy = FALSE;
   _adime_normal_edit_text.default_bytes = 64;
   _adime_normal_edit_text.increase_bytes = 256;
   _adime_normal_edit_text.extra = NULL;
   _adime_normal_edit_text.clipboard_functions = ADIME_EDIT_CLIPBOARD;
   _adime_normal_edit_text.clipboard = _edit_clipboard;
   _adime_normal_edit_text.funcs = &_adime_normal_edit_funcs;
   ADIME_ADD_EXIT_FUNC(uninstall_edit);

   return 0;
}



/* edit_delete_sel:
   Deletes the selection in an editbox.
*/
int edit_delete_sel(ADIME_EDITBOX *ed)
{
   if (edit_hassel(ed))
      return ed->options->funcs->replace_text(ed, NULL,
                                              ed->selstart, ed->sellength);
   else
      return 0;
}



/* edit_select_all:
   Makes the selection contain
*/
int edit_select_all(ADIME_EDITBOX *ed)
{
   if (!edit_selenabled(ed))
      return ADIME_EDIT_CHANGE_INVALID_OPERATION;
   ed->selstart = 0;
   ed->sellength = ed->pos = ustrlen(ed->text);
   /* todo: make return good. */
   return ADIME_EDIT_CHANGE_SEL;
}



/* Not yet implemented stuff. */
#if 0
/* create_graphic_edit:
*/
GRAPHIC_EDIT *create_graphic_edit(ADIME_EDITBOX *ed, BITMAP *bmp,
                                  int x1, int y1, int x2, int y2)
{
}



/* draw_edit:
*/
int draw_edit(ADIME_EDITBOX *ed, BITMAP *bmp, int x1, int y1, int x2, int y2,
              int text_color, int text_background,
              int selected_color, int selected_background)
{
}
#endif
