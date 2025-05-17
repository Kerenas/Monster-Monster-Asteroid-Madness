#include "gamelog.h"
#include "bubbles.h"
#define GAMELOG_X (20)
#define top 0

MANUAL_MOUSE *manual_mouse;
GAMELOG *gamelog;
QBOX *gameqbox;
FREE_EDIT *free_edit;
BITMAP *mouse_arrow;

//============================================================================
void GAMELOG::add(char *format, ...) {
	char buff[800];
	va_list args;
	va_start(args, format);
	vsnprintf(buff, sizeof buff, format, args);
        va_end(args);
	free(glog[NUMLOGENTS-1]);
	for (int c = NUMLOGENTS-1; c > 0; c--) glog[c] = glog[c - 1];
	glog[0] = strdup(buff);
	dirty_count = MAKE_DIRTY;
	//printf("> %s\n", glog[0]);
}

void GAMELOG::draw() {
        if (!visible) {
          	if (inv_dirty ) {
		//	blit((page_flip ? vram_mmam_splash : mmam_splash), active_page, GAMELOG_X, top, GAMELOG_X,  top , 220, SCREEN_H - top);
			if (inv_dirty > 0) inv_dirty--;
             	}
        } else { 
        //if (dirty_count || forced_redraw_flag) {
	// blit(mmam_splash, active_page, GAMELOG_X, top, GAMELOG_X,  top , 220, SCREEN_H - top);
        	drawing_mode(DRAW_MODE_TRANS, NULL, 0,0);
		for (int cy = 0; cy < NUMLOGENTS; cy++) {
             		if (glog[cy]) {
   				int inten = 255 - (cy < NUMLOGENTS - 8 ? 0 : (NUMLOGENTS - cy) * 32);
				set_trans_blender(inten,inten,inten,inten );
	  			textout_ex(active_page, &font_super_micro, glog[cy], GAMELOG_X, SCREEN_H - (60 + (cy * 12)), White, -1);
	     		}
		}
		solid_mode();
		if (dirty_count > 0) dirty_count--;
	}
}

void GAMELOG::set_visible(int i) {
	if ((visible = i)) {
		dirty_count = MAKE_DIRTY;
   	} else inv_dirty = MAKE_DIRTY;
}

//============================================================================

int FREE_EDIT::poll() {
	if (!visible) {
   		clear_keybuf();
		return 0;
	}
	if (!keypressed()) return 0;
	dirty_count = MAKE_DIRTY;
        int c = readkey();
        if (visible) switch (c >> 8) {
	case KEY_LEFT: {
        		if (key_shifts & KB_CTRL_FLAG) while (cur_pos && str[cur_pos]!= ' ') cur_pos--;
        		else if (cur_pos) cur_pos--;
        	}
        	break;
        case KEY_RIGHT: {
        		if (key_shifts & KB_CTRL_FLAG) while (str[cur_pos] && str[cur_pos]!= ' ') cur_pos++;
        		else if (str[cur_pos]) cur_pos++;
        	}
        	break;
	case KEY_HOME:  cur_pos = 0; break;
	case KEY_END:   while (str[cur_pos]) cur_pos++; break;
	case KEY_DEL:   del(); break;
	case KEY_BACKSPACE: if (cur_pos) { cur_pos--; del(); } break;
	case KEY_INSERT: ins_flag ^= 1; break;
	case KEY_ENTER: visible = 0; forced_redraw_flag = MAKE_DIRTY; return 1;
        default : ;
	}
	c &= 0xff;
	if (c >= ' ') { ins(c); visible = 1; }
	return 0;
}

void FREE_EDIT::draw() {
        if (!visible) return;
        // if (dirty_count || forced_redraw_flag)
        {
        	int x = 12;            	  int y = SCREEN_H - 20;
        	int x2 = SCREEN_W - 200;  int y2 = SCREEN_H - 4;
        	blit(mmam_splash,active_page, x+1,y+1,x+1,y+1,x2-x-2,y2-y-2);
        	textout_ex(active_page, &font_bold, str, x + 4,y + 1, White, -1);
                rect(active_page, x,y, x2, y2, White);
                if (dirty_count) dirty_count--;
        }
}


