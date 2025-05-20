/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dfloat.c:
   adime_dialogf() functions for all calculator floating point formats, 
   i.e., %float and %double.

   By Sven Sandberg

   See readme.txt for more information about Adime.


   float my_float;
   adime_dialogf(..., "Select a number:%float[]", &my_float);

   double my_double;
   adime_dialogf(..., "Select a number:%double[]", &my_double);
*/
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* _adime_create_floatingpoint:
*/
int _adime_create_floatingpoint(DIALOG *d, ADIME_CONST char *modifiers,
                                void *args, int is_double)
{
   /* todo: is this buffer big enough? is there any better method than using
      sprintf()? */
   char buf[1024];
   int i;
   int w, max_width, widest_digit, exponent_width;
   ADIME_EDIT_NUMBER *n;

   /* First parse format and set up edit box just like for the plain types.
   */
   _adime_create_pfloatingpoint(d, modifiers, args, is_double);
   n = (ADIME_EDIT_NUMBER *)d->dp2;
   d->proc = adime_d_double_calc_edit_proc;
   d++;

   /* Find max length of result. */
   widest_digit = 0;
   for (i = '0'; i <= '9'; i++) {
      w = adime_char_width(_ADIME_FONT, i);
      if (w > widest_digit)
         widest_digit = w;
   }

   max_width = widest_digit * 10 + adime_char_width(_ADIME_FONT, '.');
   if (n->min_val < 0)
      max_width += adime_char_width(_ADIME_FONT, '-');
   exponent_width = 0;
   if (n->min_val < -999990000) {
      sprintf(buf, "%.0f", n->min_val);
      if ((n->min_val == -HUGE_VAL) || (strlen(buf) > 11))
         exponent_width = text_length(_ADIME_FONT, "e+");
   }
   else if (n->max_val > 999990000) {
      sprintf(buf, "%.0f", n->max_val);
      if ((n->max_val == HUGE_VAL) || (strlen(buf) > 11))
         exponent_width = text_length(_ADIME_FONT, "e+");
   }
   if ((n->min_val < 0.0001) &&
       (n->max_val > -0.0001)) {
      if (text_length(_ADIME_FONT, "e-") > exponent_width)
         exponent_width = text_length(_ADIME_FONT, "e-");
   }
   if (exponent_width > widest_digit)
      max_width += exponent_width - widest_digit;

   /* Set up result object. */
   d->proc = adime_d_calc_edit_result_proc;
   d->x = d[-1].x + d[-1].w + 6;
   d->y = d[-1].y + 3;
   d->w = max_width;
   d->fg = _ADIME_TEXT_COLOR;
   d->bg = _ADIME_BACKGROUND_COLOR;
   _ADIME_MALLOC(d->dp, 256 * uwidth_max(U_CURRENT) + ucwidth(0), char);
   d->dp2 = _ADIME_FONT;

   return 0;
}



/* _adime_create_double:
*/
int _adime_create_double(DIALOG *d, ADIME_CONST char *desc,
                         ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_floatingpoint(d, modifiers, args, TRUE);
}



/* _adime_store_double:
*/
void _adime_store_double(DIALOG *d)
{
   *(double *)d->dp3 = _adime_clamp_calc(d, -HUGE_VAL, HUGE_VAL, FALSE);
}



/* _adime_create_float:
*/
int _adime_create_float(DIALOG *d, ADIME_CONST char *desc,
                        ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_floatingpoint(d, modifiers, args, FALSE);
}



/* _adime_store_float:
*/
void _adime_store_float(DIALOG *d)
{
   *(float *)d->dp3 = _adime_clamp_calc(d, -HUGE_VAL, HUGE_VAL, FALSE);
}
