/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dchain.c:
   adime_dialogf() functions for the special %chain[] format.

   By Sven Sandberg

   See readme.txt for more information about Adime.


   int my_int;
   adime_dialogf(..., "%chain[]", "format", my_va_list);
*/
#include <stdarg.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* _adime_count_chain:
*/
int _adime_count_chain(ADIME_CONST char *desc, ADIME_CONST char *modifiers,
                       adime_va_list args, void **out_args)
{
   char *sub_format;
   adime_va_list sub_args;

   _ADIME_UNUSED_ARG(desc);
   _ADIME_UNUSED_ARG(modifiers);
   _ADIME_ASSERT(ugetc(modifiers) == 0,
                 ("Error in \"%s%%chain[%s]\": "
                  "No arguments must be given to %%chain[].",
                  desc, modifiers));

   sub_format = adime_va_arg(args, char *);
   sub_args = adime_va_arg(args, adime_va_list);

   _adime_dialogf_count_objects(_adime_dialog, sub_format, sub_args);

   return 0;
}
