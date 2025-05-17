#ifndef _asteroid_h
#define _asteroid_h 1

typedef list <class monst_t*>::iterator monst_iter;
typedef list <class blood_t*>::iterator blood_iter;
typedef list <class killer_particle_t*>::iterator kp_iter;
typedef list <class jg_particle_t*>::iterator jgp_iter;


typedef class asteroid_t {
  private:
	int tw,th;
    int **grid;
	list<class blood_t*> blood;
	list<class jg_particle_t*> jg_particle;
	list<class killer_particle_t*> bullets;
	list<class killer_particle_t*> temp_bullets;
	void main_jumpgate_draw();
  public:
  	int pw,ph;
  	char *style;
	list <monst_t *> m;
	int building_sign, arena_glyph;
	int artifact_idx, artifact_x, artifact_y, mimmack_mood;
	int jumpgate, jumpgate_x, jumpgate_y;
	int camera_x, camera_y, asteroid_idx;
	int exit_time, start_time, skin_num;
	int orgs_here[org_last];
	int orgs_powerg[org_last], orgs_poweru[org_last], orgs_powerr[org_last];
	int scanner_count[5];
    int stabilizer_lvl;
	int monst_iter_unsafe;
	class weather_t *weather;
  	asteroid_t(int idx, int in_script = 0, monst_t *set_encl = NULL);
	~asteroid_t();
	void populate(int idx);
	int what_is(int x, int y);
	int building_is();
	int has_arena() { return (arena_glyph >= 0);}
	int can_add(float x, float y, float r);
	void add_monst(monst_t *monst);
	void add_monst(monst_t *monst, float x, float y);
	int  add_monst_by(monst_t *monst, float x, float y, int avoid_danger = 1);
	void _forget_monst(monst_t *monst); // invalidates pointer without touching m list.
	list <monst_t *>::iterator delete_monst(list <monst_t *>::iterator mon);
	void delete_monst(monst_t *monst);
	void unhook_monst(monst_t *monst);
	monst_t *spawn_monst_by(monst_base *mb, float x, float y, int n = 1,  int jump_out_time = 0, float rngfar = 100., float rngnear = 20.);
	monst_t *spawn_monst(monst_base *mb, int n = 1, int jump_out_time = 0);
	int      spawn_monst(monst_t *nmonst, int jump_out_time = 0);
	void spawn_city(int idx = 0, int city_flags = 0);
	monst_t *monst_near(float atx, float aty, float atr, monst_t *ignore = NULL, int show_dead = 1);        
	monst_t *monst_at(float atx, float aty, monst_t *ignore = NULL, int show_dead = 1);
	monst_t *monst_at(PMASK *mask, float atx, float aty, monst_t *ignore = NULL, int show_dead = 1);
	void add_jumpgate(float x, float y, int typ = SPR_JUMPGATE);
	void spatter_blood(monst_t *m, float ang, float vel, int amt, int alt_fluid = 0);
	void add_bullet(monst_t *m, float ang, struct ivalu_t *w );
	void drop_spore(monst_t *m, struct ivalu_t *w );
	void explode(float x, float y, float r, float inten,  monst_t *who_fired = NULL, monst_t *ignore = NULL, int flags = 0);
	monst_t *jump_in(monst_t *monst);
	monst_t *jump_away(monst_t *monst);
	int  poll();
	void draw();
	void encl_draw(int ssl, int sst, int ssr, int ssb, BITMAP *floor_bmp, BITMAP *cage_bmp);
	void clear();
	void save(PACKFILE *pf, int flags);
	int  parse(char *cmd, char *next, int line);
};

#endif
