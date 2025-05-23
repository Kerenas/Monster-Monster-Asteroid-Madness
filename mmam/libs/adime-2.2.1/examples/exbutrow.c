/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   exdialog.c:
   Example program for Adime, showing how to create custom buttons at the
   bottom of the dialog.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/
#include <allegro.h>

#include "adime.h"



COLOR_MAP snow_table;



/* setup_tables:
   Initializes palette and color map that the snow needs.
*/
void setup_tables(void)
{
   PALETTE pal;
   int i;

   /* Create a palette with suitable gradients. */
   #define SET_RGB(index, name) \
      pal[index].r = adime_##name##_rgb.r / 4; \
      pal[index].g = adime_##name##_rgb.g / 4; \
      pal[index].b = adime_##name##_rgb.b / 4;
   pal[0].r = pal[0].g = pal[0].b = 0;
   SET_RGB(1, text);
   SET_RGB(2, disabled_text);
   SET_RGB(3, error_text);
   SET_RGB(4, border);
   SET_RGB(5, background);
   SET_RGB(6, edit_field);
   SET_RGB(7, button);
   SET_RGB(8, dark_shadow);
   SET_RGB(9, shadow);
   SET_RGB(10, highlight);
   SET_RGB(11, light_highlight);
   SET_RGB(12, title_text);
   SET_RGB(13, title_background);
   SET_RGB(14, title_shadow);
   SET_RGB(15, title_highlight);
   for (i=16; i<64; i++)
      pal[i].r = pal[i].g = pal[i].b = 0;
   for (i=0; i<64; i++)
      pal[64+i].r = pal[64+i].g = pal[64+i].b = i;
   for (i=0; i<64; i++) {
      pal[128+i].r = 63;
      pal[128+i].g = pal[128+i].b = i;
   }
   for (i=0; i<64; i++) {
      pal[192+i].r = pal[192+i].g = i;
      pal[192+i].b = 32 + (i/2);
   }

   set_palette(pal);

   /* Create a light table for the snow. */
   for (i=0; i<255; i++) {
      snow_table.data[0][i] = i;
      snow_table.data[1][i] = bestfit_color(
         pal,
         (pal[i].r * 15 + 78) >> 4,
         (pal[i].g * 15 + 78) >> 4,
         (pal[i].b * 15 + 78) >> 4);
   }
   color_map = &snow_table;
}



/* snow:
   Draws snowflakes on screen.
*/
void snow(void)
{
   #define SIXTEEN(x)   x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x
   static int x[4096] = { SIXTEEN(SIXTEEN(SIXTEEN(0))) };
   static int y[4096] = { SIXTEEN(SIXTEEN(SIXTEEN(0))) };
   static int r[4096] = { SIXTEEN(SIXTEEN(SIXTEEN(0))) };
   int i=0, j;
   int mouse_mask = ~mouse_b;
   int mouse_clicked = FALSE;

   scare_mouse();
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   while (!keypressed() && !(mouse_clicked && (mouse_b == 0))) {
      if ((mouse_b & mouse_mask) != 0)
         mouse_clicked = TRUE;
      mouse_mask |= ~mouse_b;

      for (j=0; j<10; j++) {
         x[i] = (rand() + (rand() << 8)) % SCREEN_W;
         y[i] = (rand() + (rand() << 8)) % SCREEN_H;
         r[i] = 50 + (rand() % 100);
         i++;
         if (i == 4096)
            i = 0;
      }

      for (j=0; j<4096; j++) {
         if (r[j] > 0) {
            if ((r[j] % 7) == 0)
               circlefill(screen, x[j], y[j], r[j]/7, 1);
            r[j]--;
         }
      }
      rest(10);
   }
   solid_mode();
   clear_keybuf();
   clear(screen);
   unscare_mouse();
}



/* callback:
   This will be called whenever the user clicks one of the buttons.
*/
int callback(DIALOG *d, int button_index)
{
   switch (button_index) {
      /* OK button */
      case 1:
         /* Return 1 to indicate that button number 1 was clicked, and add a
            negative sign to tell adime_dialogf() to save the results. */
         return -1;

      /* Cancel button */
      case 2:
         /* Return 2 to indicate that button number 2 was clicked, and don't
            add a negative sign to tell adime_dialogf() to ignore the
            results. */
         return 2;

      /* About button */
      case 3:
         /* Open the about dialog. */
         adime_dialogf("About Today's Weather",
                       ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 200,
                       "Today's Weather\n"
                       "\n"
                       "This implementation uses undocumented\n"
                       "hooks in your operating system to alter\n"
                       "the weather.%nothing[]");
         /* Return 0 to indicate that the dialog should continue as if
            nothing had happen. */
         return 0;

      /* Snow button */
      case 4:
         /* Display some snowflakes. */
         snow();
         /* Return ADIME_REDRAW to indicate that the screen contents has been
            altered and the dialog must be redrawn. */
         return ADIME_REDRAW;

      default:
         /* NOTREACHED */
         ASSERT(0);
         return 0;
   }
}



/* run:
   Opens the dialog.
*/
void run(void)
{
   int temperature = 12;
   int cloud_type = 1;
   double wind_strength = 10;
   int phase_of_moon = 0;

   clear(screen);
   adime_lowlevel_dialogf(
      "Select Today's Weather",
      ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 200,
      "Temperature:%int[-150,90]"
      "Cloud Type:%list[,None;Cloudy;Foggy]"
      "Wind Strength (m/s):%double[0,100]"
      "Phase of the Moon:%list[,New;Half;Full]"
      "%buttonrow[&OK;ALT+O;CTRL+O;ENTER,"
                 "&Cancel;ALT+C;ESC,"
                 "About;F1,"
                 "Snow;ALT+S]",
      &temperature,
      &cloud_type,
      &wind_strength,
      &phase_of_moon,
      callback);
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

   /* Install Adime. */
   if (adime_init() != 0) {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Error initializing Adime.\n");
      exit(1);
   }

   /* Setup palette and color table for the snow effect. */
   setup_tables();

   /* Open the dialog! */
   run();

   return 0;
}
END_OF_MAIN()
