/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dlists.c:
   adime_dialogf() functions for selecting a string from a list, i.e.,
   the formats %vlist, %list, %datafile, %wvlist, %wlist, %wdatafile.

   By Sven Sandberg

   See readme.txt for more information about Adime.


   char *my_strings[] = { "foo", "bar", "baz" };
   int index;
   adime_dialogf(..., "Select a name%vlist[2]", &index, my_strings, 3);
   adime_dialogf(..., "Select a name%wvlist[3,Select a name]", &index, my_strings, 3);


   int index;
   adime_dialogf(..., "Select a name%list[3,foo;bar;baz]", &index);
   adime_dialogf(..., "Select a name%wlist[3,Select a name,foo;bar;baz]", &index);


   int index;
   DATAFILE *datafile;
   adime_dialogf(..., "Select a picture from the datafile"
                "%datafile[3,BMP ;RLE ;CMP ;XCMP]",
                &index, datafile);
   adime_dialogf(..., "Select a picture from the datafile"
                "%wdatafile[3,Select,BMP ;RLE ;CMP ;XCMP]",
                &index, datafile);
   adime_dialogf(..., "Select anything from the datafile%datafile[3,]",
                &index, datafile);
   adime_dialogf(..., "Select anything from the datafile%wdatafile[3,Select,]",
                &index, datafile);
*/
#include <stdarg.h>
#include <stdlib.h>
#include <allegro.h>
#if ALLEGRO_DATE < 20011116
#include <allegro/aintern.h>
#else
#include <allegro/internal/aintern.h>
#endif

#include "adime.h"
#include "adime/adimeint.h"



typedef struct DIALOGF_LIST
{
   int *out;            /* Where to store the result. */
   char **strings;      /* The contents of the list. */
   int n_strings;       /* The number of elements in the list. */
   DATAFILE *datafile;  /* Datafile to use for %datafile. */
   int datafile_types[MAX_DATAFILE_TYPES + 1];  /* Types for %datafile.*/
   char *title;         /* Title for %wlists */
   int n_lines;         /* Height of list. */
} DIALOGF_LIST;



#define DATAFILE_END(d) (((d)->type == DAT_END) || ((d)->type == AL_ID('i', 'n', 'f', 'o')))



static DIALOGF_LIST *current_list = NULL;



/* wlist_callback:
*/
static int wlist_callback(DIALOG *d)
{
   char format[256];
   int w, max_w;
   int i;
   DIALOGF_LIST *dl = (DIALOGF_LIST *)d->dp3;

   max_w = adime_window_border_thickness * 2 + adime_window_button_w * 2 +
           adime_window_between_button_spacing;
   w = text_length(_ADIME_FONT, dl->title);
   if (w > max_w)
      max_w = w;
   for (i = 0; i < dl->n_strings; i++) {
      w = text_length(_ADIME_FONT, dl->strings[i]);
      if (w > max_w)
         max_w = w;
   }
   if (max_w > (_ADIME_BMP_W * 3) / 4)
      max_w = (_ADIME_BMP_W * 3) / 4;

   usprintf(format, "%%vlist[%d]", d->d2);

   if (adime_dialogf(dl->title,
                     ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, max_w,
                     format, &d->d2, dl->strings, dl->n_strings) == 1)
      d->dp = dl->strings[d->d2];

   return D_O_K;
}



/* d_my_list_proc:
   A Horrible Hack needed because there is no room for custom data in
   `d_text_list_proc()'.
*/
static int d_my_list_proc(int msg, DIALOG *d, int c)
{
   int ret;
   DIALOGF_LIST *prev_list;

   if (d->dp2 != NULL) {
      prev_list = current_list;
      current_list = d->dp2;
      d->dp2 = NULL;
      ret = adime_d_text_list_proc(msg, d, c);
      d->dp2 = current_list;
      current_list = prev_list;
   }
   else
      ret = adime_d_text_list_proc(msg, d, c);

   return ret;
}



/* my_list_getter:
*/
static char *my_list_getter(int index, int *list_size)
{
   if (index == -1) {
      *list_size = current_list->n_strings;
      return NULL;
   }
   else
      return current_list->strings[index];
}



