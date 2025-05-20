/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   ddialogf.c:
   adime_dialogf() functions for opening a new sub-dialogf.

   By Sven Sandberg

   See readme.txt for more information about Adime.


   int my_int;
   adime_dialogf(..., "Description%dialogf[Open dialog]",
                "Title of sub-dialog", x, y, edit_w,
                "New format string%bool[]", &my_int);
*/
#include <stdarg.h>
#include <stdlib.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* button_callback:
*/
static int button_callback(DIALOG *d)
{
   int result;

   _adime_dialogf_reset_dialog((_ADIME_DIALOGF_DIALOG *)d->dp3);
   result = _adime_dialogf_run((_ADIME_DIALOGF_DIALOG *)d->dp3);

   if (result < 0)
      _adime_dialogf_store_results((_ADIME_DIALOGF_DIALOG *)d->dp3);
   else
      _adime_dialogf_reset_dialog((_ADIME_DIALOGF_DIALOG *)d->dp3);

   return D_O_K;
}



/* dialogf_start_caller:
   Helper function that puts the ... args into an adime_va_list before
   passing to `_adime_dialogf_start()'.
*/
static _ADIME_DIALOGF_DIALOG *dialogf_start_caller(
 ADIME_CONST char *title, int x, int y, int w, ADIME_CONST char *format, ...)
{
   adime_va_list ap;
   _ADIME_DIALOGF_DIALOG *ret;

   adime_va_start(ap, format);
   ret = _adime_dialogf_start(title, x, y, w, format, ap);
   adime_va_end(ap);

   return ret;
}



/* _adime_count_dialogf:
*/
int _adime_count_dialogf(ADIME_CONST char *desc, ADIME_CONST char *modifiers,
                         adime_va_list args, void **args_out)
{
   char *title, *format;
   int x, y, w;

   _ADIME_UNUSED_ARG(desc);
   _ADIME_UNUSED_ARG(modifiers);

   title = adime_va_arg(args, char *);
   x = adime_va_arg(args, int);
   y = adime_va_arg(args, int);
   w = adime_va_arg(args, int);
   format = adime_va_arg(args, char *);

   *args_out = dialogf_start_caller(title, x, y, w,
                                    "%chain[]"
                                    ADIME_OK_CANCEL_BUTTON_FORMAT,
                                    format, args,
                                    _adime_ok_cancel_button_callback);

   return 1;
}



/* _adime_count_vdialogf:
*/
int _adime_count_vdialogf(ADIME_CONST char *desc,
                          ADIME_CONST char *modifiers,
                          adime_va_list args, void **args_out)
{
   char *title, *format;
   int x, y, w;
   void *sub_args;

   _ADIME_UNUSED_ARG(desc);
   _ADIME_UNUSED_ARG(modifiers);

   title = adime_va_arg(args, char *);
   x = adime_va_arg(args, int);
   y = adime_va_arg(args, int);
   w = adime_va_arg(args, int);
   format = adime_va_arg(args, char *);
   sub_args = adime_va_arg(args, void *);

   *args_out = dialogf_start_caller(title, x, y, w,
                                    "%chain[]"
                                    ADIME_OK_CANCEL_BUTTON_FORMAT,
                                    format, &sub_args,
                                    _adime_ok_cancel_button_callback);

   return 1;
}



/* _adime_create_dialogfs:
   Common function for creating %vdialogf[] and %dialogf[].
*/
int _adime_create_dialogfs(DIALOG *d, ADIME_CONST char *desc,
                           ADIME_CONST char *modifiers, void *args)
{
   int state;
   char *button_text;

   button_text = (char *)modifiers;
   state = _adime_get_string_arg(&modifiers, NULL);
   _ADIME_ASSERT(state & _ADIME_ARG_END,
                 ("Error in \"%s%%dialogf[%s]\": "
                  "Garbage found at end of modifier (']' expected).",
                  desc, modifiers));

   d->proc = adime_d_button_proc;
   d->d1 = ADIME_BUTTON_CALLBACK;
   d->h += 6;
   d->dp = button_text;
   d->dp2 = button_callback;
   d->dp3 = args;

   return 0;
}



/* _adime_store_dialogfs:
*/
void _adime_store_dialogfs(DIALOG *d)
{
   _adime_dialogf_store_results((_ADIME_DIALOGF_DIALOG *)d->dp3);
}



/* _adime_reset_dialogfs:
*/
void _adime_reset_dialogfs(DIALOG *d)
{
   _adime_dialogf_reset_dialog((_ADIME_DIALOGF_DIALOG *)d->dp3);
}



/* _adime_destroy_dialogfs:
*/
void _adime_destroy_dialogfs(DIALOG *d)
{
   _adime_dialogf_end((_ADIME_DIALOGF_DIALOG *)d->dp3);
}
