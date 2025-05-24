/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   fsel.c:
   A 3d version of Allegro's file selector.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"




/* d_fsel3d_dummy_proc:
   A dummy proc used by gui_ctext_proc() which need to be hidden.
*/
static int d_fsel3d_dummy_proc(int msg, DIALOG *d, int c)
{
   _ADIME_UNUSED_ARG(msg);
   _ADIME_UNUSED_ARG(d);
   _ADIME_UNUSED_ARG(c);
   return D_O_K;
}



/* d_fsel3d_window_proc:
   Hook for drawing the background as a window.
*/
static int d_fsel3d_window_proc(int msg, DIALOG *d, int c)
{
   void *old_dp;
   int ret;

   /* This is a horrible hack that makes the alert() box that the file
      selector brings up on disk errors work. */
   old_dp = d->dp;
   if (ugetc((char *)d[1].dp) == 0)
      d->dp = d[2].dp;
   else
      d->dp = d[1].dp;

   ret = adime_d_window_proc(msg, d, c);

   d->dp = old_dp;
   return ret;
}



/* d_fsel3d_edit_proc:
   Hook for drawing the edit box in 3d.
*/
static int d_fsel3d_edit_proc(int msg, DIALOG *d, int c)
{
   int ret, fg = d->fg, bg = d->bg;
   int old_flags = d->flags;

   d->flags |= ADIME_D_EXTERNAL_BORDER;
   d->fg = _ADIME_TEXT_COLOR;
   d->bg = _ADIME_EDIT_FIELD_COLOR;
   ret = adime_d_edit_proc(msg, d, c);
   d->fg = fg;
   d->bg = bg;
   d->flags = old_flags;

   return ret;
}



/* adime_d_fsel_button_proc:
   Hook for drawing the buttons in 3d.
*/
static int d_fsel3d_button_proc(int msg, DIALOG *d, int c)
{
   int ret, fg = d->fg;
   d->fg = _ADIME_TEXT_COLOR;
   d->d1 = ADIME_BUTTON_EXIT;
   ret = adime_d_button_proc(msg, d, c);
   d->fg = fg;
   return ret;
}



/* d_fsel3d_text_list_proc:
   Hook for drawing text_lists in 3d.
*/
static int d_fsel3d_text_list_proc(int msg, DIALOG *d, int c)
{
   int ret, fg = d->fg, bg = d->bg;
   d->fg = _ADIME_TEXT_COLOR;
   d->bg = _ADIME_EDIT_FIELD_COLOR;
   ret = adime_d_text_list_proc(msg, d, c);
   d->fg = fg;
   d->bg = bg;
   return ret;
}



/* d_fsel3d_list_proc:
   Hook for drawing lists in 3d.
*/
static int d_fsel3d_list_proc(int msg, DIALOG *d, int c)
{
   int ret, fg = d->fg, bg = d->bg;
   d->fg = _ADIME_TEXT_COLOR;
   d->bg = _ADIME_EDIT_FIELD_COLOR;
   ret = adime_d_list_proc(msg, d, c);
   d->fg = fg;
   d->bg = bg;
   return ret;
}



/* adime_file_select:
   Like Allegro's `file_select_ex()', but with Adime's 3d look and feel.
   Also, doesn't touch `path' if it gets cancelled.
*/
int adime_file_select(ADIME_CONST char *message, char *path,
                      ADIME_CONST char *ext, int size, int w, int h)
{
   int ret;
   char *path_dup;

   #define SAVE_PROC(name) \
      int (*old_gui_##name##_proc)(int msg, DIALOG *d, int c) = \
       gui_##name##_proc;
   SAVE_PROC(shadow_box)
   SAVE_PROC(ctext)
   SAVE_PROC(button)
   SAVE_PROC(edit)
   SAVE_PROC(list)
   SAVE_PROC(text_list)
   #undef SAVE_PROC

   gui_shadow_box_proc = d_fsel3d_window_proc;
   gui_ctext_proc      = d_fsel3d_dummy_proc;
   gui_button_proc     = d_fsel3d_button_proc;
   gui_edit_proc       = d_fsel3d_edit_proc;
   gui_text_list_proc  = d_fsel3d_text_list_proc;
   gui_list_proc       = d_fsel3d_list_proc;

   _ADIME_MALLOC(path_dup, size, char);
   ustrzcpy(path_dup, size, path);

   ret = file_select_ex(message, path_dup, ext, size, w, h);

   if (ret)
      ustrzcpy(path, size, path_dup);
   free(path_dup);

   #define RESTORE_PROC(name) gui_##name##_proc = old_gui_##name##_proc;
   RESTORE_PROC(shadow_box)
   RESTORE_PROC(ctext)
   RESTORE_PROC(button)
   RESTORE_PROC(edit)
   RESTORE_PROC(list)
   RESTORE_PROC(text_list)
   #undef RESTORE_PROC

   return ret;
}