/* read_datafile_types:
   Reads the semicolon-separated list of datafile types from modifiers.
*/
static int read_datafile_types(DIALOGF_LIST *dl,
                               ADIME_CONST char **modifiers)
{
   int n;
   char buf[4];
   int c;
   int i;

   c = ugetc(*modifiers);

   if ((c == ',') || (c == 0)) {
      dl->datafile_types[0] = 0;
      ugetxc(modifiers);
      return (c == ',') ? _ADIME_ARG_COMMA : _ADIME_ARG_END;
   }
   else {
      n = 0;
      do {
         _ADIME_ASSERT(n < MAX_DATAFILE_TYPES,
                       ("Too many datafile types in %%datafile[]."));
         for (i = 0; i < 4; i++) {
            c = ugetxc(modifiers);
            _ADIME_ASSERT((c >= ' ') && (c < 256),
                         ("Invalid character in datafile type list: '%c' "
                          "(ascii %d)", c, c));
            buf[i] = c;
         }
         dl->datafile_types[n] = AL_ID(buf[0], buf[1], buf[2], buf[3]);
         n++;
         c = ugetxc(modifiers);
      } while (c == ';');
      _ADIME_ASSERT((c == ',') || (c == 0),
                    ("Datafile types must be exactly 4 characters long and "
                     "separated by semicolons."));
      dl->datafile_types[n] = 0;
      return _ADIME_ARG_READ |
             ((c == ',') ? _ADIME_ARG_COMMA : _ADIME_ARG_END);
   }
}



/* in_datafile_type_list:
   Returns nonzero if d is in the list of types.
*/
static int in_datafile_type_list(DATAFILE *d, ADIME_CONST int *types)
{
   int i;

   if (!types[0])
      return TRUE;

   for (i = 0; types[i]; i++)
      if (d->type == types[i])
         return TRUE;

   return FALSE;
}



/* get_datafile_names:
   Find the datafile object names which match the given object types.
*/
static void get_datafile_names(DIALOGF_LIST *dl)
{
   int i, num_added;

   /* Count matching datafile items. */
   dl->n_strings = 0;
   for (i = 0; !DATAFILE_END(dl->datafile + i); i++)
      if (in_datafile_type_list(dl->datafile + i, dl->datafile_types))
         dl->n_strings++;

   /* Copy names of matching datafile items. */
   _ADIME_MALLOC(dl->strings, dl->n_strings, char *);
   num_added = 0;
   for (i = 0; !DATAFILE_END(dl->datafile + i); i++)
      if (in_datafile_type_list(dl->datafile + i, dl->datafile_types))
         dl->strings[num_added++] =
          ustrdup(get_datafile_property(dl->datafile + i, DAT_NAME));
}



/* _adime_count_vlist:
   The `count' function for %vlist[] and %wvlist[].
*/
int _adime_count_vlist(ADIME_CONST char *desc, ADIME_CONST char *modifiers,
                       adime_va_list args, void **saved_args)
{
   DIALOGF_LIST *dl;

   _ADIME_UNUSED_ARG(desc);
   _ADIME_UNUSED_ARG(modifiers);

   _ADIME_MALLOC(dl, 1, DIALOGF_LIST);
   dl->out = adime_va_arg(args, int *);
   dl->strings = adime_va_arg(args, char **);
   dl->n_strings = adime_va_arg(args, int);
   *saved_args = dl;

   return 1;
}



/* There is no _adime_count_list() because it uses
   _adime_count_pointer_and_return_1() */



/* _adime_count_datafile:
   The `count' function for %datafile[] and %wdatafile[].
*/
int _adime_count_datafile(ADIME_CONST char *desc,
                          ADIME_CONST char *modifiers,
                          adime_va_list args, void **saved_args)
{
   DIALOGF_LIST *dl;

   _ADIME_UNUSED_ARG(desc);
   _ADIME_UNUSED_ARG(modifiers);

   _ADIME_MALLOC(dl, 1, DIALOGF_LIST);
   dl->out = adime_va_arg(args, int *);
   dl->datafile = adime_va_arg(args, DATAFILE *);

   *saved_args = dl;

   return 1;
}



