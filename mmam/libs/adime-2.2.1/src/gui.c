/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   gui.c:
   Misc gui helper functions.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/
#include <string.h>
#include <allegro.h>
#if ALLEGRO_DATE < 20011116
#include <allegro/aintern.h>
#else
#include <allegro/internal/aintern.h>
#endif

#include "adime.h"
#include "adime/adimeint.h"


typedef char *(*GETFUNCPTR)(int, int *);



static int _old_clip_x1, _old_clip_y1, _old_clip_x2, _old_clip_y2, _old_clip;
/* add_clip:
   Adds the given clipping area to the bitmap.
*/
static void add_clip(BITMAP *bmp, int x1, int y1, int x2, int y2)
{
   _ADIME_ASSERT(bmp != NULL, ("The bitmap was NULL."));

   _old_clip_x1 = bmp->cl;
   _old_clip_y1 = bmp->ct;
   _old_clip_x2 = bmp->cr;
   _old_clip_y2 = bmp->cb;
   _old_clip = bmp->clip;

   if (_old_clip) {
      x1 = MAX(_old_clip_x1, x1);
      x2 = MIN(_old_clip_x2, x2);
      y1 = MAX(_old_clip_y1, y1);
      y2 = MIN(_old_clip_y2, y2);

      if ((x1 > x2) || (y1 > y2))
         x1 = y1 = x2 = y2 = -1;
   }

   set_clip(bmp, x1, y1, x2, y2);
}



/* restore_clip:
   Restores the clipping area after a call to add_clip().
*/
static void restore_clip(BITMAP *bmp)
{
   if (_old_clip)
      set_clip(bmp, _old_clip_x1, _old_clip_y1, _old_clip_x2, _old_clip_y2);
   else
      set_clip(bmp, 0, 0, 0, 0);
}



/* adime_draw_empty_button_frame:
   Draws the frame of a button, without filling it.
*/
void adime_draw_empty_button_frame(BITMAP *bmp,
                                  int x1, int y1, int x2, int y2, int face,
                                  int xlight, int light, int dark, int xdark)
{
   _ADIME_ASSERT(bmp != NULL, ("The bitmap was NULL."));

   acquire_bitmap(bmp);

   hline(bmp, x1,   y1,   x2-1, xlight);
   hline(bmp, x1+1, y1+1, x2-2, light);
   hline(bmp, x1+2, y2-1, x2-1, dark);
   hline(bmp, x1+1, y2,   x2,   xdark);
   vline(bmp, x1,   y1+1, y2-1, xlight);
   vline(bmp, x1+1, y1+2, y2-2, light);
   vline(bmp, x2-1, y1+2, y2-2, dark);
   vline(bmp, x2,   y1+1, y2-1, xdark);

   putpixel(bmp, x2,   y1,   face);
   putpixel(bmp, x2-1, y1+1, face);
   putpixel(bmp, x1,   y2,   face);
   putpixel(bmp, x1+1, y2-1, face);

   release_bitmap(bmp);
}



/* adime_draw_empty_button:
   Draws an empty button.
*/
void adime_draw_empty_button(BITMAP *bmp, int x1, int y1, int x2, int y2,
                            int face, int xlight, int light, int dark, int xdark)
{
   _ADIME_ASSERT(bmp != NULL, ("The bitmap was NULL."));

   acquire_bitmap(bmp);

   adime_draw_empty_button_frame(bmp, x1, y1, x2, y2,
                                face, xlight, light, dark, xdark);
   if ((x2 - x1 >= 4) && (y2 - y1 > 4))
      rectfill(bmp, x1 + 2, y1 + 2, x2 - 2, y2 - 2, face);

   release_bitmap(bmp);
}



/* adime_draw_empty_button_down:
   Draws an empty button that is pressed down.
*/
void adime_draw_empty_button_down(BITMAP *bmp, int x1, int y1, int x2, int y2,
                                 int face, int xlight, int light, int dark,
                                 int xdark)
{
   adime_draw_empty_button(bmp, x1, y1, x2, y2,
                          face, xdark, dark, light, xlight);
}



/* adime_draw_text_button:
   Draws a filled button with text on.
*/
void adime_draw_text_button(BITMAP *bmp, int x1, int y1, int x2, int y2,
                           int face, int textcolor,
                           int xlight, int light, int dark, int xdark,
                           ADIME_CONST FONT *f, ADIME_CONST char *text)
{
   ADIME_CONST FONT *tmp_font;
   int old_text_mode;

   _ADIME_ASSERT(bmp != NULL, ("The bitmap was NULL."));

   /* Temporarily change `font' since gui_strlen() and gui_textout() use
      it. */
   tmp_font = font;
   font = (FONT *)f;

   acquire_bitmap(bmp);

   adime_draw_empty_button(bmp, x1, y1, x2, y2,
                          face, xlight, light, dark, xdark);

   add_clip(bmp, x1+2, y1+2, x2-2, y2-2);
   old_text_mode = text_mode(-1);
   gui_textout(bmp, text, (x1+x2+1) / 2,
                          (y1+y2-text_height(f)+1) / 2, textcolor, TRUE);
   text_mode(old_text_mode);
   restore_clip(bmp);

   font = (FONT *)tmp_font;

   release_bitmap(bmp);
}



