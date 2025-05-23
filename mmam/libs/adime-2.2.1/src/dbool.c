/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dbool.c:
   adime_dialogf() functions for editing booleans as check boxes.

   By Sven Sandberg

   See readme.txt for more information about Adime.


   int my_int = 0;
   adime_dialogf(..., "Description%bool[]", &my_int);
*/
#include <stdarg.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* _adime_create_bool:
   dp3 = result
*/
int _adime_create_bool(DIALOG *d, ADIME_CONST char *desc,
                       ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);

   _ADIME_ASSERT(ugetc(modifiers) == 0,
                 ("Error in \"%s%%bool[%s]\": "
                  "No arguments must be given to %%greybool[].",
                  desc, modifiers));

   d->proc = adime_d_check_proc;
   d->w = d->h = MAX(12, MIN(d->w, text_height(_ADIME_FONT)));
   d->dp3 = args;

   _ADIME_ASSERT((*(int *)d->dp3 >= 0) && (*(int *)d->dp3 <= 2),
                 ("Error in \"%s%%bool[%s]\": "
                  "The value passed to %%bool[] "
                  "must be 0, 1 or 2 (it was %d).",
                  desc, modifiers, *(int *)d->dp3));

   return 0;
}



/* _adime_store_bool:
*/
void _adime_store_bool(DIALOG *d)
{
   *(int *)d->dp3 = (d->flags & D_SELECTED) ? TRUE : FALSE;
}



/* _adime_reset_bool:
*/
void _adime_reset_bool(DIALOG *d)
{
   d->flags = *(int *)d->dp3 ? D_SELECTED : 0;
}
