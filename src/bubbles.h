//bubbles.h
#ifndef _bubbles_h
#define _bubbles_h

void create_player_chat_bubbles();
void box_blit(BITMAP *to, BITMAP *from, int x, int y, int vx, int vy);
void sprite_rsw(BITMAP *dest, BITMAP *src, int x, int y, float ang, int new_w, int trans = 255);
void shadow_rsw(BITMAP *dest, int shadow_idx, int x, int y, float ang, int new_w);
void bar_draw_h(BITMAP *dest, BITMAP *src, int x, int y, int percent);
void textout_box(BITMAP *dest, FONT *font, char *thetext,
	int x, int y, int w, int h, int fg, int bg = -1, int v_spaceing = -1,
	int offset = 0, int *listsize = NULL);

extern int _textout_x, _textout_y;
extern char *_textout_text;

#endif

