/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   exchain.c:
   This example program shows how to use the %chain[] format to merge several
   dialogs into one.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/
#include <allegro.h>
#include <time.h>

#ifdef FORTIFY
#include "fortify2.h"
#endif

#include "adime.h"



/* frog_dialog:
   Displays the frog dialog.
*/
void frog_dialog(char *format, ...)
{
   adime_va_list ap;
   unsigned int num_frogs = 10;
   double frog_size = 7.2;
   char first_name[256] = "David Hilbert";

   adime_va_start(ap, format);
   adime_dialogf("A Frog Dialog",
                 ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 200,
                 "Number of frogs:%uint[1,]"
                 "Frog size:%double[,]"
                 "Name of first frog:%string[256]"
                 "%line[]"
                 "%chain[]",
                 &num_frogs,
                 &frog_size,
                 first_name,
                 format, ap);
   adime_va_end(ap);
}



/* run:
   Opens the two dialogs, with different kinds of objects in the lower part.
*/
void run(void)
{
   int frog_id = 256;
   int color = 0;

   frog_dialog("This is the first dialog of two. Note that the top part\n"
               "looks the same as the top part of the next dialog. The\n"
               "rest of the dialog could be reused using the %%chain%[%]\n"
               "format.%nothing[]");
   frog_dialog("Frog color:%list[,Green;Black;Brown;Orange]"
               "Frog id:%uint[,]"
               "As you can see, the %%chain%[%] format can be used to\n"
               "glue together two dialogs into one.%nothing[]",
               &color,
               &frog_id);
}



/* main:
   Installs Allegro and Adime, then calls functions to open the dialogs.
*/
int main(void)
{
   /* Install various parts of Allegro. */
   if (allegro_init())
      exit(1);
   if (install_timer()) {
      allegro_message("Couldn't install timer.\n");
      exit(1);
   }
   if (install_keyboard()) {
      allegro_message("Couldn't install keyboard.\n");
      exit(1);
   }
   if (install_mouse() == -1) {
      allegro_message("Couldn't install mouse.\n");
      exit(1);
   }
   if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0)) {
      allegro_message("Couldn't set graphics mode.\n");
      exit(1);
   }
   set_palette(default_palette);

   /* Install Adime. */
   if (adime_init() != 0) {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Error initializing Adime.\n");
      exit(1);
   }

   /* Run! */
   run();

   return 0;
}
END_OF_MAIN()