/* do_create_list:
   Helper to create a list object. All formats that use some kind of lists
   actually create a `char **' array which they then pass over to this
   function.

   d1 = selected item
   d2 = scrolled
   dp = callback
   dp2 = DIALOGF_LIST
   dp3 = internal to d_text_list_proc
*/
static void do_create_list(DIALOG *d, DIALOGF_LIST *dl)
{
   int font_h = text_height(_ADIME_FONT);

   if (dl->n_lines == 0)
      dl->n_lines = MID(2, dl->n_strings,
                        (3 * _ADIME_BMP_H) / (4 * font_h));

   d->proc = d_my_list_proc;
   d->h = 4 + dl->n_lines * font_h;
   d->fg = _ADIME_TEXT_COLOR;
   d->bg = _ADIME_EDIT_FIELD_COLOR;
   d->d1 = MID(0, *dl->out, dl->n_strings - 1);
   d->d2 = MIN(d->d1, dl->n_strings - dl->n_lines);
   if (d->d2 < 0)
      d->d2 = 0;

   d->dp = my_list_getter;
   d->dp2 = dl;
}



/* do_create_wlist:
   Helper to create a wvlist object. All formats that use some kind of
   lists actually create a `char **' which they then pass over to this
   function.

   d1 = button type
   d2 = selected item
   dp = button text (one of the strings)
   dp2 = callback
   dp3 = DIALOGF_LIST
*/
static void do_create_wlist(DIALOG *d, DIALOGF_LIST *dl)
{
   int font_h;

   if (dl->n_lines == 0) {
      font_h = text_height(_ADIME_FONT);
      dl->n_lines = MID(2, dl->n_strings,
                        (3 * _ADIME_BMP_H) / (4 * font_h));
   }

   d->proc = adime_d_button_proc;
   d->h += 6;
   d->fg = _ADIME_TEXT_COLOR;
   d->bg = _ADIME_BUTTON_COLOR;
   d->d1 = ADIME_BUTTON_CALLBACK;
   d->dp2 = wlist_callback;
   d->dp3 = dl;
}



/* _adime_create_vlist:
   %vlist[lines]
   int *out, char **string, int n_strings
*/
int _adime_create_vlist(DIALOG *d, ADIME_CONST char *desc,
                        ADIME_CONST char *modifiers, void *args)
{
   int state;
   DIALOGF_LIST *dl;

   _ADIME_UNUSED_ARG(desc);

   dl = (DIALOGF_LIST *)args;

   state = _adime_get_int_arg(&modifiers, &dl->n_lines);
   if (!(state & _ADIME_ARG_READ))
      dl->n_lines = 0;
   _ADIME_ASSERT_ARG_END("vlist");

   do_create_list(d, dl);

   return 0;
}



/* _adime_create_list:
   %list[lines,foo;bar;baz]
   int *out
*/
int _adime_create_list(DIALOG *d, ADIME_CONST char *desc,
                       ADIME_CONST char *modifiers, void *args)
{
   int state;
   DIALOGF_LIST *dl;
   int n_lines;

   _ADIME_UNUSED_ARG(desc);

   _ADIME_MALLOC(dl, 1, DIALOGF_LIST);
   dl->out = args;

   /* # of lines */
   state = _adime_get_int_arg(&modifiers, &dl->n_lines);
   _ADIME_ASSERT(state & _ADIME_ARG_COMMA,
                 ("Error in \"%s%%list[%s]\": "
                  "You must supply a semicolon-separated list of strings "
                  "as a modifier to %%list[].",
                  desc, modifiers));
   if (!(state & _ADIME_ARG_READ))
      n_lines = 0;
   /* list */
   state = _adime_get_strlist_arg(&modifiers, &dl->n_strings, &dl->strings);
   _ADIME_ASSERT_ARG_END("list");

   do_create_list(d, dl);

   return 0;
}



