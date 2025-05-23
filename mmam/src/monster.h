#ifndef _monster_h
#define _monster_h 1

#define ACT_CHILL  0
#define ACT_WALK   1
#define ACT_EAT    2
#define ACT_ATTACK 3

#define ACT_SHOOT  4
#define ACT_THROW  5
#define ACT_BUILD  6
#define ACT_KO     7
#define ACT_DIE    8

#define AF_EGGS         0x0001
#define AF_HERDS        0x0002
#define AF_DIGS         0x0004
#define AF_ASEXUAL      0x0008
#define AF_CLOAKING     0x0010
#define AF_FLYING       0x0020
#define AF_BLOODSUCKER  0x0040
#define AF_PSYCHOSTARVER 0x0080
#define AF_NOHOOK       0x0100
#define AF_SPARKS       0x0200
#define AF_BESERK       0x1000
#define AF_BOMB         0x2000
#define AF_SEECLOAKING  0x4000
#define AF_HUNGERLESS   0x8000
#define AF_NOSHOOT     0x10000
#define AF_WANDERS     0x20000
#define AF_INORGANIC   0x40000
#define AF_AGRLIST     0x80000
#define AF_BIGMONST   0x100000
#define AF_ADVTARGET  0x200000
#define AF_POWGRID    0x400000
#define AF_PARANOID   0x800000
#define AF_FAMILY    0x1000000

#define QWERK_LAZY_MASK 0x00ff
#define QWERK_DRUGS     0x0100
#define QWERK_PACIFIST  0x0200
#define QWERK_VIOLENT   0x0400
#define QWERK_POLITICAL 0x0800
#define QWERK_LAWFUL    0x1000
#define QWERK_LOYAL     0x2000
#define QWERK_ADHD      0x4000
#define QWERK_VENGEFUL  0x8000

#define TARGET_AST0    0x1
#define TARGET_HUNTER  0x2


typedef enum award {	
	award_0et, award_0er, award_1et, award_1er, award_2et, award_2er, award_3et, award_3er,
	award_0at, award_0ar, award_1at, award_1ar, award_2at, award_2ar, award_3at, award_3ar, 
	award_0r, award_1r, award_2r, award_3r,	
	award_0race, award_1race, award_2race, award_3race, 
	award_0rm, award_1rm, award_2rm, award_3rm, 
	award_0eat, award_1eat, award_2eat, award_3eat, 
	award_0spit, award_1spit, award_2spit, award_3spit,
	award_dual, award_dual_lost, award_tdual, award_tdual_lost,
	award_last, award_none = -1 
};

#define NUM_AWARD_TYPES award_last


extern char *award_desc[award_last];
extern int  award_metal_idx[award_last];


typedef enum emotion {
  emot_none, emot_happy, emot_halo, emot_pissed, emot_confused, emot_sick, emot_hungry,
  emot_bored, emot_surprized, emot_sleep, emot_dead, emot_tired, emot_eating, emot_inject,
  emot_nopower, emot_off
};

typedef enum building_t {
	bld_none, bld_base, bld_jmart, bld_bfc, bld_arena
};

typedef enum ai_type {
	ai_player, ai_plant, ai_animal, ai_building, ai_item, ai_hunter, ai_crysillian,
	ai_trainer, ai_merchant, ai_tourist, ai_dragoonigon, ai_beholder,
	ai_turret, ai_soldier, ai_krulasite, ai_meeker, ai_ackatoo, ai_stalker,
	ai_cheech, ai_hoveler, ai_tank, ai_slymer, ai_meteor, ai_worker
};

typedef enum noise_type {
	snd_bark, snd_bark_freq, snd_die, snd_pain, snd_freek, snd_eat
};

#define snd_die_starving snd_die
#define snd_die_od       snd_die
#define snd_die_burn     snd_die
#define snd_sex          snd_pain


typedef struct smack_t { class monst_t *who; int when, hp; };

typedef struct monst_base {
	char *name, *subtype, *desc;
	ai_type ai_func;
	int mhp, max_age, mature_age, speed, lat_speed, turn_rate, str, attack_rate, stomach, size, met_rate,
	    nutrish, max_energy, od_thresh, gestation, agression, flags;
	int *blood_color;
	int fear_dist;
	float fear_factor;
	int herd_size, fame, bfc_price, combat_price, common;
	monst_base **likes_to_eat;
	monst_base **will_eat;
	monst_base **hates;
	monst_base **scared_of;
	int *spr_big;
	aspath *spr_chill;
	int *walk_span;
	int *soundtab;
	aspath *spr_walk, *spr_eat, *spr_attack, *spr_die;
	struct ivalu_t* inv_link; int monster_idx, stat_caught,
		stat_inpark, stat_killed, stat_didscan, stat_won_medals,
		stat_valid_target, org_idx;
} monst_base;

