/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   calcedit.c:
   The dialog proc for a calculator, and its helpers.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/
#include <limits.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <allegro.h>

#ifdef ALLEGRO_MSVC
   #include <float.h>
#endif

#include "adime.h"
#include "adime/adimeint.h"

#undef ROUND
#define ROUND(x)  (((x) < 0) ? ((x) - 0.5) : ((x) + 0.5))

#undef MIN_VAL
#define MIN_VAL   (1e-300)



/* _adime_destroy_calc:
*/
void _adime_destroy_calc(DIALOG *d)
{
   _adime_destroy_plain(d);
   free(d[1].dp);
}



/* _adime_destroy_plain:
*/
void _adime_destroy_plain(DIALOG *d)
{
   free(d->dp);
   free(d->dp2);
}



/* _adime_clamp_calc:
   Helper function that takes care of the clamping.
*/
double _adime_clamp_calc(DIALOG *d, double min, double max, int round)
{
   double min_val, max_val;
   int error = 0;
   double val;
   ADIME_EDIT_NUMBER *info = d->dp2;

   min_val = MAX(info->min_val, min);
   max_val = MIN(info->max_val, max);
   val = adime_uevaluate(d->dp, &error);
   val = MID(min_val, val, max_val);
   return round ? ROUND(val) : val;
}



/* _adime_clamp_plain:
   Helper function that takes care of the clamping for plain numbers, ie
   the %pint etc formats.
*/
double _adime_clamp_plain(DIALOG *d, double min, double max, int round)
{
   double min_val, max_val;
   double val;
   ADIME_EDIT_NUMBER *info = d->dp2;
   char *p;

   min_val = MAX(info->min_val, min);
   max_val = MIN(info->max_val, max);
   for (p = d->dp; isspace(*p); p++)
      ; /* do nothing */

   if ((!info->is_signed) && (p[0] == '-'))
      val = 0;
   else {
      /* long long is a bit tricky to implement and doesn't work yet. */
      #ifdef ADIME_LONG_LONG
         if (info->is_signed)
            val = strtoll(p, NULL, 10);
         else
            val = strtoull(p, NULL, 10);
      #else
         if (info->is_signed)
            val = strtol(p, NULL, 10);
         else
            val = strtoul(p, NULL, 10);
      #endif
   }
   val = MID(min_val, val, max_val);
   return round ? ROUND(val) : val;
}



/* adime_uevaluate:
   The same as `adime_evaluate()', but it also accepts an empty string (or
   one consisting of only whitespace) as 0 rather than giving an error. Also,
   this supports Unicode. In addition, it doesn't accept NaN as a valid
   result.
*/
double adime_uevaluate(ADIME_CONST char *expression, int *error)
{
   double val;
   char buf[512];
   char *buf_p;
   int i, last_nonspace;

   buf_p = uconvert_toascii(expression, buf);
   if (buf_p != buf)
      buf_p = strcpy(buf, expression);
   while (isspace(*buf_p))
      buf_p++;
   last_nonspace = 0;
   for (i = 0; buf_p[i]; i++)
      if (!isspace(buf_p[i]))
         last_nonspace = i;
   buf_p[last_nonspace + 1] = 0;
   if (*buf_p) {
      val = adime_evaluate(buf_p, error, NULL);

      if (_adime_isnan(val)) {
         *error = 1;
         val = 0;
      }
      return val;
   }
   else
      return 0;
}



/* adime_double2string:
   Create a floating point number. It's a shame there is no way to do this
   properly with `printf()'!
   The number has at most 10 digits to the left of the decimal point, at most
   9 to the right of it, and at most 10 totally. If the absolute value of the
   number is >= 10000000000 or < 0.000001, it is written in exponential form
   instead. Exponential form has at most 6 digits in the mantissa, so that
   the total number of digits never exceeds 9.
*/
void adime_double2string(double x, char *buf)
{
   int i;
   char *s;

   if (fabs(x) > 1.1e+10) {
   /* Fake into the exponent case to avoid buffer overflow for huge
      numbers. */
      buf[0] = '1';
      s = buf;
   }
   else {
      if (fabs(x) < MIN_VAL)
         x = 0;
      sprintf(buf, "%+022.9f", x);
      s = buf + 1;
   }
   /* Small and big numbers are written with exponents. */
   if ((s[0] != '0') ||
       ((x != 0) && !strncmp("00000000000.00000", s, 16))) {
      sprintf(buf, "%.5e", x);
      s = buf;
   }
   else {
      /* Calculate the width of the field and reprintf it. */
      for (i = 0; s[i] == '0'; i++)
         ; /* do nothing. */
      if (s[i] == '.')
         i--;
      sprintf(buf, "%.*f", i-1, x);
      if (i > 1) {
         for (i = strlen(buf) - 1; buf[i] == '0'; i--)
            ; /* do nothing */
         if (buf[i] == '.')
            buf[i] = 0;
         else
            buf[i + 1] = 0;
      }
      s = buf;
   }
}



