/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   parsemod.c:
   Functions for parsing modifiers to format strings.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/
#include <stdlib.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* _adime_get_string_arg:
   Helper to extract a string from an argument. `out_p' will be set to point
   to a newly allocated buffer with the string in.

   NOTE: this function must work if arg==out_p or arg is a substring of out_p
   (e.g., arg==out_p+8).
*/
int _adime_get_string_arg(ADIME_CONST char **arg, char **out_p)
{
   char *o;
   ADIME_CONST char *tmp;
   int c = 0;
   int read = 0;
   int len = 0;
   int pass;

   /* Two passes: first we count the length, then allocate, then write. */
   tmp = *arg;
   for (pass=0; pass<2; pass++) {
      *arg = tmp;

      if (pass == 0)
         o = NULL;
      else {
         if (out_p == NULL)
            break;
         _ADIME_MALLOC(*out_p, len, char);
         o = *out_p;
      }

      for (c = ugetxc(arg); c && (c != ','); c = ugetxc(arg)) {
         if (c == '%') {
            c = ugetxc(arg);
            _ADIME_ASSERT(c, ("Format string ended unexpectedly inside an "
                              "escape sequence ('%%') in a "
                              "semicolon-separated modifier."));
         }
         if (o)
            o += usetc(o, c);
         else
            len += ucwidth(c);
         read = 1;
      }
      if (o)
         o += usetc(o, 0);
      else
         len += ucwidth(0);
   }

   return (read ? _ADIME_ARG_READ : 0) |
          ((c == ',') ? _ADIME_ARG_COMMA : _ADIME_ARG_END);
}



/* get_strlist_arg_worker:
*/
static int get_strlist_arg_worker(int just_count,
                                  ADIME_CONST char **modifiers,
                                  int *out_num, char ***out_list)
{
   char *buf;
   int c;
   char *p;
   int ret = 0;
   ADIME_CONST char *my_modifiers;
   int string_i = 0;
   int len;

   len = ustrsizez(*modifiers);
   _ADIME_MALLOC(buf, len, char);

   my_modifiers = *modifiers;
   p = buf;

   /* Loop through characters. */
   for (c = ugetxc(&my_modifiers); ; c = ugetxc(&my_modifiers)) {
      /* End of substring. */
      if ((c == ';') || (c == ',') || (c == 0)) {
         if (ret || (c == ';')) {
            if (just_count)
               (*out_num)++;
            else {
               usetc(p, 0);
               (*out_list)[string_i++] = ustrdup(buf);
            }
         }
         if (c == 0) {
            free(buf);
            return ret | _ADIME_ARG_END;
         }
         else if (c == ',') {
            free(buf);
            return ret | _ADIME_ARG_COMMA;
         }
         p = buf;
      }
      else {
         if (c == '%') {
            /* Escape sequence beginning with '%' */
            c = ugetxc(&my_modifiers);
            _ADIME_ASSERT(c, ("Format string ended unexpectedly inside an "
                              "escape sequence ('%%') in a "
                              "semicolon-separated modifier."));
         }
         ret = _ADIME_ARG_READ;
         _ADIME_ASSERT((p - buf) + ucwidth(c) + ucwidth(0) < len,
                       ("Internal bug in Adime! Please copy this message "
                        "and send to the author immediately so that this "
                        "can be fixed (see readme.txt for contact info)."));
         p += usetc(p, c);
      }
   }
}



/* _adime_get_strlist_arg:
   Parses a semicolon-separated list of strings.
*/
int _adime_get_strlist_arg(ADIME_CONST char **modifiers, int *out_num,
                          char ***out_list)
{
   *out_num = 0;
   get_strlist_arg_worker(TRUE, modifiers, out_num, out_list);
   _ADIME_MALLOC(*out_list, *out_num, char *);
   return get_strlist_arg_worker(FALSE, modifiers, out_num, out_list);
}



/* _adime_get_int_arg:
   Helper to extract an integer from an argument.
*/
int _adime_get_int_arg(ADIME_CONST char **arg, int *out_p)
{
   int val;
   int ret;
   char *endp;

   val = ustrtol(*arg, &endp, 0);

   if (endp == *arg) {
      ret = 0;
      *out_p = 0;
   }
   else {
      ret = _ADIME_ARG_READ;
      *out_p = val;
   }

   *arg = endp;

   if (ugetc(endp) == 0)
      return ret | _ADIME_ARG_END;
   else if (ugetc(endp) == ',') {
      *arg += ucwidth(',');
      return ret | _ADIME_ARG_COMMA;
   }
   else if (uisdigit(ugetc(endp)))
      return ret | _ADIME_ARG_DIGIT;
   else
      return ret | _ADIME_ARG_NONDIGIT;
}



/* _adime_get_double_arg:
   Helper to extract a double from an argument.
*/
int _adime_get_double_arg(ADIME_CONST char **arg, double *out_p)
{
   double val;
   int ret;
   char *endp;

   val = ustrtod(*arg, &endp);

   if (endp == *arg) {
      ret = 0;
      *out_p = 0;
   }
   else {
      ret = _ADIME_ARG_READ;
      *out_p = val;
   }

   *arg = endp;

   if (ugetc(endp) == 0)
      return ret | _ADIME_ARG_END;
   else if (ugetc(endp) == ',') {
      *arg += ucwidth(',');
      return ret | _ADIME_ARG_COMMA;
   }
   else if (uisdigit(ugetc(endp)))
      return ret | _ADIME_ARG_DIGIT;
   else
      return ret | _ADIME_ARG_NONDIGIT;
}
