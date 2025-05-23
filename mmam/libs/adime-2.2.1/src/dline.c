/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dline.c:
   adime_dialogf() functions for the %line format, which does nothing
   but display a horizontal delimiter line.

   By Sven Sandberg

   See readme.txt for more information about Adime.


   adime_dialogf(..., "%line[]");
*/
#include <stdarg.h>
#include <stdlib.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* _adime_create_line:
*/
int _adime_create_line(DIALOG *d, ADIME_CONST char *desc,
                       ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(args);
   _ADIME_ASSERT(ugetc(modifiers) == 0,
                 ("Error in \"%s%%line[%s]\": "
                  "No arguments must be given to %%line[].",
                  desc, modifiers));

   if (_adime_dialogf_pass_index == 0) {
      d->w = 0;
      d[1].w = 0;
      return 3;
   }
   else if (_adime_dialogf_pass_index == 1) {
      /* Create description when we know where left edge of dialog is. */
      _adime_dialogf_create_desc(d, d->y, desc);
      d->x = _adime_dialog->dialog->x + adime_window_border_thickness;
      d->d1 = ADIME_ALIGN_LEFT;
      return 2;
   }
   else if (!_adime_dialogf_final_pass)
      return 2;
   else {
      /* We need a final pass to create the line since the creation of
         a description for another line may have widened the dialog even
         more. */

      /* Create line. */
      d++;
      d->proc = adime_d_line_proc;
      d->x = d[-1].x;
      if (d[-1].w)
         d->x += d[-1].w + 3;
      d->y = d[-1].y + d[-1].h / 2;
      if (d[-1].h > 2)
         d->y--;
      d->w = _adime_dialog->dialog->x + _adime_dialog->dialog->w -
             adime_window_border_thickness - d->x;
      d->h = 2;
      return 0;
   }
}
