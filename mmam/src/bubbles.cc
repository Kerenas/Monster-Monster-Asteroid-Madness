#include "mmam.h"
// #include <allegro.h>
#include "bubbles.h"
// static BITMAP *player_chat_bubble[maxplayers + 1];

BITMAP *blood_pool[6];

void init_blood_pool() {
	for (int c = 0; c < 2; c++) {
		blood_pool[c] = create_bitmap(45,45);
		clear_to_color(blood_pool[c], bitmap_mask_color(blood_pool[c]));
		circlefill(blood_pool[c], 23,23,23, (c==0? Red : Green));
	}
}

void create_player_chat_bubbles() {
        /*
	if (player_chat_bubble[0]) return;
  	int sx = 19;
  	for (int c = 1; c <= maxplayers; c++) {
                int r = getr(playerc[c]); int g = getg(playerc[c]); int b = getb(playerc[c]);
		float h,s,v;
		rgb_to_hsv(r,g,b,&h,&s,&v);
  		BITMAP *pcb = player_chat_bubble[c] = create_bitmap(sx,sx);
  		clear_to_color(pcb, bitmap_mask_color(pcb));
		for (int i=sx / 2; i; i--) {
            		hsv_to_rgb(h,(s * i) / (sx), 0.85, &r,&g,&b);
       			circlefill(pcb, sx / 2, sx/ 2, i, makecol(r,g,b));
        	}
  	}
	*/
}

/*
static void down_scale(double sf) {
	if (sf >= 0.99 ||sf <=0) return;
        int nw = (int)(floor( (sqwid - BORDER_EDGE) * sf));
        int nh = (int)(floor( (sqhi - BORDER_EDGE) * sf));
	for (int c=0; c < SPR_COUNT; c++) if (c != SPR_SPLASH_BMP && c != SPR_ARROW_BMP) {
		BITMAP *spr = (BITMAP *) sprites[c].dat;
        	BITMAP *tb = create_bitmap(nw,nh);
        	if (tb) {
        		stretch_blit(spr, tb, 0,0, spr->w, spr->h, 0,0, tb->w, tb->h);
        		destroy_bitmap(spr);
			(BITMAP *) sprites[c].dat = tb;
               }
	}
}

*/

void box_blit(BITMAP *to, BITMAP *from, int x, int y, int vx, int vy) {
  	int fw = (from->w / 2);
  	int fh = (from->h / 2);
  	if (vx < fw * 2) vx = fw * 2;
        if (vy < fh * 2) vy = fh * 2;
  	x += fw;   y += fh;
  	vx -= 2 * fw;   vy -= 2 * fh;
        int yc, yc2, xc, xc2;
        for (yc = y - fh, yc2 = 0;       yc2 < fh;       yc++, yc2++) {
        	hline(to,x, yc, x+vx, getpixel(from,fw+1, yc2));
	}
        for (yc = y + vy, yc2 = fh + 1;  yc2 < from->h;  yc++, yc2++) {
        	hline(to,x, yc, x+vx, getpixel(from,fw+1, yc2));
	}

        for (xc = x - fw, xc2 = 0;       xc2 < fw;       xc++, xc2++) {
        	vline(to, xc, y, y+vy, getpixel(from, xc2, fh+1));
	}
        for (xc = x + vx, xc2 = fw + 1;  xc2 < from->w;  xc++, xc2++) {
        	vline(to, xc, y, y+vy, getpixel(from, xc2, fh+1));
	}
	masked_blit(from, to, 0,    0,    x - fw, y - fh, fw, fh);
	masked_blit(from, to, 0,    fh+1, x - fw, y + vy, fw, fh);
	masked_blit(from, to, fw+1, 0,    x + vx, y - fh, fw, fh);
	masked_blit(from, to, fw+1, fh+1, x + vx, y + vy, fw, fh);
        rectfill(to, x,y, x+vx, y+vy, getpixel(from,fw+1, fh+1));
}

