/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   adime.c:
   Some global variables that might be common to adime_dialogf() and
   message_box(), and some generic functions.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <allegro.h>
#include <allegro/internal/aintern.h>

#include "adime.h"
#include "adime/adimeint.h"



_ADIME_DIALOGF_FORMAT *_adime_dialogf_formats = NULL;

int _adime_inited = FALSE;



/* Colors of different parts of the dialog. */
RGB adime_text_rgb             = { 0,   0,   0,   0 },
    adime_disabled_text_rgb    = { 128, 128, 128, 0 },
    adime_error_text_rgb       = { 255, 0,   0,   0 };
RGB adime_border_rgb           = { 0,   0,   0,   0 };
RGB adime_background_rgb       = { 192, 192, 192, 0 },
    adime_edit_field_rgb       = { 255, 255, 255, 0 },
    adime_button_rgb           = { 192, 192, 192, 0 };
RGB adime_dark_shadow_rgb      = { 0,   0,   0,   0 },
    adime_shadow_rgb           = { 128, 128, 128, 0 },
    adime_highlight_rgb        = { 192, 192, 192, 0 },
    adime_light_highlight_rgb  = { 255, 255, 255, 0 };
RGB adime_title_text_rgb       = { 255, 255, 255, 0 },
    adime_title_background_rgb = { 0,   0,   128, 0 },
    adime_title_shadow_rgb     = { 0,   0,   0,   0 },
    adime_title_highlight_rgb  = { 255, 255, 255, 0 };

/* Screen bitmap to draw on. */
BITMAP *adime_bmp = NULL;

/* Fonts to be used in dialogs. */
FONT *adime_font = NULL, *adime_title_font = NULL, *adime_button_font = NULL;

/* Clear this flag if you don't want to yield timeslices. */
int adime_yield = TRUE;
/* Clear this flag if you don't want the dialog to clean up the background
   after it has finished. */
int adime_clean_screen = TRUE;

/* Window measurement variables. */
int adime_window_border_thickness = 5;
int adime_window_title_spacing = 10;
int adime_window_title_border_thickness = 3,
    adime_window_title_internal_border_thickness = 3;
int adime_window_button_spacing = 10,
    adime_window_between_button_spacing = 10;
int adime_window_description_spacing = 5;
int adime_window_line_spacing = 5;
int adime_window_button_w = 60, adime_window_button_h = 20;

/* Clear this flag if you don't want to show the dialog window, only the
   objects in it. */
int adime_window_visible = TRUE;



/* _adime_count_void_and_return_1:
   Dummy function used by formats that always create one object.
*/
int _adime_count_void_and_return_1(ADIME_CONST char *desc,
                                   ADIME_CONST char *modifiers,
                                   adime_va_list args, void **saved_args)
{
   _ADIME_UNUSED_ARG(desc);
   _ADIME_UNUSED_ARG(modifiers);
   _ADIME_UNUSED_ARG(args);
   _ADIME_UNUSED_ARG(saved_args);
   return 1;
}



/* _adime_count_void_and_return_2:
   Dummy function used by formats that always create two objects.
*/
int _adime_count_void_and_return_2(ADIME_CONST char *desc,
                                   ADIME_CONST char *modifiers,
                                   adime_va_list args, void **saved_args)
{
   _ADIME_UNUSED_ARG(desc);
   _ADIME_UNUSED_ARG(modifiers);
   _ADIME_UNUSED_ARG(args);
   _ADIME_UNUSED_ARG(saved_args);
   return 2;
}



/* _adime_count_pointer_and_return_1:
   Dummy function used by formats that always create one object.
*/
int _adime_count_pointer_and_return_1(ADIME_CONST char *desc,
                                      ADIME_CONST char *modifiers,
                                      adime_va_list args, void **saved_args)
{
   _ADIME_UNUSED_ARG(desc);
   _ADIME_UNUSED_ARG(modifiers);
   *saved_args = adime_va_arg(args, void *);
   return 1;
}



/* _adime_count_pointer_and_return_2:
   Dummy function used by formats that always create two objects.
*/
int _adime_count_pointer_and_return_2(ADIME_CONST char *desc,
                                      ADIME_CONST char *modifiers,
                                      adime_va_list args, void **saved_args)
{
   _ADIME_UNUSED_ARG(desc);
   _ADIME_UNUSED_ARG(modifiers);
   *saved_args = adime_va_arg(args, void *);
   return 2;
}



/* _adime_destroy_dp:
*/
void _adime_destroy_dp(DIALOG *d)
{
   free(d->dp);
}



/* _adime_register_dialogf_format:
   Registers a new format specifier.
*/
void _adime_register_dialogf_format(
 char *specifier,
 int handle_desc,
 int (*count_dialog)(ADIME_CONST char *desc, ADIME_CONST char *modifiers,
                     adime_va_list args, void **saved_args),
 int (*create_dialog)(DIALOG *dialog, ADIME_CONST char *desc,
                      ADIME_CONST char *modifiers, void *args),
 void (*store_dialog)(DIALOG *object),
 void (*reset_dialog)(DIALOG *object),
 void (*destroy_dialog)(DIALOG *object))
{
   _ADIME_DIALOGF_FORMAT *f;

   /* Don't add already added formats: it is valid to add a format twice. */
   for (f = _adime_dialogf_formats; f != NULL; f = f->next)
      if (strcmp(specifier, f->specifier) == 0)
         return;

   _ADIME_MALLOC(f, 1, _ADIME_DIALOGF_FORMAT);

   f->specifier = specifier;
   f->handle_desc = handle_desc;
   f->count_dialog = count_dialog;
   f->create_dialog = create_dialog;
   f->store_dialog = store_dialog;
   f->reset_dialog = reset_dialog;
   f->destroy_dialog = destroy_dialog;
   f->next = _adime_dialogf_formats;

   _adime_dialogf_formats = f;
}



/* adime_init:
   Registers all dialogf formats.
*/
int adime_init(void)
{
   int i;

   _adime_inited = TRUE;

   _ADIME_ASSERT(_allegro_count > 0,
                 ("You must call allegro_init() before adime_init()! "
                  "If you have done that, the error may also be that Adime "
                  "is compiled against a different version of Allegro than "
                  "your program. In that case, reinstall Adime!\n"));
   for (i = 0; _adime_dialogf_format_list[i].specifier != NULL; i++)
      _adime_register_dialogf_format(
       _adime_dialogf_format_list[i].specifier,
       _adime_dialogf_format_list[i].handle_desc,
       _adime_dialogf_format_list[i].count_dialog,
       _adime_dialogf_format_list[i].create_dialog,
       _adime_dialogf_format_list[i].store_dialog,
       _adime_dialogf_format_list[i].reset_dialog,
       _adime_dialogf_format_list[i].destroy_dialog);

   ADIME_ADD_EXIT_FUNC(adime_exit);

   return 0;
}



/* adime_exit:
   Frees the memory allocated by _adime_register_dialogf_format().
*/
void adime_exit(void)
{
   _ADIME_DIALOGF_FORMAT *iter, *next;

   for (iter = _adime_dialogf_formats; iter != NULL; iter = next) {
      next = iter->next;
      free(iter);
   }
   _adime_dialogf_formats = NULL;

   _remove_exit_func(adime_exit);

   _adime_inited = FALSE;
}
