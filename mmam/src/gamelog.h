#ifndef _gamelog_h
#define _gamelog_h 1
#include "config.h"
#include "mmam.h"
#define NUMLOGENTS 34

typedef class GAMELOG {
   private:
      int  dirty_count;
      char *glog[NUMLOGENTS];
      int inv_dirty;
      char scratch[2000];
      char *eos;
   public:
      int  visible;
      GAMELOG() { for (int c = 0; c <NUMLOGENTS; c++) glog[c] = NULL; visible = 0; };
      ~GAMELOG() { for (int c = 0; c <NUMLOGENTS; c++) if (glog[c]) free(glog[c] ); };
      void add(char *format, ...);
      void set_visible(int i);
      void draw();
};


typedef class QBOX {
  private:
        int  visible;
        int askwid, askhi, rs, bs, ts, ls, qbar, butwid,  buthi, bdn;
        char *text;
  public:
        QBOX();
	void ask(char *s);
	int  poll();
	void draw();
};

extern BITMAP *mouse_arrow;

typedef class MANUAL_MOUSE {
    private:
	int x[2],y[2];
	BITMAP *b[2];
    public:
        int ok;
        MANUAL_MOUSE() {
        	b[0] = create_video_bitmap(20, 20);
	        b[1] = create_video_bitmap(20, 20);
	        ok = (b[0] && b[1]);
	}
        ~MANUAL_MOUSE() { destroy_bitmap(b[0]); destroy_bitmap(b[1]); }
        void show() {
        	int c =(active_page == page1 ? 0 : 1); x[c] = mouse_x; y[c] = mouse_y;
        	blit( active_page, b[c], x[c], y[c], 0, 0, b[c]->w, b[c]->h);
        	draw_sprite(active_page, mouse_arrow, x[c], y[c]);
        }
        void hide() {
        	int c =(active_page == page1 ? 0 : 1);
        	blit( b[c], active_page, 0, 0, x[c], y[c], b[c]->w, b[c]->h);
        }
};

typedef class FREE_EDIT {
	private:
		char *str;
		int max_size;
		int cur_pos;
		int ins_flag;
		int dirty_count;
		void ins(char ch) {
			if (cur_pos >= max_size - 1) return;
			if (ins_flag) memmove(str + cur_pos + 1, str + cur_pos, max_size - cur_pos - 2);
			str[cur_pos] = ch;
			if (cur_pos < max_size - 1) cur_pos++;
		}
		void del() {
                	memmove(str + cur_pos, str + cur_pos + 1, max_size - cur_pos);
		}
	public:
		int visible;
		int poll();
		void clear() { memset(str, 0, max_size); cur_pos = 0; }
		void draw();
		FREE_EDIT(char *s, int siz) {str = s; max_size = siz; clear(); ins_flag = 1; visible = 0; }
};


extern FREE_EDIT *free_edit;
extern MANUAL_MOUSE *manual_mouse;
extern GAMELOG *gamelog;
extern QBOX *gameqbox;

int stdio_printf(char *format ...);

#endif

