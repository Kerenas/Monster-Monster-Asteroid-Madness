/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dnothing.c:
   adime_dialogf() functions for the %nothing format, which does nothing
   but display its description

   By Sven Sandberg

   See readme.txt for more information about Adime.


   adime_dialogf(..., "%nothing[]");
*/
#include <stdarg.h>
#include <stdlib.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* _adime_create_nothing:
*/
int _adime_create_nothing(DIALOG *d, ADIME_CONST char *desc,
                          ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(args);

   _ADIME_ASSERT(ugetc(modifiers) == 0,
                 ("Error in \"%s%%nothing[%s]\": "
                  "No arguments must be given to %%nothing[].",
                  desc, modifiers));

   if (_adime_dialogf_pass_index == 0) {
      d->w = 0;
      return 1;
   }
   else if (_adime_dialogf_pass_index == 1) {
      /* Create description when we know where left edge of dialog is. */
      _adime_dialogf_create_desc(d, d->y, desc);
      d->x = _adime_dialog->dialog->x + adime_window_border_thickness;
      d->d1 = ADIME_ALIGN_LEFT;
      return 0;
   }
   else
      _ADIME_ASSERT(0, ("NOTREACHED"));
   return 0;
}