/* adime_udouble2string:
   Unicode-aware version of double2string.
*/
void adime_udouble2string(double val, char *buf)
{
   char tmp[512];

   adime_double2string(val, tmp);
   if (uconvert(tmp, U_ASCII, buf, U_CURRENT, 256) == tmp)
      ustrcpy(buf, tmp);
}



/* adime_d_double_calc_edit_proc:
   Dialog procedure for edit boxes with expressions in.
*/
int adime_d_double_calc_edit_proc(int msg, DIALOG *d, int c)
{
   double val;
   int color;
   int error;
   ADIME_EDIT_NUMBER *info;

   if (msg == MSG_DRAW) {
      /* Draw result object. */

      info = (ADIME_EDIT_NUMBER *)d->dp2;
      error = 0;
      val = adime_uevaluate(d->dp, &error);
      if (info->is_float)
         val = (double)(float)val;
      if (error || (val < info->min_val) || (val > info->max_val)) {
         color = 2;
         val = MID(info->min_val, val, info->max_val);
      }
      else
         color = 1;

      adime_udouble2string(val, info->result_object->dp);

      object_message(info->result_object, MSG_DRAW, color);
   }

   return adime_d_edit_proc(msg, d, c);
}



/* adime_d_int_calc_edit_proc:
   Dialog procedure for edit boxes with expressions in.

   dp3 = CALC_EDIT_STRUCT.
*/
int adime_d_int_calc_edit_proc(int msg, DIALOG *d, int c)
{
   double val;
   int error;
   char buf[256];
   ADIME_EDIT_NUMBER *info;
   int color;
   /* long long is a bit tricky to implement and doesn't work yet. */
   #ifdef ADIME_LONG_LONG
      LONG_LONG int_val;
   #else
      int int_val;
   #endif

   if (msg == MSG_DRAW) {
      /* Draw result object. */

      info = (ADIME_EDIT_NUMBER *)d->dp2;
      error = 0;
      val = adime_uevaluate(d->dp, &error);
      if (error || (val < info->min_val) || (val > info->max_val)) {
         color = 2;
         int_val = ROUND(MID(info->min_val, val, info->max_val));
      }
      else {
         color = 1;
         int_val = ROUND(val);
      }
      /* long long is a bit tricky to implement and doesn't work yet. */
      #ifdef ADIME_LONG_LONG
         if (info->is_signed)
            sprintf(buf, "%Ld", int_val);
         else
            sprintf(buf, "%Lu", int_val);
      #else
         if (info->is_signed)
            sprintf(buf, "%d", int_val);
         else
            sprintf(buf, "%u", int_val);
      #endif
      if (uconvert(buf, U_ASCII, info->result_object->dp, U_CURRENT,
                   sizeof(buf)) == buf)
         ustrcpy(info->result_object->dp, buf);
      object_message(info->result_object, MSG_DRAW, color);
   }

   return adime_d_edit_proc(msg, d, c);
}



/* adime_d_calc_edit_result_proc:
   Dialog proc for the result of a calculator edit box. The difference
   between this and adime_d_text_proc() is that it uses the c parameter to
   find the color to draw with, and it also erases its whole area even if
   the text doesn't fill it.
*/
int adime_d_calc_edit_result_proc(int msg, DIALOG *d, int c)
{
   if (msg == MSG_DRAW) {
      if (c == 1)
         d->fg = _ADIME_TEXT_COLOR;
      else if (c == 2)
         d->fg = _ADIME_ERROR_TEXT_COLOR;
      adime_fill_textout(_ADIME_BMP, _ADIME_FONT, d->dp, d->x, d->y, d->w,
                         d->fg, d->bg);
      return D_O_K;
   }
   else
      return d_text_proc(msg, d, c);
}
