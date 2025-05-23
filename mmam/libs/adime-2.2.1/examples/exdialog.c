/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   exdialog.c:
   The first introduction example program for Adime.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/
#include <allegro.h>

#ifdef FORTIFY
#include "fortify2.h"
#endif

#include "adime.h"



/* dialog1:
   Opens the first simple dialog.
*/
void dialog1(void)
{
   char name[256] = "";

   clear(screen);
   textout(screen, font,
           "With Adime, it is easy to create a simple dialog like this one.",
           0, 0, 255);
   /* The call to dialogf().
       - First parameter is a dialog caption.
       - Second and third parameters are coordinates for top left corner of
         the dialog. Fourth parameter is the width of the input fields.
       - Fifth parameter is the format string. This works much like the
         format strings passed to scanf() or printf(), but with other format
         specifiers (see the help for more detailed info).
       - The rest of the parameters are the variables in which the default
         values are read and the selection is be stored.
   */
   adime_dialogf("Enter Your Name",
                 ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 200,
                 "Name:%string[256]",
                 name);
}



/* dialog2:
   Opens the second, more complex dialog.
*/
void dialog2(void)
{
   char name[256] = "";
   double age = 20;
   double length = 170;
   int married = 0;
   char text_file[1024] = "";

   clear(screen);
   textout(screen, font,
           "It is no harder to create more complex dialogs.",
           0, 0, 255);
   adime_dialogf("Personal Settings",
                 ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 200,
                 "Name:%string[256]"
                 "Age:%double[0,150]"
                 "Length (centimetres):%double[0,300]"
                 "Married:%bool[]"
                 "Favourite Text File:%filename[1024,txt,Select a Text File]",
                 name,
                 &age,
                 &length,
                 &married,
                 text_file);
}



/* dialog3:
   Opens the third dialog, demonstrating some more features.
*/
void dialog3(void)
{
   int int_1 = 4711;
   int int_2 = 42;
   int list_index_1 = 0;
   int list_index_2 = 3;
   char *language_names[] =
   {
      "C", "C++", "Java", "Tri-intercal", "Lisp", "Prolog", "Reverse Polish"
   };

   clear(screen);
   textout(screen, font,
           "This dialog shows some more input types.",
           0, 0, 255);

   adime_dialogf("Yet Another Dialog",
                 ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 200,
                 "A separator:%line[]"
                 "An integer:%int[,]"
                 "An integer between 0 and 100:%int[0,100]"
                 "A separator:%line[]"
                 "A string from a list:%list[,Dutch;Lojban;Mandarin;Swahili]"
                 "A string from a list (in another way):%vlist[4]"
                 ,
                 &int_1,
                 &int_2,
                 &list_index_1,
                 &list_index_2,
                 language_names,
                 7);
}



/* dialog4:
   Big Boss Dialog.
   This may look complicated, but remember that it's also a very complex
   dialog! With most other libraries, the code would be much more
   complicated.
*/
void dialog4(void)
{
   char player_name[256] = "";
   char player_picture[1024] = "";
   int game_speed = 5;
   int difficulty_level = 4;

   int graphics_mode_index = 2;
   int page_flipping = 1;
   int bilinear_filtering = 1;

   int sound_volume = 80;
   int music_volume = 80;
   int stereo = 0;

   int save_default = 0;

   clear(screen);
   textout(screen, font,
           "There is even support for nested dialogs.",
           0, 0, 255);
   textout(screen, font,
           "See the other examples for some more advanced features.",
           0, 10, 255);

   adime_dialogf("Simulation Of Options Dialog For A Game",
                 ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 200,
                 "Game Options:%dialogf[Configure...]"
                 "Graphics:%dialogf[Configure...]"
                 "Sound:%dialogf[Configure...]"
                 "Remember as default:%bool[]",
                 /* The game options dialog. */
                    "Game Options",
                    ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 200,
                    "Player Name:%string[256]"
                    "Player Picture:%filename[1024,pcx;tga;lbm,Select A Picture]"
                    "Speed (0 - 10):%int[0,10]"
                    "Difficulty level:%list[,I'm Too Young To Die;"
                                            /* Note the escaped comma! */
                                            "Hey%, Not Too Rough;"
                                            "Hurt Me Plenty;"
                                            "Ultra-Violence;"
                                            "Nightmare]",
                    player_name,
                    player_picture,
                    &game_speed,
                    &difficulty_level,
                 /* The Graphics dialog. */
                    "Graphics Options",
                    ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 200,
                    "Graphics mode:%list[,320x200;640x480;800x600;1024x768;1600x1200]"
                    "Page flipping:%bool[]"
                    "Bilinear filtering:%bool[]",
                    &graphics_mode_index,
                    &page_flipping,
                    &bilinear_filtering,
                 /* The sound dialog. */
                    "Sound Options",
                    ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 200,
                    "SFX Volume (0 - 100):%int[0,100]"
                    "Music Volume (0 - 100):%int[0,100]"
                    "Stereo:%bool[]",
                    &sound_volume,
                    &music_volume,
                    &stereo,
                 /* The "remember as default" check box. */
                 &save_default);
}



/* main:
   Installs Allegro and Adime, then calls functions to open the dialogs.
*/
int main(void)
{
   /* Install various parts of Allegro. */
   if (allegro_init())
      exit(1);
   if (install_keyboard()) {
      allegro_message("Could not install keyboard.\n");
      exit(1);
   }
   if (install_mouse() == -1) {
      allegro_message("Could not install mouse.\n");
      exit(1);
   }
   if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0)) {
      allegro_message("Could not set graphics mode.\n");
      exit(1);
   }
   set_palette(default_palette);

   /* Install Adime. */
   if (adime_init() != 0) {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Error initializing Adime.\n");
      exit(1);
   }

   /* Open the dialogs. */
   dialog1();
   dialog2();
   dialog3();
   dialog4();

   return 0;
}
END_OF_MAIN()
