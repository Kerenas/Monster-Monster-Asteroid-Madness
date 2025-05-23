/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dstring.c:
   adime_dialogf() functions for editing a text.

   By Sven Sandberg

   See readme.txt for more information about Adime.


   char name[1024];
   adime_dialogf(..., "What's your name?%string[256]", name);
*/
#include <stdarg.h>
#include <stdlib.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* _adime_create_string:
*/
int _adime_create_string(DIALOG *d, ADIME_CONST char *desc,
                         ADIME_CONST char *modifiers, void *args)
{
   int state;
   int size;

   _ADIME_UNUSED_ARG(desc);

   /* Parse parameters. */
   state = _adime_get_int_arg(&modifiers, &size);
   _ADIME_ASSERT(state & _ADIME_ARG_READ,
                 ("Error in \"%s%%string[%s]\": "
                  "You must specify a max length.",
                  desc, modifiers));
   _ADIME_ASSERT_ARG_END("string");

   d->proc = adime_d_edit_proc;
   d->h += 6;
   d->fg = _ADIME_TEXT_COLOR;
   d->bg = _ADIME_EDIT_FIELD_COLOR;
   d->d1 = -size;
   _ADIME_MALLOC(d->dp, size, char);
   d->dp3 = args;

   return 0;
}



/* _adime_store_string:
*/
void _adime_store_string(DIALOG *d)
{
   ustrzcpy((char *)d->dp3, -d->d1, (char *)d->dp);
}



/* _adime_reset_string:
*/
void _adime_reset_string(DIALOG *d)
{
   ustrzcpy((char *)d->dp, -d->d1, (char *)d->dp3);
}