/* _adime_create_datafile:
   %datafile[lines,formats]
   int *out, DATAFILE *datafile
*/
int _adime_create_datafile(DIALOG *d, ADIME_CONST char *desc,
                           ADIME_CONST char *modifiers, void *args)
{
   int state;
   DIALOGF_LIST *dl;

   _ADIME_UNUSED_ARG(desc);

   dl = args;

   /* # of lines */
   state = _adime_get_int_arg(&modifiers, &dl->n_lines);
   if (!(state & _ADIME_ARG_READ))
      dl->n_lines = 0;
   _ADIME_ASSERT(state & _ADIME_ARG_COMMA,
                 ("Error in \"%s%%datafile[%s]\": "
                  "You must supply a (possibly empty) list of datafile "
                  "formats to %%datafile[].",
                  desc, modifiers));
   /* datafile types */
   state = read_datafile_types(dl, &modifiers);
   _ADIME_ASSERT_ARG_END("datafile");
   get_datafile_names(dl);

   do_create_list(d, dl);

   return 0;
}



/* _adime_create_wvlist:
   %wvlist[lines,title]
   int *out, char **list, int num_items
*/
int _adime_create_wvlist(DIALOG *d, ADIME_CONST char *desc,
                         ADIME_CONST char *modifiers, void *args)
{
   int state;
   DIALOGF_LIST *dl;

   _ADIME_UNUSED_ARG(desc);

   dl = args;

   /* # of lines */
   state = _adime_get_int_arg(&modifiers, &dl->n_lines);
   if (!(state & _ADIME_ARG_READ))
      dl->n_lines = 0;
   _ADIME_ASSERT(state & _ADIME_ARG_COMMA,
                 ("Error in \"%s%%wvlist[%s]\": "
                  "You must supply a dialog title as a modifier to "
                  "%%wvlist[].",
                  desc, modifiers));
   /* title */
   state = _adime_get_string_arg(&modifiers, &dl->title);
   _ADIME_ASSERT_ARG_END("wvlist");

   do_create_wlist(d, dl);

   return 0;
}



/* _adime_create_wlist:
   %wlist[lines,title,foo;bar;baz]
   int *out
*/
int _adime_create_wlist(DIALOG *d, ADIME_CONST char *desc,
                        ADIME_CONST char *modifiers, void *args)
{
   int state;
   DIALOGF_LIST *dl;

   _ADIME_UNUSED_ARG(desc);

   _ADIME_MALLOC(dl, 1, DIALOGF_LIST);
   dl->out = args;

   /* # of lines */
   state = _adime_get_int_arg(&modifiers, &dl->n_lines);
   if (!(state & _ADIME_ARG_READ))
      dl->n_lines = 0;
   _ADIME_ASSERT(state & _ADIME_ARG_COMMA,
                 ("Error in \"%s%%wlist[%s]\": "
                  "You must supply a dialog title and a list as modifiers "
                  "to %%wlist[].",
                  desc, modifiers));
   /* title */
   state = _adime_get_string_arg(&modifiers, &dl->title);
   _ADIME_ASSERT(state & _ADIME_ARG_COMMA,
                 ("Error in \"%s%%wlist[%s]\": "
                  "You must supply a list as a modifier to %%wlist[]",
                  desc, modifiers));
   /* list */
   state = _adime_get_strlist_arg(&modifiers, &dl->n_strings, &dl->strings);
   _ADIME_ASSERT_ARG_END("wlist");

   do_create_wlist(d, dl);

   return 0;
}



/* _adime_create_wdatafile:
   %wdatafile[lines,title,formats]
*/
int _adime_create_wdatafile(DIALOG *d, ADIME_CONST char *desc,
                            ADIME_CONST char *modifiers, void *args)
{
   int state;
   DIALOGF_LIST *dl;

   _ADIME_UNUSED_ARG(desc);

   dl = args;

   /* # of lines */
   state = _adime_get_int_arg(&modifiers, &dl->n_lines);
   if (!(state & _ADIME_ARG_READ))
      dl->n_lines = 0;
   _ADIME_ASSERT(state & _ADIME_ARG_COMMA,
                 ("Error in \"%s%%wdatafile[%s]\": "
                  "You must supply a dialog title and a (possibly empty) "
                  "list of datafile types as modifiers to %%wdatafile[].",
                  desc, modifiers));
   /* title */
   state = _adime_get_string_arg(&modifiers, &dl->title);
   _ADIME_ASSERT(state & _ADIME_ARG_COMMA,
                 ("Error in \"%s%%wdatafile[%s]\": "
                  "You must supply a a (possibly empty) list of datafile "
                  "types as a modifier to %%wdatafile[].",
                  desc, modifiers));
   /* formats */
   state = read_datafile_types(dl, &modifiers);
   _ADIME_ASSERT_ARG_END("wdatafile");

   get_datafile_names(dl);
   do_create_wlist(d, dl);

   return 0;
}



