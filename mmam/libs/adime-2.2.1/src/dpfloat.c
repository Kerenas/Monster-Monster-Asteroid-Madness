/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dpfloat.c:
   adime_dialogf() functions for all plain floating point formats, i.e.,
   %pfloat and %pdouble.

   By Sven Sandberg

   See readme.txt for more information about Adime.


   float my_float;
   adime_dialogf(..., "Select a number:%pfloat[]", &my_float);

   double my_double;
   adime_dialogf(..., "Select a number:%pdouble[]", &my_double);
*/
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* _adime_create_pfloatingpoint:
*/
int _adime_create_pfloatingpoint(DIALOG *d, ADIME_CONST char *modifiers,
                                 void *args, int is_double)
{
   ADIME_EDIT_NUMBER *n;
   int state;
   double val;

   _ADIME_MALLOC(n, 1, ADIME_EDIT_NUMBER);

   /* Parse arguments. */
   state = _adime_get_double_arg(&modifiers, &val);
   _ADIME_ASSERT(state & _ADIME_ARG_COMMA,
                 ("',' expected in modifier: [%s]", modifiers));
   if (state & _ADIME_ARG_READ)
      n->min_val = val;
   else
      n->min_val = -HUGE_VAL;

   state = _adime_get_double_arg(&modifiers, &val);
   _ADIME_ASSERT(state & _ADIME_ARG_END,
                 ("Garbage at end of modifier (']' expected): [%s]",
                  modifiers));
   if (state & _ADIME_ARG_READ)
      n->max_val = val;
   else
      n->max_val = HUGE_VAL;

   n->result_object = d + 1;

   /* Set up object. */
   d->proc = adime_d_edit_proc;
   d->h += 6;
   d->fg = _ADIME_TEXT_COLOR;
   d->bg = _ADIME_EDIT_FIELD_COLOR;
   d->d1 = 256;
   _ADIME_MALLOC(d->dp, 256 * uwidth_max(U_CURRENT) + ucwidth(0), char);
   d->dp2 = n;

   d->dp3 = (void *)args;
   n->is_float = !is_double;

   return 0;
}



/* _adime_create_pdouble:
*/
int _adime_create_pdouble(DIALOG *d, ADIME_CONST char *desc,
                          ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_pfloatingpoint(d, modifiers, args, TRUE);
}



/* _adime_store_pdouble:
*/
void _adime_store_pdouble(DIALOG *d)
{
   *(double *)d->dp3 = _adime_clamp_plain(d, -HUGE_VAL, HUGE_VAL, FALSE);
}



/* _adime_reset_double:
*/
void _adime_reset_double(DIALOG *d)
{
   adime_udouble2string(*(double *)d->dp3, (char *)d->dp);
   d->d2 = ustrlen((char *)d->dp);
}



/* _adime_create_pfloat:
*/
int _adime_create_pfloat(DIALOG *d, ADIME_CONST char *desc,
                         ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_pfloatingpoint(d, modifiers, args, FALSE);
}



/* _adime_store_pfloat:
*/
void _adime_store_pfloat(DIALOG *d)
{
   *(float *)d->dp3 = _adime_clamp_plain(d, -HUGE_VAL, HUGE_VAL, FALSE);
}



/* _adime_reset_float:
*/
void _adime_reset_float(DIALOG *d)
{
   adime_udouble2string(*(float *)d->dp3, (char *)d->dp);
   d->d2 = ustrlen((char *)d->dp);
}