/* adime_draw_text_button_down:
   Draws a filled button with text on that is pressed down.
*/
void adime_draw_text_button_down(BITMAP *bmp, int x1, int y1, int x2, int y2,
                                int face, int textcolor,
                                int xlight, int light, int dark, int xdark,
                                ADIME_CONST FONT *f, ADIME_CONST char *text)
{
   ADIME_CONST FONT *tmp_font;
   int old_text_mode;

   _ADIME_ASSERT(bmp != NULL, ("The bitmap was NULL."));

   /* Temporarily change `font' since gui_strlen() and gui_textout() use
      it. */
   tmp_font = font;
   font = (FONT *)f;

   acquire_bitmap(bmp);

   adime_draw_empty_button_down(bmp, x1, y1, x2, y2,
                               face, xlight, light, dark, xdark);

   add_clip(bmp, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
   old_text_mode = text_mode(-1);
   gui_textout(bmp, text, (x1+x2+1) / 2 + 2,
                          (y1+y2-text_height(f)+1) / 2 + 2, textcolor, TRUE);
   text_mode(old_text_mode);
   restore_clip(bmp);

   font = (FONT *)tmp_font;

   release_bitmap(bmp);
}



/* adime_draw_picture_button:
   Draws a filled button with a picture on.
*/
void adime_draw_picture_button(BITMAP *bmp, int x1, int y1, int x2, int y2,
                              int face,int xlight, int light, int dark,
                              int xdark, BITMAP *sprite)
{
   _ADIME_ASSERT(bmp != NULL, ("The bitmap was NULL."));

   acquire_bitmap(bmp);

   adime_draw_empty_button(bmp, x1, y1, x2, y2,
                          face, xlight, light, dark, xdark);

   add_clip(bmp, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
   draw_sprite(bmp, sprite,
               (x1+x2-sprite->w+1) / 2, (y1+y2-sprite->h+1) / 2);
   restore_clip(bmp);

   release_bitmap(bmp);
}



/* adime_draw_picture_button_down:
   Draws a filled button with a picture on, pressed down.
*/
void adime_draw_picture_button_down(BITMAP *bmp,
                                   int x1, int y1, int x2, int y2,
                                   int face, int xlight, int light, int dark,
                                   int xdark, BITMAP *sprite)
{
   _ADIME_ASSERT(bmp != NULL, ("The bitmap was NULL."));

   acquire_bitmap(bmp);

   adime_draw_empty_button_down(bmp, x1, y1, x2, y2,
                               face, xlight, light, dark, xdark);

   add_clip(bmp, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
   draw_sprite(bmp, sprite,
               (x1+x2-sprite->w+1) / 2 + 2, (y1+y2-sprite->h+1) / 2 + 2);
   restore_clip(bmp);

   release_bitmap(bmp);
}



/* adime_init_dialog_find_focus_object:
   Like Allegro's `init_dialog()', but focuses the first object after
   `focus_obj' that is willing to take focus, in case `focus_obj' isn't.
*/
DIALOG_PLAYER *adime_init_dialog_find_focus_object(DIALOG *dialog,
                                                  int focus_obj)
{
   DIALOG_PLAYER *player;

   player = init_dialog(dialog, focus_obj);
   if (player->focus_obj != -1)
      dialog[player->focus_obj].flags &= ~D_GOTFOCUS;

   if (focus_obj < 0) {
      player->focus_obj = player->mouse_obj;
      return player;
   }

   while ((dialog[focus_obj].proc != NULL) &&
          !(object_message(dialog+focus_obj, MSG_WANTFOCUS, 0) & D_WANTFOCUS))
      focus_obj++;

   if (dialog[focus_obj].proc == NULL)
      player->focus_obj = -1;
   else {
      dialog[focus_obj].flags |= D_GOTFOCUS;
      player->focus_obj = focus_obj;
   }

   return player;
}



/* adime_do_dialog_find_focus_object:
   Like Allegro's `do_dialog()', but focuses the first object after
   `focus_obj' that is willing to take focus, in case `focus_obj' isn't
   focusable.
*/
int adime_do_dialog_find_focus_object(DIALOG *dialog, int focus_obj)
{
   BITMAP *mouse_screen = _mouse_screen;
   int screen_count = _gfx_mode_set_count;
   void *player;

   if (!is_same_bitmap(_mouse_screen, screen))
      show_mouse(screen);

   player = adime_init_dialog_find_focus_object(dialog, focus_obj);
   while (update_dialog(player))
      ; /* do nothing */

   if (_gfx_mode_set_count == screen_count)
      show_mouse(mouse_screen);

   return shutdown_dialog(player);
}



/* resize_object:
*/
static void resize_object(DIALOG *d, int amount)
{
   d->x -= amount;
   d->y -= amount;
   d->w += amount * 2;
   d->h += amount * 2;
}



/* inwards_bevel:
   Draws a 3d bevel suitable for the checkboxes and editboxes.
*/
static void inwards_bevel(DIALOG *d, int ofs, int white_space)
{
   acquire_bitmap(_ADIME_BMP);
   if (white_space)
      rect(_ADIME_BMP, d->x+2+ofs, d->y+2+ofs,
           d->x+d->w-3-ofs, d->y+d->h-3-ofs, _ADIME_EDIT_FIELD_COLOR);
   hline(_ADIME_BMP, d->x+ofs, d->y+ofs, d->x+d->w-2-ofs,
         _ADIME_SHADOW_COLOR);
   hline(_ADIME_BMP, d->x+1+ofs, d->y+1+ofs, d->x+d->w-3-ofs,
         _ADIME_DARK_SHADOW_COLOR);
   vline(_ADIME_BMP, d->x+ofs, d->y+1+ofs, d->y+d->h-2-ofs,
         _ADIME_SHADOW_COLOR);
   vline(_ADIME_BMP, d->x+1+ofs, d->y+1+ofs, d->y+d->h-3-ofs,
         _ADIME_DARK_SHADOW_COLOR);
   hline(_ADIME_BMP, d->x+ofs+1, d->y+d->h-1-ofs, d->x+d->w-1-ofs,
         _ADIME_EDIT_FIELD_COLOR);
   hline(_ADIME_BMP, d->x+ofs+1, d->y+d->h-2-ofs, d->x+d->w-2-ofs,
         _ADIME_BACKGROUND_COLOR);
   vline(_ADIME_BMP, d->x+d->w-1-ofs, d->y+1+ofs, d->y+d->h-2-ofs,
         _ADIME_EDIT_FIELD_COLOR);
   vline(_ADIME_BMP, d->x+d->w-2-ofs, d->y+1+ofs, d->y+d->h-3-ofs,
         _ADIME_BACKGROUND_COLOR);
   release_bitmap(_ADIME_BMP);
}



/* adime_fill_textout:
*/
void adime_fill_textout(BITMAP *bmp, ADIME_CONST FONT *f, ADIME_CONST char *text,
                       int x, int y, int w, int fg, int bg)
{
   int text_w, text_h;
   int old_text_mode;

   text_w = text_length(f, text);
   text_h = text_height(f);

   old_text_mode = text_mode(bg);

   if (is_screen_bitmap(bmp))
      scare_mouse_area(x, y, x+w-1, y+text_h-1);

   add_clip(bmp, x, y, x+w-1, y+text_h-1);

   textout(bmp, f, text, x, y, fg);
   if (text_w < w)
      rectfill(bmp, x + text_w, y, x+w-1, y+text_h-1, bg);

   restore_clip(bmp);

   if (is_screen_bitmap(bmp))
      unscare_mouse();

   text_mode(old_text_mode);
}



/* adime_d_line_proc:
   Simple gui proc for drawing an orthogonal 3d line.
*/
int adime_d_line_proc(int msg, DIALOG *d, int c)
{
   _ADIME_UNUSED_ARG(c);
   if (msg == MSG_DRAW) {
      if (d->w > d->h) {
         if (d->w > 4) {
            hline(_ADIME_BMP, d->x, d->y, d->x + d->w - 1,
                  _ADIME_SHADOW_COLOR);
            hline(_ADIME_BMP, d->x, d->y + 1, d->x + d->w - 1,
                  _ADIME_LIGHT_HIGHLIGHT_COLOR);
         }
      }
      else {
         if (d->h > 4) {
            vline(_ADIME_BMP, d->x, d->y, d->y + d->h - 1,
                  _ADIME_SHADOW_COLOR);
            vline(_ADIME_BMP, d->x + 1, d->y, d->y + d->h - 1,
                  _ADIME_LIGHT_HIGHLIGHT_COLOR);
         }
      }
   }
   return D_O_K;
}



/* adime_d_check_proc:
   Dialog proc for 3d-ish check boxes.
*/
int adime_d_check_proc(int msg, DIALOG *d, int c)
{
   if (msg == MSG_DRAW) {
      inwards_bevel(d, 1, TRUE);
      rectfill(_ADIME_BMP, d->x+4, d->y+4, d->x+d->w-5, d->y+d->h-5,
               _ADIME_EDIT_FIELD_COLOR);
      if (d->flags & D_SELECTED) {
         line(_ADIME_BMP, d->x+4, d->y+4, d->x+d->w-5, d->y+d->h-5, d->fg);
         line(_ADIME_BMP, d->x+d->w-5, d->y+4, d->x+4, d->y+d->h-5, d->fg);
      }
      if (d->flags & D_GOTFOCUS)
         rect(_ADIME_BMP, d->x, d->y, d->x+d->w-1, d->y+d->h-1,
              _ADIME_BORDER_COLOR);
      else
         rect(_ADIME_BMP, d->x, d->y, d->x+d->w-1, d->y+d->h-1,
              _ADIME_BACKGROUND_COLOR);
      return D_O_K;
   }
   else
      return d_button_proc(msg, d, c);
}



/* adime_d_greyable_check_proc:
   A three-state greyable checkbox. The d1 field specifies the state.
*/
int adime_d_greyable_check_proc(int msg, DIALOG *d, int c)
{
   int old_state, new_state;

   _ADIME_UNUSED_ARG(c);

   switch (msg) {
      case MSG_DRAW:
         if (d->d1 == 2) {
            inwards_bevel(d, 1, FALSE);
            rectfill(_ADIME_BMP, d->x+3, d->y+3, d->x+d->w-4, d->y+d->h-4,
                     _ADIME_BACKGROUND_COLOR);
         }
         else {
            inwards_bevel(d, 1, TRUE);
            rectfill(_ADIME_BMP, d->x+4, d->y+4, d->x+d->w-5, d->y+d->h-5,
                     _ADIME_EDIT_FIELD_COLOR);
         }
         if (d->d1 == 1) {
            line(_ADIME_BMP,d->x+4, d->y+4, d->x+d->w-5, d->y+d->h-5, d->fg);
            line(_ADIME_BMP,d->x+d->w-5, d->y+4, d->x+4, d->y+d->h-5, d->fg);
         }
         if (d->flags & D_GOTFOCUS)
            rect(_ADIME_BMP, d->x, d->y, d->x+d->w-1, d->y+d->h-1,
                 _ADIME_BORDER_COLOR);
         else
            rect(_ADIME_BMP, d->x, d->y, d->x+d->w-1, d->y+d->h-1,
                 _ADIME_BACKGROUND_COLOR);
         break;

      case MSG_WANTFOCUS:
         return D_WANTFOCUS;

      case MSG_KEY:
         d->d1 = (d->d1+1) % 3;
         scare_mouse();
         object_message(d, MSG_DRAW, 0);
         unscare_mouse();
         break;

      case MSG_CLICK:
         /* what state was the button originally in? */
         old_state = d->d1;

         /* track the mouse until it is released */
         while (gui_mouse_b()) {
            if ((gui_mouse_x() >= d->x) &&
                (gui_mouse_y() >= d->y) &&
                (gui_mouse_x() < d->x + d->w) &&
                (gui_mouse_y() < d->y + d->h))
               new_state = (old_state+1) % 3;
            else
               new_state = old_state;

            /* redraw? */
            if (new_state != d->d1) {
               d->d1 = new_state;
               scare_mouse();
               object_message(d, MSG_DRAW, 0);
               unscare_mouse();
            }

            /* let other objects continue to animate */
            broadcast_dialog_message(MSG_IDLE, 0);
         }
         break;
   }

   return D_O_K;
}



/* adime_draw_scrollable_frame:
 *  Helper function to draw a frame for all objects with vertical scrollbars.
 *  Most of this function is stolen from Allegro.
 */
void adime_draw_scrollable_frame(DIALOG *d, int num_elements)
{
   int handle_y, handle_h;
   int virtual_h;
   int visible_elements;
   int font_h;
   int x1, x2;
   int y1, y2;

   /* draw frame */
   inwards_bevel(d, 0, FALSE);
   font_h = text_height(_ADIME_FONT);
   virtual_h = font_h * num_elements;

   /* possibly draw scrollbar */
   if (virtual_h > d->h - 4) {
      visible_elements = (d->h - 4) / font_h;
      handle_h = ((d->h - 4) * (d->h - 4) + virtual_h / 2) / virtual_h;
      handle_h = MID(1, handle_h, d->h - 5);

      handle_y = ((d->h - 5 - handle_h) * d->d2 +
                  (num_elements - visible_elements) / 2) /
                 (num_elements - visible_elements);

      x1 = d->x + d->w - 10;
      x2 = d->x + d->w - 3;
      y1 = d->y + 2 + handle_y;
      y2 = y1 + handle_h;
      if (handle_y > 0)
         rectfill(screen, x1, d->y+2, x2, y1, _ADIME_HIGHLIGHT_COLOR);

      adime_draw_empty_button(screen, x1, y1, x2, y2,
                            _ADIME_BUTTON_COLOR,
                            _ADIME_LIGHT_HIGHLIGHT_COLOR,
                            _ADIME_HIGHLIGHT_COLOR,
                            _ADIME_SHADOW_COLOR, _ADIME_DARK_SHADOW_COLOR);

      if (y2 < d->y+d->h-3)
         rectfill(screen, x1, y2+1, x2, d->y+d->h-3,
                  _ADIME_HIGHLIGHT_COLOR);
      vline(screen, x1-1, d->y+2, d->y+d->h-3, _ADIME_BACKGROUND_COLOR);
   }
}



/* adime_draw_listbox:
 *  Helper function to draw a listbox object. Most of this function is stolen
 *  from Allegro.
 */
void adime_draw_listbox(DIALOG *d)
{
   int height, listsize, i, len, bar, x, y, w;
   int fg_color, bg_color, fg, bg;
   char *sel = d->dp2;
   int font_h;
   char s[1024];
   int old_text_mode;

   (*(GETFUNCPTR)d->dp)(-1, &listsize);
   font_h = text_height(_ADIME_FONT);
   height = (d->h-3) / font_h;
   bar = (listsize > height);
   w = (bar ? d->w-12 : d->w-3);
   fg_color = (d->flags & D_DISABLED) ? gui_mg_color : d->fg;
   bg_color = d->bg;

   old_text_mode = text_mode(-1);
   /* draw box contents */
   for (i=0; i<height; i++) {
      if (d->d2+i < listsize) {
         if (d->d2+i == d->d1) {
            fg = bg_color;
            bg = fg_color;
         }
         else if ((sel) && (sel[d->d2+i])) {
            fg = bg_color;
            bg = gui_mg_color;
         }
         else {
            fg = fg_color;
            bg = bg_color;
         }
         usetc(s, 0);
         ustrncat(s, (*(GETFUNCPTR)d->dp)(i+d->d2, NULL),
                  sizeof(s)-ucwidth(0));
         x = d->x + 2;
         y = d->y + 2 + i*font_h;
         rectfill(screen, x, y, x+7, y+font_h-1, bg);
         x += 8;
         len = ustrlen(s);
         while (text_length(_ADIME_FONT, s) >= MAX(d->w - (bar?20:10), 1)) {
            len--;
            usetat(s, len, 0);
         }
         text_mode(bg);
         textout(screen, _ADIME_FONT, s, x, y, fg);
         x += text_length(_ADIME_FONT, s);
         if (x <= d->x+w)
            rectfill(screen, x, y, d->x+w, y+font_h-1, bg);
      }
      else {
         rectfill(screen, d->x+2, d->y+2+i*font_h,
                          d->x+w, d->y+1+(i+1)*font_h, bg_color);
      }
   }

   text_mode(old_text_mode);

   if (d->y+2+i*font_h <= d->y+d->h-3)
      rectfill(screen, d->x+2, d->y+2+i*font_h,
                       d->x+w, d->y+d->h-3, d->bg);

   /* draw frame, maybe with scrollbar */
   adime_draw_scrollable_frame(d, listsize);
}



/* adime_d_text_list_proc:
   Dialog procedure that makes Allegro's text_listbox more 3d'ish.
*/
int adime_d_text_list_proc(int msg, DIALOG *d, int c)
{
   FONT *old_font;
   int ret;

   old_font = font;
   font = _ADIME_FONT;

   if (msg == MSG_DRAW) {
      adime_draw_listbox(d);
      ret = D_O_K;
   }
   else
      ret = d_text_list_proc(msg, d, c);

   font = old_font;
   return ret;
}



/* adime_d_list_proc:
   Dialog procedure that makes Allegro's listbox more 3d'ish.
*/
int adime_d_list_proc(int msg, DIALOG *d, int c)
{
   FONT *old_font;
   int ret;

   old_font = font;
   font = _ADIME_FONT;

   if (msg == MSG_DRAW) {
      adime_draw_listbox(d);
      ret = D_O_K;
   }
   else
      ret = d_list_proc(msg, d, c);

   font = old_font;
   return ret;
}



/* adime_d_edit_proc:
   Dialog procedure that makes Allegro's editbox look more 3d'ish.
*/
int adime_d_edit_proc(int msg, DIALOG *d, int c)
{
   int ret;
   FONT *old_font;
   static int num_resizings = 0;
   static int ignore_next_uchar = FALSE;

   /* Simulate when Allegro's d_edit_proc() ignores uchars. */
   if (msg == MSG_CHAR) {
      if ((c >> 8) == KEY_TAB)
         ignore_next_uchar = TRUE;
      else
         ignore_next_uchar = FALSE;
   }

   if (msg == MSG_UCHAR) {
      /* It's better if we take care if this, so that we can handle negative
         values of d1. */
      if ((c >= ' ') && (uisok(c)) && (!ignore_next_uchar)) {
         if (((d->d1 > 0) &&
              (ustrlen((char *)d->dp) < d->d1)) ||
             ((d->d1 < 0) &&
              (ustrsizez((char *)d->dp) + ucwidth(c) < -d->d1))) {
            uinsert((char *)d->dp, d->d2, c);
            d->d2++;

            scare_mouse();
            object_message(d, MSG_DRAW, 0);
            unscare_mouse();
         }
         return D_USED_CHAR;
      }
      return D_O_K;
   }
   else {
      old_font = font;
      font = _ADIME_FONT;

      if (num_resizings == 0)
         if (!(d->flags & ADIME_D_EXTERNAL_BORDER))
            resize_object(d, -3);
      num_resizings++;

      ret = d_edit_proc(msg, d, c);

      num_resizings--;
      if (num_resizings == 0) {
         if (d->flags & ADIME_D_EXTERNAL_BORDER) {
            if (msg == MSG_DRAW) {
               resize_object(d, 3);
               inwards_bevel(d, 0, TRUE);
               resize_object(d, -3);
            }
         }
         else {
            resize_object(d, 3);
            if (msg == MSG_DRAW)
               inwards_bevel(d, 0, TRUE);
         }
      }

      font = old_font;

      return ret;
   }
}



/* draw_window_title:
*/
static void draw_window_title(DIALOG *d, ADIME_CONST char *text)
{
   int old_text_mode = text_mode(_ADIME_TITLE_BACKGROUND_COLOR);
   int title_height = text_height(_ADIME_TITLE_FONT);

   rectfill(_ADIME_BMP,
            d->x + adime_window_title_border_thickness + 1,
            d->y + adime_window_title_border_thickness + 1,
            d->x + d->w - adime_window_title_border_thickness - 2,
            d->y + adime_window_title_border_thickness + 1 +
                   adime_window_title_internal_border_thickness +
                   title_height +
                   adime_window_title_internal_border_thickness - 1,
                   _ADIME_TITLE_BACKGROUND_COLOR);
   adime_fill_textout(_ADIME_BMP, _ADIME_TITLE_FONT, text,
                     d->x + adime_window_title_border_thickness + 1 +
                            adime_window_title_internal_border_thickness,
                     d->y + adime_window_title_border_thickness + 1 +
                            adime_window_title_internal_border_thickness,
                     d->w - 2 * (adime_window_title_border_thickness + 1 +
                            adime_window_title_internal_border_thickness),
                     _ADIME_TITLE_TEXT_COLOR, _ADIME_TITLE_BACKGROUND_COLOR);
   hline(_ADIME_BMP,
         d->x + adime_window_title_border_thickness,
         d->y + adime_window_title_border_thickness,
         d->x + d->w - adime_window_title_border_thickness - 2,
         _ADIME_TITLE_SHADOW_COLOR);
   vline(_ADIME_BMP,
         d->x + adime_window_title_border_thickness,
         d->y + adime_window_title_border_thickness,
         d->y + adime_window_title_border_thickness +
                2 * adime_window_title_internal_border_thickness +
                title_height,
         _ADIME_TITLE_SHADOW_COLOR);
   hline(_ADIME_BMP,
         d->x + adime_window_title_border_thickness,
         d->y + adime_window_title_border_thickness +
                2 * adime_window_title_internal_border_thickness +
                title_height + 1,
         d->x + d->w - adime_window_title_border_thickness - 1,
         _ADIME_TITLE_HIGHLIGHT_COLOR);
   vline(_ADIME_BMP, d->x + d->w - adime_window_title_border_thickness - 1,
         d->y + adime_window_title_border_thickness,
         d->y + adime_window_title_border_thickness +
                2 * adime_window_title_internal_border_thickness +
                title_height,
         _ADIME_TITLE_HIGHLIGHT_COLOR);
   text_mode(old_text_mode);
}



/* adime_d_window_proc:
   Dialog procedure for drawing a window.
*/
int adime_d_window_proc(int msg, DIALOG *d, int c)
{
   char *text;

   _ADIME_UNUSED_ARG(c);

   if (msg == MSG_DRAW) {
      adime_draw_empty_button(_ADIME_BMP,
                              d->x, d->y, d->x+d->w-1, d->y+d->h-1,
                              _ADIME_BACKGROUND_COLOR,
                              _ADIME_LIGHT_HIGHLIGHT_COLOR,
                              _ADIME_HIGHLIGHT_COLOR,
                              _ADIME_SHADOW_COLOR, _ADIME_DARK_SHADOW_COLOR);
      text = (char *)d->dp;
      draw_window_title(d, text);
      return D_O_K;
   }
   else
      return D_O_K;
}



#if 0
/* Not finished */

/* adime_d_edit_proc_:
   An editable text object (the dp field points to an `EDITBOX' struct).
   When it has the input focus (obtained by clicking on it with the mouse),
   text can be typed into this object.
*/
int adime_d_edit_proc_(int msg, DIALOG *d, int c)
{
   EDITBOX *e;
   static int ignore_next_uchar = FALSE;
   int f, l, p, w, x, fg, b, scroll;
   char buf[16];
   char *s;
   int old_text_mode;

   e = d->dp;

   /* calculate maximal number of displayable characters */
/*   if (d->d2 == l)  {
      usetc(buf+usetc(buf, ' '), 0);
      x = text_length(font, buf);
   }
   else
      x = 0;

   b = 0;

   for (p=d->d2; p>=0; p--) {
      usetc(buf+usetc(buf, ugetat(s, p)), 0);
      x += text_length(font, buf);
      b++;
      if (x > d->w)
    break;
   }

   if (x <= d->w) {
      b = l;
      scroll = FALSE;
   }
   else {
      b--;
      scroll = TRUE;
   }
*/
   switch (msg) {

/*      case MSG_START:
    d->d2 = l;
    break;*/

      case MSG_DRAW:
    fg = (d->flags & D_DISABLED) ? gui_mg_color : d->fg;
    x = 0;

    if (scroll) {
       p = d->d2-b+1;
       b = d->d2;
    }
    else
       p = 0;

    for (; p<=b; p++) {
       f = ugetat(s, p);
       usetc(buf+usetc(buf, (f) ? f : ' '), 0);
       w = text_length(font, buf);
       if (x+w > d->w)
          break;
       f = ((p == d->d2) && (d->flags & D_GOTFOCUS));
       old_text_mode = text_mode((f) ? fg : d->bg);
       textout(screen, font, buf, d->x+x, d->y, (f) ? d->bg : fg);
       text_mode(old_text_mode);
       x += w;
    }
    if (x < d->w)
       rectfill(screen, d->x+x, d->y, d->x+d->w-1, d->y+text_height(font)-1, d->bg);
    break;

      case MSG_CLICK:
    x = d->x;

    if (scroll) {
       p = d->d2-b+1;
       b = d->d2;
    }
    else
       p = 0;

    for (; p<b; p++) {
       usetc(buf+usetc(buf, ugetat(s, p)), 0);
       x += text_length(font, buf);
       if (x > gui_mouse_x())
          break;
    }
    d->d2 = MID(0, p, l);
    scare_mouse();
    object_message(d, MSG_DRAW, 0);
    unscare_mouse();
    break;

      case MSG_WANTFOCUS:
      case MSG_LOSTFOCUS:
      case MSG_KEY:
    return D_WANTFOCUS;

      case MSG_CHAR:
         ignore_next_uchar = FALSE;

         if ((c >> 8) == KEY_LEFT) {
            if (d->d2 > 0)
               d->d2--;
         }
         else if ((c >> 8) == KEY_RIGHT) {
            if (d->d2 < l)
               d->d2++;
         }
         else if ((c >> 8) == KEY_HOME) {
            d->d2 = 0;
         }
         else if ((c >> 8) == KEY_END) {
            d->d2 = l;
         }
         else if ((c >> 8) == KEY_DEL) {
            if (d->d2 < l)
               uremove(s, d->d2);
         }
         else if ((c >> 8) == KEY_BACKSPACE) {
            if (d->d2 > 0) {
               d->d2--;
               uremove(s, d->d2);
            }
         }
         else if ((c >> 8) == KEY_ENTER) {
            if (d->flags & D_EXIT) {
               scare_mouse();
               object_message(d, MSG_DRAW, 0);
               unscare_mouse();
               return D_CLOSE;
            }
            else
               return D_O_K;
         }
         else if ((c >> 8) == KEY_TAB) {
            ignore_next_uchar = TRUE;
            return D_O_K;
         }
         else {
            /* don't process regular keys here: MSG_UCHAR will do that */
            break;
         }
         scare_mouse();
         object_message(d, MSG_DRAW, 0);
         unscare_mouse();
         return D_USED_CHAR;

      case MSG_UCHAR:
         if ((c >= ' ') && (uisok(c)) && (!ignore_next_uchar)) {
            if (l < d->d1) {
               uinsert(s, d->d2, c);
               d->d2++;

               scare_mouse();
               object_message(d, MSG_DRAW, 0);
               unscare_mouse();
            }
            return D_USED_CHAR;
         }
         break;
   }

   return D_O_K;
}
#endif



/* adime_d_button_proc:
   The same as d_button_proc(), but it uses my drawing functions, always
   exits when it is clicked and gets clicked when the user presses enter.
*/
int adime_d_button_proc(int msg, DIALOG *d, int c)
{
   int ofs;
   int ret, old_flags;

   if (msg == MSG_DRAW) {
      if (d->flags & D_GOTFOCUS) {
         rect(_ADIME_BMP, d->x, d->y, d->x+d->w-1, d->y+d->h-1,
              _ADIME_BORDER_COLOR);
         ofs = 1;
      }
      else
         ofs = 0;

      if (d->flags & D_SELECTED)
         adime_draw_text_button_down(_ADIME_BMP,
                                    d->x+ofs, d->y+ofs,
                                    d->x+d->w-1-ofs, d->y+d->h-1-ofs,
                                    _ADIME_BUTTON_COLOR, d->fg,
                                    _ADIME_LIGHT_HIGHLIGHT_COLOR,
                                    _ADIME_HIGHLIGHT_COLOR,
                                    _ADIME_SHADOW_COLOR,
                                    _ADIME_DARK_SHADOW_COLOR,
                                    _ADIME_BUTTON_FONT, (char *)d->dp);
      else
         adime_draw_text_button(_ADIME_BMP, d->x+ofs, d->y+ofs,
                                          d->x+d->w-1-ofs, d->y+d->h-1-ofs,
                               _ADIME_BUTTON_COLOR, d->fg,
                               _ADIME_LIGHT_HIGHLIGHT_COLOR,
                               _ADIME_HIGHLIGHT_COLOR,
                               _ADIME_SHADOW_COLOR, _ADIME_DARK_SHADOW_COLOR,
                               _ADIME_BUTTON_FONT, (char *)d->dp);
      return D_O_K;
   }
   else if ((msg == MSG_CHAR) && (c >> 8 == KEY_ENTER)) {
      return object_message(d, _ADIME_MSG_PRESS, 0) | D_USED_CHAR;
   }
   else if (msg == _ADIME_MSG_PRESS) {
      switch (d->d1) {
         case ADIME_BUTTON_TOGGLE:
            d->flags ^= D_SELECTED;
            scare_mouse();
            object_message(d, MSG_DRAW, 0);
            unscare_mouse();
            return D_O_K;

         case ADIME_BUTTON_EXIT:
            d->flags &= ~D_SELECTED;
            scare_mouse();
            object_message(d, MSG_DRAW, 0);
            unscare_mouse();
            return D_CLOSE;

         case ADIME_BUTTON_CALLBACK:
            d->flags &= ~D_SELECTED;
            scare_mouse();
            object_message(d, MSG_DRAW, 0);
            unscare_mouse();
            return ((int (*)(DIALOG *d))d->dp2)(d);

         default:
            _ADIME_ASSERT(0, ("NOTREACHED"));
            return D_O_K;
      }
   }
   else /*if (msg == MSG_CLICK)*/ {
      old_flags = d->flags;
      d->flags |= D_EXIT;
      ret = d_button_proc(msg, d, c);
      d->flags = old_flags;
      if (ret & D_CLOSE) {
         return object_message(d, _ADIME_MSG_PRESS, 0);
      }
      else
         return ret;
   }
/* else
      return d_button_proc(msg, d, c);*/
}



/* _adime_multiline_text_size:
   Finds both the width and the height of a string in pixels. The string may
   include one or more '\n' characters which will be treated as newlines.
*/
void _adime_multiline_text_size(ADIME_CONST FONT *f, ADIME_CONST char *text,
                               int *w, int *h)
{
   const char *end;
   int len;
   int this_w;
   int font_h;
   int c;

   font_h = text_height(f);

   if (w != NULL)
      *w = 0;
   if (h != NULL)
      *h = 0;

   end = text;
   c = ugetc(end);
   while (c) {
      len = 0;
      text = end;
      for (c = ugetxc(&end); c && (c != '\n'); c = ugetxc(&end))
         len++;

      if (w != NULL) {
         this_w = adime_substr_width(f, text, 0, len);
         if (this_w > *w)
            *w = this_w;
      }
      if (h != NULL)
         *h += font_h;
   }
}



/* adime_d_multiline_text_proc:
   Like d_text_proc, but with several lines.
   The d1 field specifies the alignment as one of the constants:
      ADIME_ALIGN_LEFT (or 0), ADIME_ALIGN_CENTRE or ADIME_ALIGN_RIGHT.
*/
int adime_d_multiline_text_proc(int msg, DIALOG *d, int c)
{
   if (msg == MSG_DRAW) {
      int fg = (d->flags & D_DISABLED) ? gui_mg_color : d->fg;
      FONT *f;
      int x, y;
      char *text, *end;
      int len;
      int font_h;
      int old_text_mode;

      if (d->dp2)
         f = d->dp2;
      else
         f = font;

      font_h = text_height(f);
      old_text_mode = text_mode(d->bg);

      x = d->x;
      y = d->y;
      end = (char *)d->dp;
      /* Draw each line of text. */
      do {
         /* Find end of line. */
         len = 0;
         text = end;
         for (c = ugetx(&end); c && (c != '\n'); c = ugetx(&end))
            len++;

         /* Find position to draw at. */
         if (d->d1 == ADIME_ALIGN_RIGHT)
            x = d->x + d->w - adime_substr_width(f, text, 0, len);
         else if (d->d1 == ADIME_ALIGN_CENTRE)
            x = d->x + (d->w - adime_substr_width(f, text, 0, len)) / 2;

         /* Draw. */
         adime_draw_substr(screen, f, text, x, y, fg, 0, len);

         y += font_h;
      } while (c == '\n');
      text_mode(old_text_mode);
   }

   return D_O_K;
}



/* adime_char_width:
   Returns the width of one single character of a font. (Allegro only has a
   function for strings). This also treats NULL fonts as having all
   characters one pixel wide, which is useful in a couple of cases when I
   want a NULL font to mean that we're writing to a text screen.
*/
int adime_char_width(ADIME_CONST FONT *f, int c)
{
   char text[16];
   usetc(text + usetc(text, c), 0);
   return text_length(f, text);
}



/* adime_substr_width:
   Returns the width of only part of a string.
*/
int adime_substr_width(ADIME_CONST FONT *f, ADIME_CONST char *text,
                      int start, int length)
{
   int i, c;
   int w = 0;

   _ADIME_ASSERT(text != NULL, ("The text was NULL."));

   text += uoffset(text, start);

   for (i = 0; i < length; i++) {
      c = ugetxc(&text);
      if (!c)
         break;
      w += adime_char_width(f, c);
   }

   return w;
}



/* adime_draw_substr:
   Draws part of a string.
*/
void adime_draw_substr(BITMAP *bmp, ADIME_CONST FONT *f, ADIME_CONST char *text,
                      int x, int y, int color, int start, int length)
{
   int end_pos = 0;
   char tmp[512];
   int empty_width = uwidth(EMPTY_STRING);
   int i;
   int current_length;

   _ADIME_ASSERT(bmp != NULL, ("The bitmap was NULL."));
   _ADIME_ASSERT(f != NULL, ("The font was NULL."));
   _ADIME_ASSERT(text != NULL, ("The text was NULL."));

   /* I couldn't think of any way to do it both fast and beautiful; this is
      neither :o) The string is copied into a temporary buffer, and then
      textout() is called with this buffer.
   */
   text += uoffset(text, start);
   for (i = 0; ugetc(text + end_pos) && (i < length); i++) {
      current_length = uwidth(text + end_pos);
      end_pos += current_length;
      if (end_pos > 512 - empty_width) {
         end_pos -= current_length;
         memcpy(tmp, text, end_pos);
         memcpy(tmp + end_pos, EMPTY_STRING, empty_width);
         textout(bmp, f, tmp, x, y, color);
         x += text_length(font, tmp);
         text += end_pos;
         end_pos = current_length;
         break;
      }
   }
   memcpy(tmp, text, end_pos);
   memcpy(tmp + end_pos, EMPTY_STRING, empty_width);
   textout(bmp, f, tmp, x, y, color);

/* We shouldn't use this since it may be forbidden to patch in a char* even
   though we restore it: String constants may be read-only.

   pos = uoffset(text, start);
   memcpy(backup, text + pos, empty_width);
   usetc(text + pos, empty_char);

   textout(bmp, f, text, x, y, color);

   memcpy(text + pos, backup, empty_width);
*/
}
