/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dbutton.c:
   adime_dialogf() functions for a button which calls a custom callback
   function.

   By Sven Sandberg

   See readme.txt for more information about Adime.


   int my_int;
   adime_dialogf(..., "Description%button[Button text]", my_callback);
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
   int ret;

   if (d->dp) {
      ret = ((int (*)(DIALOG *))d->dp3)(d);
      if (ret == ADIME_REDRAW)
         return D_REDRAW;
      else if (ret) {
         _adime_dialog->return_value = ret;
         return D_CLOSE;
      }
      else
         return D_O_K;
   }
   else
      return D_O_K;
}



/* _adime_create_button:
*/
int _adime_create_button(DIALOG *d, ADIME_CONST char *desc,
                         ADIME_CONST char *modifiers, void *args)
{
   int state;
   char *button_text;

   _ADIME_UNUSED_ARG(desc);

   /* Parse parameters. */
   button_text = (char *)modifiers;
   state = _adime_get_string_arg(&modifiers, NULL);
   _ADIME_ASSERT_ARG_END("button");

   d->proc = adime_d_button_proc;
   d->d1 = ADIME_BUTTON_CALLBACK;
   d->h += 6;
   d->dp = button_text;
   d->dp2 = button_callback;
   d->dp3 = args;

   return 0;
}