void sprite_rsw(BITMAP *dest, BITMAP *src, int x, int y, float ang, int new_w, int trans) {
	if (new_w <= 0 || trans <= 0) return;
        if (trans >= 255) {
	   rotate_scaled_sprite(dest, src, x - (new_w / 2), y - (new_w / 2), ftofix(0x80 * ang / M_PI), (new_w << 16)/src->w);
	} else  if (trans >= 15){
	   int sqs = MAX(src->h, src->w) * new_w / src->w;
	   int bs = (sqs | 0x4) &~ 0x3; // work arround for bug in draw_trans_sprite
           BITMAP *t = create_bitmap(bs, bs);
	   if (t) {
           	clear_to_color(t, bitmap_mask_color(t));
           	rotate_scaled_sprite(t, src, (sqs - new_w) / 2, (sqs - new_w) / 2, ftofix(0x80 * ang / M_PI), (new_w << 16)/src->w);
           	set_trans_blender(0, 0, 0, trans);
           	draw_trans_sprite(dest, t, x - sqs / 2, y - sqs / 2);
           	destroy_bitmap(t);
	   }
	}
}

static BITMAP* shadow_list[SPR_COUNT];

void shadow_rsw(BITMAP *dest, int shadow_idx, int x, int y, float ang, int new_w) {
     if (!shadow_list[shadow_idx]) {
        BITMAP *src = (BITMAP *)sprites[shadow_idx].dat;
        BITMAP *shad = shadow_list[shadow_idx] = create_bitmap(src->w, src->h);
        blit(src, shadow_list[shadow_idx], 0, 0, 0, 0, src->w, src->h);
        int pink = bitmap_mask_color(shad);
        for (int y = 0; y < shad->h; y++) for (int x = 0; x < shad->w; x++) {
            if (getpixel(shad, x, y) != pink) putpixel(shad, x, y, Black);
        }
     }
     sprite_rsw(dest, shadow_list[shadow_idx], x, y, ang, new_w, 127);
}

void bar_draw_h(BITMAP *dest, BITMAP *src, int x, int y, int percent) {
	percent  = MID(0, percent, 100);
	int mark = src->w * percent / 100;
        if (percent) blit(src,dest,0,0,x,y, mark, src->h);
        if (percent < 100) rectfill(dest, x+mark, y, x+ src->w - 1, y + src->h -1, 0);
}

void textout_box2(BITMAP *dest, FONT *font, char **s, int *j, int x, int y, int w, int h, int color, int bg, int v_space) {
	int ye = y + h - v_space;
	for (int c = 0; s[c] && y <= ye; c++) {
                if (j[c]) textout_justify_ex(dest, font, s[c], x, x+w, y, w, color, bg);
                else textout_ex(dest, font, s[c], x, y, color, bg);
		y += v_space;
   	}
}

int _textout_x, _textout_y;
char *_textout_text;

void textout_box(BITMAP *dest, FONT *font, char *thetext,
	int x, int y, int w, int h, int fg, int bg, int v_space,
	int offset, int *listsize)
{
   if (bg != -1) rectfill(dest, x, y, x+w-1, y+h-1, bg);   
   if (!thetext || !dest || !font) return;
   if (v_space < 0) v_space = text_height(font);
   // int tabsize = MID(text_length(font, "   "), w / 8, text_length(font, "        "));
   int yo = y - offset * v_space;
   char s[200] = "";
   char *wm, *tt, *lm, *ss;
   wm = tt = lm =  thetext;
   ss = s;
   int sm = 0, lf = 0;
   while (1) {
         *ss = *tt;
         switch (*tt) {
         case '\t':
         case ' ': sm++; break;
         case '\r': break;
         case 0:    lf = 1; wm = tt; break;
         case '\n': lf = 1; wm = tt+1; break;
         default:
           if (sm) {
             wm = tt;
             for (;sm;sm--) { *ss=' '; ss++;}
             *ss = 0;
             if (text_length(font, s) >= w) lf = 1;
           }
           if (!lf) {
              *ss = *tt;
              ss++;
      	   }
           break;
         } // switch
         *ss = 0;
         if (lm != wm && (text_length(font, s) >= w)) {
            lf = 1;
            for (;ss != s && *ss != ' '; ss--);
            for (;ss != s && *ss == ' '; ss--);
            if (ss != s) ss++;
            *ss = 0;
    	 }
         if (lf) {
            if (yo + text_height(font) > y + h) return;
            if (yo >= y) {
	        	textout_ex(dest, font, s, x, yo, fg, bg);
				_textout_x = x + text_length(font, s);
				_textout_y = yo;
				_textout_text = *wm == 0 ? NULL : s + 1;        	
	    	}
            yo += v_space;
            ss = s;
            tt = lm = wm;
            lf = 0;
            if (*tt == 0) return;
         } else tt++;
   }
}

