/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   messbox.c:
   The message_box() and vmessage_box() functions.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/

#include <stdio.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* vmessage_box:
   Displays a message box with the specified title and text.
*/
int vmessage_box(char *title, char *buttons, char *format, va_list args)
{
   char *buttons2;
   int c, last_amp;
   int num_buttons;
   char full_text[MESSAGE_BOX_MAX_TEXT_LENGTH];
   int font_h, title_w, title_h;

   font_h = text_height(_adime_font);
   title_h = text_height(_adime_title_font);
   title_w = text_length(_adime_title_font, title);

   /* Count number of buttons. */
   buttons2 = buttons;
   last_amp = FALSE;
   num_buttons = 1;
   for (c = ugetx(&buttons2); c; c = ugetx(&buttons2)) {
      if ((c == ';') && !last_amp)
         num_buttons++;
      else if (c == '&')
         last_amp = !last_amp;
      else
         last_amp = FALSE;
   }

   /* Fixup text. */
   uvsprintf(full_text, format, args);

   /* Adjust width of dialog. */
}



/* message_box:
   Wrapper for taking the parameters as variable number of arguments, rather
   than putting them in a list.
*/
int message_box(char *title, char *buttons, char *format, ...)
{
   int ret;
   va_list args;

   va_start(args, format);
   ret = vmessage_box(title, buttons, format, args);
   va_end(args);

   return ret;
}
