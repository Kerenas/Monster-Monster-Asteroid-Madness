#ifndef _hud_h
#define _hud_h 1
typedef enum hud_menu_t {hmenu_none, hmenu_caged, hmenu_food,
	hmenu_inject, hmenu_trader, hmenu_arena,
	hmenu_animal_info, hmenu_inventory, hmenu_watch_item,
};
typedef enum hud_inv_act {hinv_act_cage, hinv_view, hinv_drop, hinv_sell, hinv_use};
enum {news_std, news_addend, news_ignore};

#define MAX_JAILCELLS 12
#define HUD_MENU_EXIT 0xffff
#define MENU_EXIT 0xfffe
#define MENU_YES 0xfffd
#define MENU_NO  0xfffc
#define MENU_POWTOG 0xfffb
#define MENU_UPGRADE 0xfffa
#define MENU_TOGAUTOJUMP 0xfff9
#define MENU_JUMPOPEN  0xfff8
#define MENU_JUMPCLOSE 0xfff7
#define HUD_CLAW 0xfff6
#define HUD_WEAPON 0xfff5
#define HUD_DROP_WEAPON 0xfff4

#define MENU_BUY0 0x10000
#define MENU_SUB0 0x20000
#define MENU_EXTRA0 0x30000	
#define MENU_JUMP0 0x40000
#define MENU_POWTOG0 0x50000
#define MENU_TARGET0 0x60000
#define MENU_BUYN0 0x70000


typedef class hud_t {
  private:
  	BITMAP *hud_bmp;
        int scroll_dir, time_scroll;
        char *short_news_str, *long_news_str;
        int short_news_time, long_news_time;
	int menu_num;
	void poll_claw();
        void fishing(float &hooked_monst_va, float &ship_va);
	void yank(float &t_free, float &t_strain, float &As, float &Am,  float &Vs, float &Vm);
	void draw_monster_info(class monst_t *look_monst);
	void draw_bars();
	void draw_timer_circle();
	void draw_exit();
	void draw_menu_frame(char *title);
	void draw_mi(int idx, char *s, int enabled);
	void draw_mi2( int idx, char *s, int n);
	void draw_menu_caged();
	void draw_menu_food();
	void draw_menu_inject();
	void draw_menu_trader();
	void draw_menu_arena();
	void draw_watch_item();
	void poll_menus();
  public:
        hud_t(); ~hud_t();
  	class cage_t *jail[MAX_JAILCELLS];
  	class monst_t *hooked_monst, *watch_monst, *scan_monst, *track_monst;
  	int cage_sel, inventory_act, next_cage;
        int claw_test, claw_drag, claw_stress, ox;
        int fire_claw_dir, pin_time, flash_time, shake_time;
        float claw_line_out, claw_line, claw_x, claw_y, claw_vx, claw_vy, claw_ang, claw_vel;
        int food[10], injector[10];
        char marked[1000];
        int cage_info_upgrade, claw_level, rope_level;
        int thrust_level, hull_level, claw_speed_level, spr_claw[2], money, karma, fame;
        struct ivalu_t *watch_item;
        int last_tour_grp, ticket_price, hunting_allowed, hunting_price;
        int news_mode;
        float high_force;
		int high_force_time;
	void poll();
	int  capture(monst_t *monst);
	void short_news(char *format, ...);
	void long_news(char *format, ...);
	void draw();
	void scroll_in();
        void scroll_out();
        int  mm_drop_monst(int query, cage_t *cage, int cage_num);   // 0
        int  mm_id_monst(int query, cage_t *cage, int cage_num);     // 1
        int  mm_feed_monst(int query, cage_t *cage, int cage_num);   // 2
        int  mm_inject_monst(int query, cage_t *cage, int cage_num); // 3
        int  mm_sell_monst(int query, cage_t *cage, int cage_num);   // 5
        int  mm_repair_cage(int query, cage_t *cage, int cage_num);  // 6
        int  mm_sell_cage(int query, cage_t *cage, int cage_num);    // 7
        void menu_exit() {menu_num = hmenu_none; cage_sel = -1;inventory_act = hinv_act_cage; };
        void forget(monst_t *monst);
	void forget_all();
	int  upgrade(struct ivalu_t *t, int n = -1);
	void show_item_info(struct ivalu_t *k);
	void set_menu(int m) { menu_num = m; watch_item = NULL; watch_monst = NULL; };
	void save(PACKFILE *pf, int flags);
	int  parse(char *cmd, char *next, int line);
};


typedef class monsterdex_t {
  private:
	int x, y;
	class monst_t *m;
  public:
        monsterdex_t(monst_t *monst);
        void poll();
        void draw();
};



#endif