extern monst_base mb_psychoshroom, mb_poisonshroom, mb_pyroshroom;

typedef class monst_t {
   private:
	int frame_ms, is_starving, want_emote;
	float traveled;
	int hit_ex(int dam, monst_t *who, int die_noise, int pain_noise);
	void calc_can_attack_next();
	void issue_weaps(int mutate); // in inventory.cc
	void forget_monst(monst_t *monst);
	void decay_bonus(float &f);
	void mutate(monst_base *m, int time = 0);
	int  _krulasite_inf_ai();
	int  _fire_run_ai();
	int  _slap_and_tickle_ai(); 
	int _build_home_ai(int prefers_inside);
	void fear_hunger_pick();
	void fear_hunger_pick2();
	int  chase(monst_t *who, float speed = 1., float back_speed = 0.05, float r1 = 0., float r2  = 0. );
	void evade(monst_t *who);
	void wander(float speed = 0.3, float clamp_tr = 500.);
	int  can_hide_in(monst_t *hole);
	int  hide_in_hole(monst_t *hole);
	void climb_out_hole();
	// AI funcs
	void player_ai();     /* in mmam.cc */
	void hunter_ai();     /* in mmam_ai.cc */
	void animal_ai();     /* in mmam_ai.cc */
	void plant_ai();      /* in mmam_ai.cc */
	void building_ai();   /* in mmam_ai.cc */
	void crysillian_ai(); /* in mmam_ai.cc */
	void trainer_ai();    /* in mmam_ai.cc */
	void merchant_ai();   /* in mmam_ai.cc */
	void tourist_ai();    /* in mmam_ai.cc */
	void item_ai();       /* in mmam_ai.cc */
	void dragoonigon_ai(); /* in mmam_ai.cc */
	void beholder_ai();    /* in mmam_ai.cc */
	void turret_ai();      /* in mmam_ai.cc */
	void soldier_ai();
	void meeker_ai();
	void ackatoo_ai();
	void stalker_ai();
	void krulasite_ai();
	void cheech_ai();
	void hoveler_ai();
	void tank_ai();
	void slymer_ai();
	void meteor_ai();
	void worker_ai();
	
	int  huntraid_friendly(monst_t *m);
   public:
   // begin read only
   	ai_type ai_func;
   	int action_idx, action_frame, org_idx, vis_count, sex, 
		decay_time, emote_icon, emote_count, inject_idx, time_thaw,
		time_born, time_die, time_mature, time_captured,
		time_done_pregnant, jump_out_time, horny_time,
		hp, mhp, attack_time, fear_dist, nutrish, ast_from, aun, weap_rounds,
		name_xp, name_xp_last;
	float speed, lat_speed, turn_rate, in_stomach, energy, growth_rate;
	int   str, attack_rate, stomach, size, met_rate,
		max_nutrish, max_energy, od_thresh, gestation, agression, fame_bonus, flags;
	int on, pow_drain, pow_on, building_squads, in_a_hole, on_fire;
	float speed_bonus, energy_bonus, str_bonus, met_rate_bonus,
		healing_bonus, agression_bonus, growth_bonus, restlessness,
		invisability_bonus, drugged;
	int krulasite_inf, spore_pyro_count, spore_psycho_count, spore_poison_count, time_bark;
	list <struct weap_t *> weap;
	list <struct smack_t *> smacked;
	smack_t last_smack;
	struct monst_base **extra_hates;
	struct monst_base **extra_scared_of;
	char *name;
	struct monst_base *base;
	char *worker_history, *worker_rapsheet, *worker_lastboss;
	int worker_salary, worker_skill; 
	float x, y, radius, vx,vy, ax,ay;
	float ang;
	int mutate_time, revert_time; monst_t *mutate_to; float mutate_growth;
	asteroid_t *encl;
	monst_t *hole_monst;
	int award[NUM_AWARD_TYPES];// fix later
	// end read only
	int hunger_level;
	float run_x, run_y, wang;
	int aiu1, aiu2, aiu3, target_ang[4];
	class asteroid_t *my_ast;
	class trader_t *trader_ui;
	monst_t *who_hit_me_last, *see_monst, *see_hole, *see_buddy, *chase_last, *home_hole;
	monst_t *momma, *daddy, *spouse, *baby;
	PMASK mask;
	// std funcs
	monst_t(monst_base *m, int grown = 1);
	~monst_t();
	void poll();
	void caged_poll(class cage_t *jail);
	void draw();
	// utility funcs
	void rename(char *s);
	void forget(monst_t *monst);
	void forget_all();
	float chubby_dist(monst_t *who);
	float mdist(monst_t *who);
	int   shot_lead(monst_t *who, float &shang);
	int check_mb_list(struct monst_base **base_list);
	int check_mb_list_better(struct monst_base **base_list, monst_base *better);
	int check_mb_list_worse(struct monst_base **base_list, monst_base *worse);
	// feedback funcs
	void feel(emotion e);
	void sqwak(int noise_idx);
	void do_act(int action);
	// action funcs
	int  hit(int dam, monst_t *who = NULL, int die_noise = snd_die, int pain_noise = snd_pain);
	int  hit(int dam, int die_noise, int pain_noise);
	int  stress(float stress_factor);
	int  attack(monst_t *who);
	int  eat(monst_t *who);
	int  inject(int inject_idx, int flags =0);
	int  try_to_give_birth();
	int  adopt_new_parent(monst_t *parent);
	int  feed(int food_idx);
	int  eat_affect(monst_t *eatm);
	void walk(float tang, float exp, float lat_exp);
	void open_interface() { if (!trader && trader_ui) {trader_ui->scroll_in(); } };
	void fix_radius() { radius = (int)(sqrt(size)/2 + 5); }
	void building_upgrade();
	// query funcs
	char *worker_class(int shortver = 0);
	int sort_height();
	struct BITMAP *emote_bitmap();
	int  hates(monst_t *who);
	int  scared_of(monst_t *who);	
	int  will_eat(monst_t *who);
	int  likes_to_eat(monst_t *who);
	int  would_eat(monst_t *who);
	int  will_eat(monst_base *what);
	int  likes_to_eat(monst_base *what);
	int  would_eat(monst_base *what);
	int  isa_plant()  { return ai_func == ai_plant; }
	int  isa_building()  { return ai_func == ai_building; }
	int  isa_item()  { return ai_func == ai_item; }
	int  isa_weapon()  { return base->inv_link && base->inv_link->typ == inv_proj; }
	int  isa_shroom() { return (base == &mb_psychoshroom || base == &mb_poisonshroom || base == &mb_pyroshroom); }
	int  inanimate() { return ( isa_building() || isa_plant() || isa_item());}
	float eff_speed() { return speed + speed_bonus / 20. / TURNS_PER_SEC; }
	float eff_lat_speed() { return lat_speed + speed_bonus / 40. / TURNS_PER_SEC;}
	float eff_turn_rate() {return turn_rate + speed_bonus / 20. / TURNS_PER_SEC;}
	float eff_energy() { return energy + energy_bonus; }
	char *He();
	char *he();
	char *him();
	char *His();
	char *his();
	char *a_name(char *scrap, int long_worker = 0);
	char *A_name(char *scrap, int long_worker = 0);
	int  dead() { return (hp <= 0); };
	int  alive() { return (hp > 0); };
	int  tired() { return (eff_energy() < 100.); };
	int  asleep() { return (eff_energy() <= 0.); };
	int  invisible() { return (flags & AF_CLOAKING); };
	int  flammable();
	int  can_poison();
	int  can_shroom_poison();
	int  can_sleep();
	int  can_pickup_spores();
	struct weap_t *armed() { return (weap.empty() ? NULL : weap.front()); }
	int  fire_at(monst_t *who);
	int  fire_at(float at_x, float at_y) { return fire(atan2(at_y - y, at_x - x)); };
	int  fire(float ang);
	int  use_item() {return fire(0.);};
	void give_weap(struct ivalu_t *t, int n = -1, int rl = 0);
	int  orick_clan() { return MID(0, ast_from / 200, 4); }
	int  drop_weap(int force = 0);
	int  take_weap(monst_t *m, struct builder_table *bt = NULL);
	void toggle_weap(int dir) {
        	if (weap.empty()) return;
        	if (dir > 0) for (;dir != 0; dir--) {
        		weap_t *w = weap.front(); weap.pop_front();weap.push_back(w);
		} else for (;dir != 0; dir++) {
        		weap_t *w = weap.back();  weap.pop_back();weap.push_front(w);
    		}
	};
	int  krulasite_sym() {return (krulasite_inf != 0 && (krulasite_inf - ms_count < (1000 * 180)));}	
	void cryo_advance(int t) {
		int d = ms_count - t;
		time_thaw = ms_count + 15000;  if (decay_time) decay_time += d;   time_die += d;
            	time_mature += d;  if (jump_out_time) jump_out_time += d;   if (krulasite_inf) krulasite_inf += d;
            	time_captured += d;
	};
	int buddy_wants_to_get_laid();
	void reset_horny_time();
	
	void save(PACKFILE *pf, int flags);
	int  parse(char *cmd, char *next, int line);
};

#endif
