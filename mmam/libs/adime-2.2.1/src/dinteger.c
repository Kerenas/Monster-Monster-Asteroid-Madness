/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dinteger.c:
   Functions for all calculator integer formats, i.e.,
   %int, %uint, %short, %ushort, %char, %uchar, %longlong, %ulonglong

   By Sven Sandberg

   See readme.txt for more information about Adime.


   int my_int = 0;
   adime_dialogf(..., "Number between -4711 and 471100:%int[-4711,471100]",
                &my_int);

   unsigned int my_uint = 0;
   adime_dialogf(..., "Positive number:%uint[,]",
                &my_uint);

   short my_short = 0;
   adime_dialogf(..., "Number between -4711 and 4711:%short[-4711,4711]",
                &my_short);

   unsigned short my_ushort = 0;
   adime_dialogf(..., "Select a number between 0 and 4711:%ushort[,4711]",
                &my_ushort);

   char my_char = 0;
   adime_dialogf(..., "Select a number between -47 and 11:%char[-47,11]",
                &my_char);

   unsigned char my_uchar = 0;
   adime_dialogf(..., "Select a number between 11 and 47:%uchar[11,47]",
                &my_uchar);
*/
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* _adime_create_integer:
*/
static int _adime_create_integer(DIALOG *d, ADIME_CONST char *modifiers,
                                 void *args, double min_val, double max_val)
{
   char buf[256];
   int i;
   int w, widest_digit;
   int result_max_len, len;
   ADIME_EDIT_NUMBER *n;

   /* First parse format and set up edit box just like for the plain types.
   */
   _adime_create_pinteger(d, modifiers, args, min_val, max_val);
   n = (ADIME_EDIT_NUMBER *)d->dp2;
   d->proc = adime_d_int_calc_edit_proc;
   d++;

   /* Find max length of result. */
   widest_digit = 0;
   for (i = '0'; i <= '9'; i++) {
      w = adime_char_width(_ADIME_FONT, i);
      if (w > widest_digit)
         widest_digit = w;
   }
   w = adime_char_width(_ADIME_FONT, '-');
   if (w > widest_digit)
      widest_digit = w;

   /* long long is a bit tricky to implement and doesn't work yet. */
   #ifdef _ADIME_LONG_LONG
      if (min_val < -1)
         sprintf(buf, "%lld", (LONG_LONG)n->max_val);
      else
         sprintf(buf, "%llu", (unsigned LONG_LONG)n->max_val);
      result_max_len = strlen(buf);

      if (min_val < -1)
         sprintf(buf, "%lld", (LONG LONG)n->min_val);
      else
         sprintf(buf, "%llu", (unsigned LONG_LONG)n->min_val);
      len = strlen(buf);
      if (len > result_max_len)
         result_max_len = len;
   #else
      if (min_val < -1)
         sprintf(buf, "%d", (int)n->max_val);
      else
         sprintf(buf, "%u", (unsigned int)n->max_val);
      result_max_len = strlen(buf);

      if (min_val < -1)
         sprintf(buf, "%d", (int)n->min_val);
      else
         sprintf(buf, "%u", (unsigned int)n->min_val);
      len = strlen(buf);
      if (len > result_max_len)
         result_max_len = len;
   #endif

   /* Set up result object. */
   d->proc = adime_d_calc_edit_result_proc;
   d->x = d[-1].x + d[-1].w + 6;
   d->y = d[-1].y + 3;
   d->w = widest_digit * result_max_len;
   d->fg = _ADIME_TEXT_COLOR;
   d->bg = _ADIME_BACKGROUND_COLOR;
   _ADIME_MALLOC(d->dp, result_max_len * uwidth_max(U_CURRENT) + ucwidth(0),
                 char);
   d->dp2 = _ADIME_FONT;

   return 0;
}



/* _adime_create_int:
*/
int _adime_create_int(DIALOG *d, ADIME_CONST char *desc,
                     ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_integer(d, modifiers, args, INT_MIN, INT_MAX);
}



/* _adime_store_int:
*/
void _adime_store_int(DIALOG *d)
{
   *(int *)d->dp3 = _adime_clamp_calc(d, INT_MIN, INT_MAX, TRUE);
}



/* _adime_create_uint:
*/
int _adime_create_uint(DIALOG *d, ADIME_CONST char *desc,
                      ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_integer(d, modifiers, args, 0, UINT_MAX);
}



/* _adime_store_uint:
*/
void _adime_store_uint(DIALOG *d)
{
   *(unsigned int *)d->dp3 = _adime_clamp_calc(d, 0, UINT_MAX, TRUE);
}



/* _adime_create_short:
*/
int _adime_create_short(DIALOG *d, ADIME_CONST char *desc,
                       ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_integer(d, modifiers, args, SHRT_MIN, SHRT_MAX);
}



/* _adime_store_short:
*/
void _adime_store_short(DIALOG *d)
{
   *(short *)d->dp3 = _adime_clamp_calc(d, SHRT_MIN, SHRT_MAX, TRUE);
}



/* _adime_create_ushort:
*/
int _adime_create_ushort(DIALOG *d, ADIME_CONST char *desc,
                         ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_integer(d, modifiers, args, 0, USHRT_MAX);
}



/* _adime_store_ushort:
*/
void _adime_store_ushort(DIALOG *d)
{
   *(unsigned short *)d->dp3 = _adime_clamp_calc(d, 0, USHRT_MAX, TRUE);
}



/* _adime_create_char:
*/
int _adime_create_char(DIALOG *d, ADIME_CONST char *desc,
                      ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_integer(d, modifiers, args, CHAR_MIN, CHAR_MAX);
}



/* _adime_store_char:
*/
void _adime_store_char(DIALOG *d)
{
   *(char *)d->dp3 = _adime_clamp_calc(d, CHAR_MIN, CHAR_MAX, TRUE);
}



/* _adime_create_uchar:
*/
int _adime_create_uchar(DIALOG *d, ADIME_CONST char *desc,
                       ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_integer(d, modifiers, args, 0, UCHAR_MAX);
}



/* _adime_store_uchar:
*/
void _adime_store_uchar(DIALOG *d)
{
   *(unsigned char *)d->dp3 = _adime_clamp_calc(d, 0, UCHAR_MAX, TRUE);
}



/* long long is a bit tricky to implement and doesn't work yet. */
#ifdef _ADIME_LONG_LONG
/* _adime_create_longlong:
*/
int _adime_create_longlong(DIALOG *d, ADIME_CONST char *desc,
                          ADIME_CONST char *modifiers, void *args)
{
   return _adime_create_integer(d, modifiers, args,
                                LONG_LONG_MIN, LONG_LONG_MAX);
}



/* _adime_store_longlong:
*/
void _adime_store_longlong(DIALOG *d)
{
   *(LONG_LONG *)d->dp3 = _adime_clamp_calc(d, LONG_LONG_MIN, LONG_LONG_MAX,
                                           TRUE);
}



/* _adime_create_ulonglong:
*/
int _adime_create_ulonglong(DIALOG *d, ADIME_CONST char *desc,
                           ADIME_CONST char *modifiers, void *args)
{
   return _adime_create_integer(d, modifiers, args, 0, ULONG_LONG_MAX);
}



/* _adime_store_ulonglong:
*/
void _adime_store_ulonglong(DIALOG *d)
{
   *(unsigned LONG_LONG *)d->dp3 = _adime_clamp_calc(d, 0, ULONG_LONG_MAX,
                                                    TRUE);
}
#endif
