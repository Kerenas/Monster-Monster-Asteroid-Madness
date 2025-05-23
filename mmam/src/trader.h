#ifndef _trader_h
#define _trader_h 1

typedef class inven_t {
    public:
		int count, plus, ivalu, price, pointer_is_mine;
		struct weap_t *w;
		class monst_t *m;
		class cage_t *cage;
		inven_t(int i, int n = -1);
        inven_t(struct weap_t *wp, int p = 1);
        inven_t(class monst_t *mon, int p = 1);
        inven_t(class cage_t *c, int p = 1);
        ~inven_t();
        char *name();  char *desc();
        int lvl();     int rnd();
        struct ivalu_t *inv();
        int group_buy();
};

extern vector <class inven_t *> applicants;
    

typedef class trader_t {
  private:
	int scroll_dir, time_scroll, start_talk, sylabals;
	int *raw_puppet;
	int raw_puppet_idx;
	void make_sell_list(monst_t *monst, int mode = 0);
	void poll_menus();
	int  button_is_for(int btn);
	void make_pit_inv();
	void make_trade_inv();
	void draw_face();
	void draw_menu_buy();
	void draw_menu_sell();
	void draw_menu_park();
	void draw_menu_fight();
	void draw_menu_cryo();
	void draw_menu_lab();
	void draw_menu_alignment();
	void draw_menu_scan();
	void draw_menu_target(int menu_style);
	void draw_menu_graphs();
	void draw_menu_encl(int menu_num); 
    void draw_menu_jump(int menu_num = 0);
	void draw_menu_power();
	void draw_menu_hire();
	void draw_menu_worker();
	void draw_menu_payroll();
	int poll_menu_buy();
	int poll_menu_sell();
	int poll_menu_park();
	int poll_menu_fight();
	void park_advice();
	//-----
	void render_gear(BITMAP *dest, BITMAP *tooth, int x, int y, int radius, int teeth, float ang );
	void draw_gearbox();

	void draw_exit();
	void draw_powertog();
	void draw_upgrade();
	void draw_scroll_bar(int num, int top, int dispcount, int y1, int y2); 
	void draw_menu_frame(char *title);
	//void draw_mi(int idx, char *s, int enabled);
	int  draw_menu_but( int x, int y, char *s, int ret, int sel = 0, int stack_dir = 0);
	int  draw_mini_but( int x, int y, char *s, int ret, int sel = 0, int stack_dir = 0);	
    void draw_hire_list();
	void short_stock_phrase(class inven_t *i);
	void buy_phrase(class inven_t *i);
	void no_space_phrase(class inven_t *i);
	void short_on_cash_phrase(class inven_t *i);
	void sell_phrase(class inven_t *i, int n);
	void view_switch_phrase();
	int  build_battle_list();
  public:
  	char *name;
    monst_t *link;
    monst_t *watch_monst;
  	BITMAP *kmenu;
  	char *said;
  	int said_mood, said_done, ox, geararm_x, geararm_y;
  	int picture_idx, picture_layout_idx, gibberish_idx, gibberish_nc, building_idx, menu_num, focus, top_buy_item;
    int jg_first, jg_respawn_time, jg_autojump;
    int bs_grp;
	int price_sheet, enable_buy, enable_sell, enable_park, enable_fight,
		enable_cryo, enable_lab, enable_jump, enable_turret, enable_encl,
		enable_power, enable_scan, enable_graphs, enable_worker, enable_hire;
	float price_mult;
	vector <class inven_t *> inventory;
    trader_t(int who, monst_t *attach_to = NULL);
    ~trader_t();
	void scroll_in();
	void scroll_out();
	void swap_in();
	void generate_applicant();
	void add_item(int i, int n, monst_t *sell_to = NULL);
	void add_cage(class cage_t* c, monst_t *sell_to = NULL); 
	void add_weap(struct weap_t *wp, monst_t *sell_to = NULL);
	void add_monst(class monst_t *monst, monst_t *sell_to = NULL);
	void add_monst(struct monst_base *mb, monst_t *sell_to = NULL);
	void forget(monst_t *m) { if (watch_monst == m) watch_monst = NULL;}
	void forget_all() { watch_monst = NULL; }
    void draw_item_list();
	int  wants_monst(monst_t *monst);
    void poll();
    void draw();        
    int  jg_open(int n = -1);
	void say(char *s, int mood = 0, int sylabals = -1);
	void sayrand(char *s[], int mood = 0, int sylabals = -1);
	void greeting();
	void goodbye();
	int  extended() { return (ox >= SCREEN_W - 800);};
	void save(PACKFILE *pf, int flags);
	int  parse(char *cmd, char *next, int line);
};

extern trader_t *trader;

#endif
