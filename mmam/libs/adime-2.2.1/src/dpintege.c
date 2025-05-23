/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dpintege.c:
   adime_dialogf() functions common to all plain integer types.

   By Sven Sandberg

   See readme.txt for more information about Adime.


   int my_int = 0;
   adime_dialogf(..., "Number between -4711 and 471100:%pint[-4711,471100]",
                &my_int);

   unsigned int my_uint = 0;
   adime_dialogf(..., "Positive number:%uint[,]",
                &my_uint);

   short my_short = 0;
   adime_dialogf(..., "Number between -4711 and 4711:%pshort[-4711,4711]",
                &my_short);

   unsigned short my_ushort = 0;
   adime_dialogf(..., "Number between 0 and 4711:%pushort[,4711]",
                &my_ushort);

   char my_char = 0;
   adime_dialogf(..., "Number between -47 and 11:%pchar[-47,11]",
                &my_char);

   unsigned char my_uchar = 0;
   adime_dialogf(..., "Number between 11 and 47:%puchar[11,47]",
                &my_uchar);
*/
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* _adime_create_pinteger:
*/
int _adime_create_pinteger(DIALOG *d, ADIME_CONST char *modifiers,
                           void *args, double min_val, double max_val)
{
   ADIME_EDIT_NUMBER *n;
   int state;
   int val;

   _ADIME_MALLOC(n, 1, ADIME_EDIT_NUMBER);
   n->is_signed = min_val < -1 ? TRUE : FALSE;

   /* Parse arguments. */
   state = _adime_get_int_arg(&modifiers, &val);
   _ADIME_ASSERT(state & _ADIME_ARG_COMMA,
                 ("',' expected in modifier: [%s]", modifiers));
   if (state & _ADIME_ARG_READ)
      n->min_val = val;
   else
      n->min_val = min_val;

   state = _adime_get_int_arg(&modifiers, &val);
   _ADIME_ASSERT(state & _ADIME_ARG_END,
                 ("Garbage at end of modifier (']' expected): [%s]",
                  modifiers));
   if (state & _ADIME_ARG_READ)
      n->max_val = val;
   else
      n->max_val = max_val;

   n->result_object = d + 1;

   /* Set up object. */
   d->proc = adime_d_edit_proc;
   d->h += 6;
   d->fg = _ADIME_TEXT_COLOR;
   d->bg = _ADIME_EDIT_FIELD_COLOR;
   d->d1 = 256;
   _ADIME_MALLOC(d->dp, 256 * uwidth_max(U_CURRENT) + ucwidth(0), char);
   d->dp2 = n;
   d->dp3 = args;

   return 0;
}



/* _adime_create_pint:
*/
int _adime_create_pint(DIALOG *d, ADIME_CONST char *desc,
                       ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_pinteger(d, modifiers, args, INT_MIN, INT_MAX);
}



/* _adime_store_pint:
*/
void _adime_store_pint(DIALOG *d)
{
   *(int *)d->dp3 = _adime_clamp_plain(d, INT_MIN, INT_MAX, TRUE);
}



/* _adime_reset_int:
*/
void _adime_reset_int(DIALOG *d)
{
   usprintf((char *)d->dp, "%d", *(int *)d->dp3);
   d->d2 = ustrlen((char *)d->dp);
}



/* _adime_create_puint:
*/
int _adime_create_puint(DIALOG *d, ADIME_CONST char *desc,
                        ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_pinteger(d, modifiers, args, 0, UINT_MAX);
}



/* _adime_store_puint:
*/
void _adime_store_puint(DIALOG *d)
{
   *(unsigned int *)d->dp3 = _adime_clamp_plain(d, 0, UINT_MAX, TRUE);
}



/* _adime_reset_uint:
*/
void _adime_reset_uint(DIALOG *d)
{
   usprintf((char *)d->dp, "%u", *(unsigned int *)d->dp3);
   d->d2 = ustrlen((char *)d->dp);
}



/* _adime_create_pshort:
*/
int _adime_create_pshort(DIALOG *d, ADIME_CONST char *desc,
                         ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_pinteger(d, modifiers, args, SHRT_MIN, SHRT_MAX);
}



/* _adime_store_pshort:
*/
void _adime_store_pshort(DIALOG *d)
{
   *(short *)d->dp3 = _adime_clamp_plain(d, SHRT_MIN, SHRT_MAX, TRUE);
}



