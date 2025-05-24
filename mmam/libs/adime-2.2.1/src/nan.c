/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   nan.c:
   Provides a semi-portable way to check for NaN.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/

#include <math.h>
#include <stdio.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* _adime_isnan:
   Checks if the given number is NaN.
*/
int _adime_isnan(double x)
{
/* This requires us to link with -lm, so I prefer the default test.
   #ifdef ALLEGRO_DJGPP

      return isnan(x);
*/

   #if defined ALLEGRO_MSVC

      return _isnan(x);

   #else

      char buf[256];

      /* This line should be compiled without optimization. */
      if (x != x)
         return TRUE;

      sprintf(buf, "%f", x);
      if (((buf[0] == 'n') || (buf[0] == 'N')) &&
          ((buf[1] == 'a') || (buf[1] == 'A')) &&
          ((buf[2] == 'n') || (buf[2] == 'N')))
         return TRUE;

      return FALSE;

   #endif
}