//===================================================================

void QBOX::draw() {
  	if (!visible) return;
	int sw  = 6;
	//                              S1             S2               X       Y
	//blit(mmam_splash, active_page, rs,      ts+sw, rs,      ts+sw, sw,      bs-ts);
	//blit(mmam_splash, active_page, ls + sw, bs+1,  ls + sw, bs+1,  rs - ls, sw);
	drawing_mode(DRAW_MODE_TRANS, NULL, 0,0);
	set_trans_blender(90, 90, 90, 90);
	int shc = makecol(20, 20, 20);
      	rectfill(active_page, rs, ts+sw,rs+sw,bs,shc);
      	rectfill(active_page, ls + sw, bs+1,rs+sw,bs+sw,shc);
      	solid_mode();
      	rectfill(active_page, rs,ts,ls,bs, Purple);
	rectfill(active_page,
		SCREEN_W / 2 - askwid,
		SCREEN_H / 2 - askhi + qbar,
		SCREEN_W / 2 + askwid,
        	SCREEN_H / 2 + askhi, BluishPurple
 	);
	if (text) {
		if (text_length(&font_bold, text) < 2 * askwid) {
			textout_centre_ex(active_page, &font_bold, text, SCREEN_W / 2, SCREEN_H / 2 - askhi + 5, White, - 1);
		} else textout_box(active_page, &font_bold, text,
			SCREEN_W / 2 - askwid+8, SCREEN_H / 2 - askhi + 5,
			askwid * 2 - 16, askhi * 2-8, White, BluishPurple
		);
	}
	rectfill(active_page, SCREEN_W / 2 + 2, SCREEN_H / 2 + bdn, SCREEN_W / 2 + butwid, SCREEN_H / 2 + bdn + buthi, LightGray);
	rectfill(active_page,SCREEN_W / 2 - 2, SCREEN_H / 2 + bdn, SCREEN_W / 2 - butwid, SCREEN_H / 2 + bdn + buthi, LightGray);
	textout_centre_ex(active_page, &font_bold, "yes", SCREEN_W / 2 - butwid / 2 - 2, SCREEN_H / 2 + bdn, White, -1);
	textout_centre_ex(active_page, &font_bold, "no",  SCREEN_W / 2 + butwid / 2 + 2, SCREEN_H / 2 + bdn, White, -1);
      	rect(active_page, ls + 2,ts + 2, rs - 2, bs - 2, White);
}

void QBOX::ask(char *s) {
  	visible = 1;
	rs = SCREEN_W / 2 + askwid;
	bs = SCREEN_H / 2 + askhi;
	ts = SCREEN_H / 2 - askhi;
	ls = SCREEN_W / 2 - askwid;
	text = strdup(s);
}

int QBOX::poll() {
	int res = -1;
	if (key[KEY_Y]) res = 1;
	if (key[KEY_N]) res = 0;
	if ((mouse_b & 0x1) &&
	    mouse_x >= SCREEN_W / 2 - butwid &&
	    mouse_y <= SCREEN_W / 2 + butwid &&
	    mouse_y >= SCREEN_H / 2 + bdn &&
	    mouse_y <= SCREEN_H / 2 + bdn + buthi
	) {
        	res = (mouse_x < SCREEN_W / 2 ? 1 : 0);
	}
	if (res >= 0) {
   		visible = 0;
		free(text); 
		text = NULL;
	}
	return res;
}

QBOX::QBOX() {
      askwid = 180;
      askhi = 50;
      qbar = 20;
      butwid = 80;
      buthi = 15;
      bdn = 20;
      visible = 0;
}

int stdio_printf(char *format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
        va_end(args);
}

