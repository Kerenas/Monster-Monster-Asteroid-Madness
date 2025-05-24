/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   exanim.c:
   This example program shows how to animate while a dialog is open, using
   the `dialogf_callback()' function pointer.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/
#include <time.h>
#include <stdlib.h>
#include <allegro.h>

#ifdef FORTIFY
#include "fortify2.h"
#endif

#include "adime.h"



volatile int time_count = 0;



/* time_counter:
   Timer handler called every 20 millisecond.
*/
void time_counter(void)
{
   time_count++;
}
END_OF_FUNCTION(time_counter)



/* ball_inside:
   Returns TRUE if the ball which has the given coordinates touches the
   dialog.
*/
int ball_inside(int x, int y, DIALOG *d)
{
   if ((x > d->x - 10) && (x < d->x + d->w) &&
       (y > d->y - 10) && (y < d->y + d->h))
      return TRUE;
   else
      return FALSE;
}



/* animate:
   This function will be called continuously while the dialog is open.
*/
void animate(DIALOG *d)
{
   #define GRAVITY   (1)
   static float x = 0, y = 0;
   static float dx = 0, dy = 0;
   static int my_count = 0;
   static int virgin = TRUE;

   /* Set up initial values. */
   if (virgin) {
      virgin = FALSE;
      x = SCREEN_W / 2 - 5;
      y = (d->y - 10) / 2;
      srand(time(NULL));
      dx = rand() / (RAND_MAX / 1.4) - 0.7;
      dy = rand() / (RAND_MAX / 1.4) - 0.7;
   }

   /* Has enough time elapsed since last time we updated? */
   if (my_count < time_count) {
      vsync();

      /* Undraw. */
      scare_mouse();
      circlefill(screen, x+5, y+5, 4, 0);

      /* Update position. */
      while (my_count < time_count) {
         my_count++;

         /*
          * Update x coordinate.
          */
         x += dx;
         /* Bounce against left and right edge. */
         if (x < 0) {
            x = 0;
            dx = -dx;
            dy += (rand() / (RAND_MAX / 4.0)) - 2;
         }
         else if (x > SCREEN_W - 10) {
            x = SCREEN_W - 10;
            dx = -dx;
            dy += (rand() / (RAND_MAX / 4.0)) - 2;
         }
         /* Bounce against dialog. */
         else if (ball_inside(x, y, d)) {
            if (dx < 0)
               x = d->x + d->w;
            else
               x = d->x - 10;
            dx = -dx;
            dy += (rand() / (RAND_MAX / 4.0)) - 2;
         }

         /* Make sure speed won't be too high. */
         if (dx > 20)
            dx = 20;
         if (dx < -20)
            dx = -20;

         /*
          * Update y coordinate.
          */
         dy += GRAVITY;
         y += dy;
         /* Bounce against top and bottom edge. */
         if (y < 0) {
            y = 0;
            dy = -dy;
            dx += (rand() / (RAND_MAX / 8.0)) - 4;
         }
         else if (y > SCREEN_H - 10) {
            y = SCREEN_H - 10;
            /* Bounce lower when the ball is under the dialog. */
            if ((x < d->x - 10) || (x > d->x + d->w))
               dy = -(rand() / (RAND_MAX / 25.0) + 15);
            else
               dy = -(rand() / (RAND_MAX / 7.0) + 10);
            dx += (rand() / (RAND_MAX / 8.0)) - 4;
         }
         /* Bounce against dialog. */
         else if (ball_inside(x, y, d)) {
            if (dy < 0) {
               y = d->y + d->h;
               dy = -dy;
            }
            else {
               y = d->y - 10;
               dy = -(rand() / (RAND_MAX / 7.0) + 10);
            }
            dx += (rand() / (RAND_MAX / 8.0)) - 4;
         }
      }

      /* Draw. */
      circlefill(screen, x+5, y+5, 4, 255);
      unscare_mouse();
   }
}



/* run:
   Opens the dialog.
*/
void run(void)
{
   int int1 = 4, int2 = 7, int3 = 1;
   double double1 = 1;

   /* Arrange for `animate()' to be called continuously while the dialog is
      open. */
   adime_callback = animate;

   /* This might make animation more smooth under windows. */
   adime_yield = FALSE;

   /* Open the dialog. */
   adime_dialogf("Enter Some Numbers",
                 ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 100,
                 "(Note how nicely we can animate in the\n"
                 "background while showing the dialog)%nothing[]"
                 "An integer:%int[,]"
                 "Another integer:%int[,]"
                 "Yet another integer:%int[,]"
                 "A double:%double[,]",
                 &int1,
                 &int2,
                 &int3,
                 &double1);
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

   /* Set up the timer. */
   LOCK_VARIABLE(time_count);
   LOCK_FUNCTION(time_counter);
   install_int(time_counter, 20);

   /* Run! */
   run();

   return 0;
}
END_OF_MAIN()