/* _adime_store_lists:
*/
void _adime_store_lists(DIALOG *d)
{
   *((DIALOGF_LIST *)d->dp2)->out = d->d1;
}



/* _adime_store_wlists:
*/
void _adime_store_wlists(DIALOG *d)
{
   *((DIALOGF_LIST *)d->dp3)->out = d->d2;
}



/* store_datafile_worker:
*/
static void store_datafile_worker(int sel, DIALOGF_LIST *dl)
{
   int pos;
   int i;

   pos = 0;
   for (i = 0; !DATAFILE_END(dl->datafile + i); i++) {
      if (in_datafile_type_list(dl->datafile + i, dl->datafile_types)) {
         if (pos == sel) {
            *dl->out = i;
            return;
         }
         pos++;
      }
   }
   _ADIME_ASSERT(0, ("NOTREACHED"));
}



/* _adime_store_datafile:
*/
void _adime_store_datafile(DIALOG *d)
{
   store_datafile_worker(d->d1, (DIALOGF_LIST *)d->dp2);
}



/* _adime_store_wdatafile:
*/
void _adime_store_wdatafile(DIALOG *d)
{
   store_datafile_worker(d->d2, (DIALOGF_LIST *)d->dp3);
}



/* _adime_reset_lists:
*/
void _adime_reset_lists(DIALOG *d)
{
   DIALOGF_LIST *dl = (DIALOGF_LIST *)d->dp2;

   d->d1 = *dl->out;
   d->d1 = MID(0, d->d1, dl->n_strings - 1);
}



/* _adime_reset_wlists:
*/
void _adime_reset_wlists(DIALOG *d)
{
   DIALOGF_LIST *dl = (DIALOGF_LIST *)d->dp3;

   d->d2 = *dl->out;
   d->d2 = MID(0, d->d2, dl->n_strings - 1);
   d->dp = ((DIALOGF_LIST *)d->dp3)->strings[d->d2];
}



/* get_datafile_pos:
   Converts position in list to index in DATAFILE object (some objects may
   be filtered out in the list).
*/
static int get_datafile_pos(DIALOGF_LIST *dl)
{
   int pos, end;
   int i;

   end = MID(0, *dl->out, dl->n_strings - 1);
   pos = 0;
   for (i = 0; i < end; i++)
      if (in_datafile_type_list(dl->datafile + i, dl->datafile_types))
         pos++;
   return pos;
}



/* _adime_reset_datafile:
*/
void _adime_reset_datafile(DIALOG *d)
{
   d->d1 = get_datafile_pos((DIALOGF_LIST *)d->dp2);
}



/* _adime_reset_wdatafile:
*/
void _adime_reset_wdatafile(DIALOG *d)
{
   d->d2 = get_datafile_pos((DIALOGF_LIST *)d->dp3);
}



/* _adime_destroy_vlist:
*/
void _adime_destroy_vlist(DIALOG *d)
{
   free(d->dp2);
}



/* _adime_destroy_list_and_datafile:
*/
void _adime_destroy_list_and_datafile(DIALOG *d)
{
   int i;
   DIALOGF_LIST *dl = (DIALOGF_LIST *)d->dp2;

   for (i = 0; i < dl->n_strings; i++)
      free(dl->strings[i]);
   free(dl->strings);
   free(d->dp2);
}



/* _adime_destroy_wvlist:
*/
void _adime_destroy_wvlist(DIALOG *d)
{
   free(((DIALOGF_LIST *)d->dp3)->title);
   free(d->dp3);
}



/* _adime_destroy_wlist_and_wdatafile:
*/
void _adime_destroy_wlist_and_wdatafile(DIALOG *d)
{
   int i;
   DIALOGF_LIST *dl = (DIALOGF_LIST *)d->dp3;

   for (i = 0; i < dl->n_strings; i++)
      free(dl->strings[i]);
   free(dl->strings);
   _adime_destroy_wvlist(d);
}