/* _adime_reset_short:
*/
void _adime_reset_short(DIALOG *d)
{
   usprintf((char *)d->dp, "%d", (int)*(short *)d->dp3);
   d->d2 = ustrlen((char *)d->dp);
}



/* _adime_create_pushort:
*/
int _adime_create_pushort(DIALOG *d, ADIME_CONST char *desc,
                          ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_pinteger(d, modifiers, args, 0, USHRT_MAX);
}



/* _adime_store_pushort:
*/
void _adime_store_pushort(DIALOG *d)
{
   *(unsigned short *)d->dp3 = _adime_clamp_plain(d, 0, USHRT_MAX, TRUE);
}



/* _adime_reset_ushort:
*/
void _adime_reset_ushort(DIALOG *d)
{
   usprintf((char *)d->dp, "%u", (int)*(unsigned short *)d->dp3);
   d->d2 = ustrlen((char *)d->dp);
}



/* _adime_create_pchar:
*/
int _adime_create_pchar(DIALOG *d, ADIME_CONST char *desc,
                        ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_pinteger(d, modifiers, args, CHAR_MIN, CHAR_MAX);
}



/* _adime_store_pchar:
*/
void _adime_store_pchar(DIALOG *d)
{
   *(char *)d->dp3 = _adime_clamp_plain(d, CHAR_MIN, CHAR_MAX, TRUE);
}



/* _adime_reset_char:
*/
void _adime_reset_char(DIALOG *d)
{
   usprintf((char *)d->dp, "%d", (int)*(char *)d->dp3);
   d->d2 = ustrlen((char *)d->dp);
}



/* _adime_create_puchar:
*/
int _adime_create_puchar(DIALOG *d, ADIME_CONST char *desc,
                         ADIME_CONST char *modifiers, void *args)
{
   _ADIME_UNUSED_ARG(desc);
   return _adime_create_pinteger(d, modifiers, args, 0, UCHAR_MAX);
}



/* _adime_store_puchar:
*/
void _adime_store_puchar(DIALOG *d)
{
   *(unsigned char *)d->dp3 = _adime_clamp_plain(d, 0, UCHAR_MAX, TRUE);
}



/* _adime_reset_uchar:
*/
void _adime_reset_uchar(DIALOG *d)
{
   usprintf((char *)d->dp, "%u", (int)*(unsigned char *)d->dp3);
   d->d2 = ustrlen((char *)d->dp);
}



/* long long is a bit tricky to implement and doesn't work yet. */
#ifdef _ADIME_LONG_LONG
/* _adime_create_plonglong:
*/
int _adime_create_plonglong(DIALOG *d, ADIME_CONST char *desc,
                            ADIME_CONST char *modifiers, void *args)
{
   return _adime_create_pinteger(d, modifiers, args,
                                 LONG_LONG_MIN, LONG_LONG_MAX);
}



/* _adime_store_plonglong:
*/
void _adime_store_plonglong(DIALOG *d)
{
   *(LONG_LONG *)d->dp3 = _adime_clamp_plain(d, LONG_LONG_MIN,
                                               LONG_LONG_MAX, TRUE);
}



/* _adime_reset_long_long:
*/
void _adime_reset_long_long(DIALOG *d)
{
   usprintf((char *)d->dp, "%Ld", *(LONG_LONG *)d->dp3);
   d->d2 = ustrlen((char *)d->dp);
}



/* _adime_create_pulonglong:
*/
int _adime_create_pulonglong(DIALOG *d, ADIME_CONST char *desc,
                             ADIME_CONST char *modifiers, void *args)
{
   return _adime_create_pinteger(d, modifiers, args, 0, ULONG_LONG_MAX);
}



/* _adime_store_pulonglong:
*/
void _adime_store_pulonglong(DIALOG *d)
{
   *(unsigned LONG_LONG *)d->dp3 = _adime_clamp_plain(d, 0, ULONG_LONG_MAX,
                                                      TRUE);
}



/* _adime_reset_ulong_long:
*/
void _adime_reset_ulong_long(DIALOG *d)
{
   usprintf((char *)d->dp, "%Lu", *(LONG_LONG *)d->dp3);
   d->d2 = ustrlen((char *)d->dp);
}
#endif
