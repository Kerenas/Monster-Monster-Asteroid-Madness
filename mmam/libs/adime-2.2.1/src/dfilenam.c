/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   dfilenam.c:
   adime_dialogf() functions for editing filenames.

   By Sven Sandberg

   See readme.txt for more information about Adime.


   char filename[1024];
   adime_dialogf(..., "Select a file%filename[1024,txt,Select a text file]",
                filename);
*/
#include <stdarg.h>
#include <stdlib.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



static int fsel_callback(DIALOG *d)
{
   if (adime_file_select(d->dp3, d[-1].dp, d[-1].dp2,
                         (d[-1].d1+1) * uwidth_max(U_CURRENT),
                         OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT))
      object_message(d-1, MSG_DRAW, 0);

   return D_REDRAWME;
}



/* _adime_create_filename:
   d[0]: (edit box)
      d1 = maxlen
      d2 = pos
      dp = filename (allocated)
      dp2 = extension (allocated)
      dp3 = result
   d[1]: (button)
      d1 = _ADIME_BUTTON_CALLBACK
      d2
      dp = button text (...) (allocated)
      dp2 = callback
      dp3 = dialog title (allocated)
*/
int _adime_create_filename(DIALOG *d, ADIME_CONST char *desc,
                           ADIME_CONST char *modifiers, void *args)
{
   int state;
   char buf[64];
   char *ext, *title;
   int button_w;
   int size;

   /* Parse parameters. */
   state = _adime_get_int_arg(&modifiers, &size);
   _ADIME_ASSERT(state & _ADIME_ARG_READ,
                 ("Error in \"%s%%filename[%s]\": "
                  "You must specify a max length for %%filename[].",
                  desc, modifiers));
   _ADIME_ASSERT(state & _ADIME_ARG_COMMA,
                 ("Error in \"%s%%filename[%s]\": "
                  "You must specify a (possibly empty) set of extensions "
                  "to %%filename[] (',' expected).",
                  desc, modifiers));

   state = _adime_get_string_arg(&modifiers, &ext);
   _ADIME_ASSERT(state & _ADIME_ARG_COMMA,
                 ("Error in \"%s%%filename[%s]\": "
                  "You must specify a dialog title for %%filename[] "
                  "(',' expected",
                  desc, modifiers));
   if (!(state & _ADIME_ARG_READ)) {
      free(ext);
      ext = NULL;
   }

   state = _adime_get_string_arg(&modifiers, &title);
   _ADIME_ASSERT_ARG_END("filename");
   if (!(state & _ADIME_ARG_READ)) {
      free(title);
      title = ustrdup(desc);
   }

   /* The title is not allowed to be the empty string: This is because the
      callback functions from the file selector use the title to detect if
      they're called from the file selector itself or from the alert() dialog
      that it brings up on disk error. */
   if (ugetc(title) == 0) {
      free(title);
      usetc(buf + usetc(buf, ' '), 0);
      title = ustrdup(buf);
   }

   /* Create text editor. */
   d->proc = adime_d_edit_proc;
   button_w = text_length(_ADIME_FONT, uconvert_ascii("...", buf)) + 6;
   d->w -= button_w;
   d->h += 6;
   d->fg = _ADIME_TEXT_COLOR;
   d->bg = _ADIME_EDIT_FIELD_COLOR;
   d->d1 = size / uwidth_max(U_CURRENT) - 1;
   _ADIME_MALLOC(d->dp, size, char);
   d->dp2 = ext;
   d->dp3 = args;

   /* Create button for opening file selector. */
   d++;
   d->proc = adime_d_button_proc;
   d->x = d->x + d->w - button_w;
   d->w = button_w;
   d->h += 6;
   d->d1 = ADIME_BUTTON_CALLBACK;
   d->fg = _ADIME_TEXT_COLOR;
   d->bg = _ADIME_BUTTON_COLOR;
   d->dp = ustrdup(uconvert_ascii("...", buf));
   d->dp2 = fsel_callback;
   d->dp3 = title;

   return 0;
}



/* _adime_store_filename:
*/
void _adime_store_filename(DIALOG *d)
{
   ustrzcpy(d->dp3, (d->d1+1) * uwidth_max(U_CURRENT), d->dp);
}



/* _adime_reset_filename:
*/
void _adime_reset_filename(DIALOG *d)
{
   ustrzcpy(d->dp, (d->d1+1) * uwidth_max(U_CURRENT), d->dp3);
}



/* _adime_destroy_filename:
*/
void _adime_destroy_filename(DIALOG *d)
{
   free(d->dp);
   free(d->dp2);
   free(d[1].dp);
   free(d[1].dp3);
}
