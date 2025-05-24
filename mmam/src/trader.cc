#include "mmam.h"
#include "rand.h"
#include "bubbles.h"
#include "graphs.h"
#include "namelist.h"
#include <ctype.h>
#ifdef USE_ALOGG 
extern "C" {
#include <alogg/aloggint.h>
}
#include <alogg/alogg.h>
#endif	
				

#define MENU_BUY    (MENU_SUB0 + 1)
#define MENU_SELL   (MENU_SUB0 + 2)
#define MENU_PARK   (MENU_SUB0 + 3)
#define MENU_FIGHT  (MENU_SUB0 + 4)
#define MENU_CRYO   (MENU_SUB0 + 5)
#define MENU_ALIGNMENT (MENU_SUB0 + 6)
#define MENU_LAB    (MENU_SUB0 + 7)
#define MENU_JUMP   (MENU_SUB0 + 8)
#define MENU_TURRET (MENU_SUB0 + 9)
#define MENU_ENCLMAIN (MENU_SUB0 + 10)
#define MENU_ENCLTAKE (MENU_SUB0 + 11)
#define MENU_ENCLPUT  (MENU_SUB0 + 12)
#define MENU_POWER  (MENU_SUB0 + 13)
#define MENU_SCAN   (MENU_SUB0 + 14)
#define MENU_GRAPHS   (MENU_SUB0 + 15)
#define MENU_HIRE   (MENU_SUB0 + 16)
#define MENU_PAYROLL   (MENU_SUB0 + 17)
#define MENU_WORKER   (MENU_SUB0 + 18)
#define MENU_HUNTING  (MENU_SUB0 + 19)
#define MENU_CRITTERS   (MENU_SUB0 + 20)

#define MENU_MORE   (MENU_EXTRA0 + 99)

#define MENU_TICKET_UP (MENU_EXTRA0 + 1)
#define MENU_TICKET_DN (MENU_EXTRA0 + 2)
#define MENU_HUNT_OK (MENU_EXTRA0 + 3)
#define MENU_HUNT_UP (MENU_EXTRA0 + 4)
#define MENU_HUNT_DN (MENU_EXTRA0 + 5)
#define MENU_CHECK_REFS (MENU_EXTRA0 + 6)
#define MENU_CHECK_BACKGROUND (MENU_EXTRA0 + 7)
#define MENU_DRUG_TEST (MENU_EXTRA0 + 8)
#define MENU_DO_HIRE (MENU_EXTRA0 + 9)
#define MENU_DO_BATTLE (MENU_EXTRA0 + 9)
#define MENU_DISMIS (MENU_EXTRA0 + 10)
#define MENU_WORKER_NEXT (MENU_EXTRA0 + 10)
#define MENU_WORKER_PREV (MENU_EXTRA0 + 11)
#define MENU_WORKER_PAYUP (MENU_EXTRA0 + 12)
#define MENU_WORKER_PAYDN (MENU_EXTRA0 + 13)
#define MENU_WORKER_GIVE_WEAP (MENU_EXTRA0 + 14)
#define MENU_WORKER_TRAIN (MENU_EXTRA0 + 15)
#define MENU_WORKER_FIRE (MENU_EXTRA0 + 16)


#define SHOW_ITEMS 10

extern char *major_hints[];
extern char *minor_hints[];
extern char *pr_chatter[];
extern char *pr_goodbye[];
extern char *workerK_chatter[];
extern char *workerK_goodbye[];
extern char *workerM_chatter[];
extern char *workerM_goodbye[];
extern char *workerR_chatter[];
extern char *workerR_goodbye[];
extern char *workerSm_chatter[];
extern char *workerSm_goodbye[];
extern char *workerSf_chatter[];
extern char *workerSf_goodbye[];
extern char *workerCm_chatter[];
extern char *workerCm_goodbye[];
extern char *workerCf_chatter[];
extern char *workerCf_goodbye[];
extern char *workerV_chatter[];
extern char *workerV_goodbye[];

#define SND_GIB_BFC -1
#define SND_GIB_POLICE -1
#define SND_GIB_FISEC -1
#define SND_GIB_ORICK00 -1
#define SND_GIB_TRADE -1

static int npc_voice = 0;
static int gibberish_freq[SND_COUNT];	

int worker_skill_upgrade_cost[6] = {
0,0, 100, 200, 500, 1000
};

trader_t *sell_out;
vector <class inven_t *> applicants;

class battle_desc *active_battle;
class battle_desc {
public:
	char *short_desc, *long_desc;
	int player_cage;
	int style,  monsters;
	int prize[4], medal[4], fame[4];
	int entryfee;
	int time_limit;
	int glyph;
	monst_base **mbl;
    int *mblweight;
    int num_mblweight;
    int mblweight_pounds;
    
	// qualifier, toughness, 
    asteroid_t *battle_zone;
    monst_t *save_ship;
    trader_t *save_trader;
    int save_ai;
    char results[1024];
    
    int last_place;
    monst_t *place[32];
	battle_desc() {
		battle_zone = NULL;
    	save_ship = NULL;
    	mbl = NULL;
    	mblweight = NULL;
    	results[0] = 0;  	
		glyph = 1;
		for (int i = 0; i < 4; i++) {
			medal[i] = -1; prize[i] = fame[i] = 0;
		}
	};
	
	void start_battle() {
		if (battle_zone) return;
		save_trader = trader; save_ship = ship;
		save_ai = hud->jail[player_cage]->m->ai_func; 
		ship = hud->jail[player_cage]->m;
		ship->ai_func = ai_player;
		ship->action_idx = ACT_WALK;		
		// ship->flags |= AF_BESERK;
		hud->jail[player_cage]->m = NULL;
		battle_zone = new asteroid_t(0);
		battle_zone->clear();
		// grid is alloc'd larger but who cares.
		battle_zone->pw = battle_zone->ph = 800; 
		battle_zone->skin_num = 7;		
		battle_zone->add_monst(ship);
		battle_zone->exit_time = ms_count + time_limit;
		battle_zone->start_time = ms_count + 3000;
		battle_zone->arena_glyph = glyph;
		for (int c = 0; c < monsters - 1; c++) {
			int pick = irand(mblweight_pounds);
			int pix = 0;int tw = 0;
			for (; pix < num_mblweight-1; pix++) {
				tw += mblweight[pix];
				if (tw > pick) break;
			} 
			monst_t *mm = battle_zone->spawn_monst(mbl[pix]);
			// if (mm) mm->flags |= AF_BESERK;
		}
		ast = battle_zone;
		if (trader) trader->scroll_out();
		active_battle = this;
		last_place = monsters - 1;
	}
	int continue_battle() {
		return 0;
	}
	void end_battle() {
		int pix = -1;
		if (place[0] == ship) pix = 0;
		if (place[1] == ship) pix = 1;
		if (place[2] == ship) pix = 2;
		if (place[3] == ship) pix = 3;
		char *ee = results;
			
		if (pix >= 0 ) {
			hud->money += prize[pix];
			ship->fame_bonus = MAX(fame[pix],MIN(ship->fame_bonus + fame[pix] / 20, 3 * fame[pix] / 2 ));
			if (medal[pix] >= 0) ship->award[medal[pix]]++;
			static char *placestr[4] = {
				(char *)"WON!", (char *)"took second place.", 
				(char *)"got third place.", (char *)"came in fourth place."
			};
			ee += sprintf(ee, "Your %s %s. ", ship->name, placestr[pix]);
			if (prize[pix]) ee += sprintf(ee, "%s performance earned you $%i. ", ship->His(), prize[pix]);
					
		} else ee += sprintf(results, "Your %s got stomped.  ", ship->name);		 
		if (ship->hp <= 0) {
			if (irand(3) != 0 || monsters == 2) {
				ship->hp = ship->mhp / 4;
				ee += sprintf(ee, "Lucky for %s our medics got to %s in time, "
					"but %s's going to need pleanty of rest before fighting again. ", 
					ship->him(), ship->him(), ship->he()
				);			
			} else if (pix < 0) {
			} else ee += sprintf(ee, "Unfortunately %s wounds were too severe, and the medical "
				"staff wasn't able to do anything for %s.  Rest in peace %s. ", 
				ship->his(), ship->him(), ship->name
			);		
		} else if (ship->hp < ship->mhp / 4) {
			ship->hp = ship->mhp / 4;
			ee += sprintf(ee, "%s's pretty beat up, so "
				"%s's going to need pleanty of rest before fighting again. ", 
				ship->He(), ship->he()
			);		
		}
		if (monsters == 2) {
			// the monster trainer is a bit more polite
			ee = results;
			if (pix == 0) ee += sprintf(ee, "Congratulations on your win. ");
			else ee += sprintf(ee, "Your %s put up a good fight. Next time %s will probably win. ", ship->name, ship->he());			
			if (ship->name_xp > 1) ship->name_xp = ship->name_xp_last;
		}
		int xp = ship->name_xp;
		int name_lvl = (xp <= 0 ? 0 : xp <=  2 ? 1 : xp <= 7 ? 3 : MAX(3 + xp / 15, 9));
		xp = ship->name_xp_last;
		int last_lvl = (xp <= 0 ? 0 : xp <=  2 ? 1 : xp <= 7 ? 3 : MAX(3 + xp / 15, 9));
		if (ship->alive() && name_lvl != last_lvl) {
			char namescrap[512]; strcpy(namescrap, ship->name);
			static monst_base *use_human_names[] = {
				&mb_worker, &mb_raider, &mb_hunter, &mb_sentry, &mb_sentinel, NULL
			};
			char fn[256];
			name_gen ng;
			if (ship->check_mb_list(use_human_names)) {
				sprintf(fn,"%s/custom/humannames.conf", home_dir);
				ng.load(fn); // load returns true immediately once data is loaded 
				sprintf(fn,"%s/baseconf.dat#HUMANNAMES_CONF", data_dir);
				if (ng.load(fn)) {
					ng.get_fullname(namescrap);
				}
				ng.kill();
				last_lvl++;			
			}
			sprintf(fn,"%s/custom/warriorname.conf", home_dir);
			ng.load(fn, ship->base->name);  
			sprintf(fn,"%s/baseconf.dat#WARRIORNAME_CONF", data_dir);
			if (ng.load(fn)) for (int lu = last_lvl + 1; lu <= name_lvl; lu++) {
				ng.level_up_name(namescrap, lu, ship->sex);	
			}
			ng.kill();
			free(ship->name);
			ship->name = strdup(namescrap);
			ee += sprintf(ee, "%s is now known as \"%s\" to %s fans. ", ship->He(), ship->name, ship->his());
		}
		if (ship->alive()) {
			//ship->flags &=~ AF_BESERK;
			ship->action_idx = ACT_WALK; // avoids silly ko flying		
			ship->my_ast->unhook_monst(ship);
			ship->drugged = MIN(ship->drugged, MAX(ship->od_thresh - 1, 0));
			ship->name_xp_last = ship->name_xp;		
			hud->jail[player_cage]->m = ship;
		}
		ship = save_ship;
		ast = ship->my_ast;
		save_trader->scroll_in();
	}
	int check_fight() {
		
		for (list<monst_t *>::iterator mon = ast->m.begin(); mon != ast->m.end(); ++mon) {
			if ((*mon)->dead()  && (*mon)->ai_func != ai_item && (*mon)->ai_func != ai_plant) {
				(*mon)->ai_func = ai_item;
				place[last_place] = *mon;					
				(*mon)->decay_time = 0;
				last_place--;				
			}
		}
		if (last_place == 0) for (list<monst_t *>::iterator mon = ast->m.begin(); mon != ast->m.end(); ++mon) {
			if ((*mon)->alive()  && (*mon)->ai_func != ai_item && (*mon)->ai_func != ai_plant) {
				(*mon)->ai_func = ai_item;
				place[last_place] = *mon;					
				last_place--;				
			}
		}
		if (ms_count > ast->exit_time) { // later: make this fair. 
			for (list<monst_t *>::iterator mon = ast->m.begin(); mon != ast->m.end(); ++mon) {
				if ((*mon)->alive()  && (*mon)->ai_func != ai_item && (*mon)->ai_func != ai_plant) {
					(*mon)->ai_func = ai_item;
					place[last_place] = *mon;					
					last_place--;				
				}
			}
		}
		
		if (last_place < 0) {
			end_battle();	
		}
		
		return 0; // 0 = running   1 = break between rounds   2 = fight done 
	}
	void set(int cage, char *sd, char *ld, int sty, int mon, int fee, int tl, monst_base **smbl,
    int *smblweight
    ) {
		player_cage = cage;
		short_desc = sd;
		long_desc = ld;
		style = sty;
		monsters = mon;
		time_limit = tl;
		entryfee = fee;
		mbl = smbl;
    	mblweight = smblweight;
    	mblweight_pounds = 0;
    	for (num_mblweight = 0; mblweight[num_mblweight] >= 0; ++num_mblweight ) {
    		mblweight_pounds += mblweight[num_mblweight];
    	}    		
	}
	void set_mpf(int ix, int m, int p, int f) {
		medal[ix] = m; prize[ix] = p; fame[ix] = f;
	}
};

static vector <battle_desc *> battle_list;

void check_minigame() {
	if (active_battle) {
		active_battle->check_fight();
	}
}


inven_t::inven_t(int i, int n) { 
	cage = NULL; w = NULL; m = NULL; ivalu = i; count = n; plus = 0; pointer_is_mine = 0; price = 0;
}
inven_t::inven_t(struct weap_t *wp, int p) {
        int nr = MAX(1, wp->t->grp);
	cage = NULL; w = wp; m = NULL; ivalu = -1; count = 1; pointer_is_mine = p;price = 0;
	count = wp->count / nr;
	plus = wp->count % nr;
}
inven_t::inven_t(class monst_t *mon, int p) { cage = NULL; w = NULL; m = mon; ivalu = -1; count = 1; plus = 0; pointer_is_mine = p;price = 0;}
inven_t::inven_t(class cage_t *c, int p) { cage = c; w = NULL; m = NULL; ivalu = -1; count = 1; plus = 0; pointer_is_mine = p;price = 0;}
inven_t::~inven_t() { if (pointer_is_mine) { delete w; delete m; } }
ivalu_t *inven_t::inv() { return (m ? NULL : cage ? &ivalu_list[cage->lvl + FIRST_INV_CAGE_IDX] : w ? w->t : &ivalu_list[ivalu]); }
char *inven_t::desc()  { return ( m ? m->base->desc : inv()->desc); }
char *inven_t::name() { return ( m ? m->name : inv()->nam); }
int inven_t::lvl() { return (m ? -1 : inv()->lvl); }
int inven_t::rnd() { return ( ivalu >= 0 && ivalu_list[ivalu].typ == inv_proj ? ivalu_list[ivalu].grp : w ? ( count ? w->t->grp : w->count ): -1 ); }
int inven_t::group_buy() {
	if (ivalu) switch (ivalu_list[ivalu].typ) {
        case inv_food:
        case inv_injector:
        case inv_mineral: return 1;
   	}
	return 0;
}


trader_t::trader_t(int idx, monst_t *attach_to) {
	link = attach_to;
	watch_monst = NULL;
	building_idx = idx;
	top_buy_item = focus = jg_respawn_time = jg_first = 0;
	bs_grp = 1;
	scroll_dir = 0; time_scroll = 0;
	kmenu = (BITMAP *)sprites[SPR_KMENU].dat;
	ox = -kmenu->w;
	geararm_x = geararm_y = 0;
	price_mult = 1.0;
	raw_puppet = NULL;   raw_puppet_idx = 0;
	said = NULL;
	enable_fight = enable_buy = enable_sell = enable_park = enable_cryo = enable_lab = 0;
	enable_jump = enable_turret = enable_encl = enable_power = enable_scan = enable_graphs = 0;
	enable_hire = enable_worker = 0;
	picture_layout_idx = 0;
	gibberish_nc = 1;
	inventory.clear();
	if (link->base == &mb_astbase) {
		picture_idx = SPR_PRREP00;
		gibberish_idx = SND_GIB_PRREP;
		enable_park = 1;
		menu_num =  MENU_PARK;
		price_mult = 1.5;		
	} else if (link->base == &mb_worker) {
		picture_layout_idx = 1;	
		if (link->sex == 2) {
			// picture_idx = SPR_SENTRY;
			picture_idx = SPR_WORKERF;
			gibberish_idx = SND_GIB_JMART00 + irand(4);
		} else {
			// picture_idx = SPR_SENTINEL;
			picture_idx = SPR_WORKERM;
			gibberish_idx = SND_GIB_PITGUY;
		}
		enable_worker = 1;	
		menu_num =  MENU_WORKER;
	} else if (link->base == &mb_jmart) {
		picture_idx = SPR_JMARTGUY00;
		gibberish_idx = SND_GIB_JMART00;
		gibberish_nc = 4;
		enable_buy = 1;
		menu_num =  MENU_BUY;
	} else if (link->base == &mb_bfc) {
		picture_idx = SPR_BFCGUY00;
		gibberish_idx = SND_GIB_BFC;
		enable_sell = enable_buy = 1;
		menu_num =  MENU_BUY;
		price_mult = 1.;		
	} else if (link->base == &mb_wisehut) {
		picture_idx = SPR_WISEOLDGUY00;
		gibberish_idx = SND_GIB_WIZE;
		enable_buy = 1;
		menu_num = MENU_BUY;
	} else if (link->base == &mb_arena) {
		picture_idx = SPR_PITGUY00;
		gibberish_idx = SND_GIB_PITGUY;
		enable_fight = 1;
		enable_buy = 1;
		menu_num = MENU_FIGHT;
		price_mult = 1.5;		
	} else if (link->base == &mb_trainer) {
		picture_idx = SPR_TRAINERGUY00;
		gibberish_idx = SND_GIB_TRAINER;
		enable_fight = enable_buy = enable_sell = 1;
		menu_num = MENU_FIGHT;
		price_mult = 2.0;
	} else if (link->base == &mb_policestation) {
		picture_idx = SPR_POLICE00;
		gibberish_idx = SND_GIB_POLICE;
		enable_buy = 1;
		menu_num = MENU_BUY;
	} else if (link->base == &mb_fisec) {
		picture_idx = SPR_FISECGUY00;
		gibberish_idx = SND_GIB_FISEC;
		enable_buy = 1;
		menu_num = MENU_BUY;
		price_mult = 1.25;
	} else if (link->base == &mb_orickcamp) {
		picture_idx = SPR_ORICK00;
		gibberish_idx = SND_GIB_ORICK00;
		gibberish_nc = 6;
		enable_sell = 1;
		menu_num = MENU_SELL;
	} else if (link->base == &mb_merchant) {
		picture_idx = SPR_MERCHANT00;
		gibberish_idx = SND_GIB_TRADE;
		building_idx = bld_none;
		enable_buy = 1;
		enable_sell = irand(2);
		menu_num =  MENU_BUY;
		double f = frand() * 14.;
		double fip; 
		double lp = modf(f, &fip);
		int tix = MID(0, (int)(fip), 13);
		double mcurve[15] = { // simi bell shaped curve.
			1.20, 1.50, 1.80, 2.00, 2.10, 2.20, 2.40, 2.50,
			2.65, 3.00, 3.25, 3.40, 4.20, 5.00, 6.00
		};
		price_mult = mcurve[tix] + lp * (mcurve[tix + 1] - mcurve[tix]);   
	} else if (link->base != &mb_radiomarker && link->base->inv_link && link->base->inv_link->typ == inv_scanner) {
		picture_idx = SPR_PRREP00;
		gibberish_idx = SND_GIB_PRREP;
		enable_scan = 0;
		menu_num = MENU_SCAN;
	} else if (
		link->base == &mb_guntower || link->base == &mb_dgtower ||
		link->base == &mb_xcannontower || link->base == &mb_galtixtower
	) {
		picture_idx = SPR_PRREP00;
		gibberish_idx = SND_GIB_PRREP;
		enable_turret = 1;
		menu_num =  MENU_TURRET;
	} else if (link->base->inv_link && link->base->inv_link->typ == inv_encl) {
		picture_idx = SPR_PRREP00;
		gibberish_idx = SND_GIB_PRREP;
		enable_encl = 1;
		menu_num =  MENU_ENCLMAIN;
	} else if (
		link->base == &mb_jumpgatenode           || link->base == &mb_jumpgatestabilizer ||
		link->base == &mb_transphasitestabilizer || link->base == &mb_multiphasitestabilizer ||
		link->base == &mb_omniphasitestabilizer
	) {
		picture_idx = SPR_PRREP00;
		gibberish_idx = SND_GIB_PRREP;          	
		enable_jump = 1;
		jg_autojump = 1;
		menu_num =  MENU_JUMP;
	} else if (link->base == &mb_solararray || link->base == &mb_fissionplant || link->base == &mb_fusionplant) {
		picture_idx = SPR_PRREP00;
		gibberish_idx = SND_GIB_PRREP;          	
		enable_power = 1;
		menu_num =  MENU_POWER;
	} else {
		link = NULL;
		return;
   	}
   	// set up the shop inventory for this trader.
   	if (link->base == &mb_bfc) {
   		add_item(FIRST_INV_FOOD_IDX + 10, -1); // chicken
   	}
	for (int i = 0; i < NUM_IVALU_LIST; i++) {
		if (
			link->base == &mb_astbase && arg_buyany &&
			(ivalu_list[i].jm || ivalu_list[i].common) ? 1 :
			link->base == &mb_policestation ? i == FIRST_INV_DONATION_IDX :
			link->base == &mb_wisehut  ? ivalu_list[i].typ == inv_hint :
			// link->base == &mb_bfc ? (i >= FIRST_INV_FOOD_IDX && i < FIRST_INV_FOOD_IDX + 4) :
			link->base == &mb_jmart && how_aligned(jmart_corp) > -2 ? ivalu_list[i].jm & 1 :
			link->base == &mb_astbase ? ivalu_list[i].jm & 2  :
			link->base == &mb_arena  ? ivalu_list[i].jm & 4 :
			link->base == &mb_fisec && link->hp > link->mhp / 4 ? ivalu_list[i].jm & 8 :
			link->base == &mb_merchant ? enable_buy && ivalu_list[i].common > irand(100) : 0
		) {
			int c = ivalu_list[i].common;
			int n = (
				link->base != &mb_merchant ? -1 :
				c > 50 ? 5 + irand(50) :
				c > 30 ? 4 + irand(10) :
							c > 20 ? 1 + irand(10) :
							c > 10 ? 1 + irand(4) : 1
			);
			add_item(i,n);
		}
  	}
}


void trader_t::add_item(int i, int n, monst_t *sell_to) {
	if (n == 0) return;
	if (n >= 0) for (vector <inven_t *>::iterator inv = inventory.begin(); inv != inventory.end(); inv++) {
                if ((*inv)->w && ((*inv)->w->t == &ivalu_list[i]) ) {
                	(*inv)->count += n;
                	(*inv)->w->count += n * ((*inv)->w->t->grp); //???
              	} else if ((*inv)->ivalu == i) {
                        if ((*inv)->count >= 0) (*inv)->count += n;
                	return;
           	}
   	}
   	inven_t *itm = new inven_t(i,n);
        itm->price = (int)(sell_to == NULL ? floor(price_mult * itm->inv()->cost) : itm->inv()->cost / 3);
	inventory.push_back(itm);
}


void trader_t::add_monst(monst_t *monst, monst_t *sell_to) {
	inven_t *i = new inven_t(monst, (sell_to == NULL));
	i->price = (int)(
        	sell_to && sell_to->base == &mb_bfc ? monst->base->bfc_price :
        	monst->base->combat_price * (sell_to != NULL ? 0.333 : price_mult )
	);
	inventory.push_back(i);
	monst->my_ast = NULL; monst->forget_all();
}


void trader_t::add_monst(monst_base *mb, monst_t *sell_to) { add_monst(new monst_t(mb), sell_to); }

void trader_t::generate_applicant() {
    monst_t *monst = new monst_t(&mb_worker);
	inven_t *i = new inven_t(monst, 0);
	i->price = (int)(monst->worker_salary);
	applicants.push_back(i);
	monst->my_ast = NULL; monst->forget_all();
}

void trader_t::add_weap(weap_t *wp, monst_t *sell_to) {
	if (!wp) return;
	inven_t *i = new inven_t(wp, (sell_to == NULL));
	float mnr = MAX(1, wp->t->grp);
	i->price = (int)((i->plus ? i->plus / mnr : 1.) * wp->t->cost * (sell_to == NULL ? price_mult : sell_to->base == &mb_orickcamp ? 0.5 :  0.333));
	inventory.push_back(i);
}

void trader_t::add_cage(cage_t *c, monst_t *sell_to) {
    if (!c) return;
	inven_t *i = new inven_t(c, (sell_to == NULL));
    i->price = (int)(sell_to != NULL ? floor(price_mult * i->inv()->cost) : i->inv()->cost / 3);
	inventory.push_back(i);
}

extern monst_base* orick_collect[];

void trader_t::make_sell_list(monst_t *monst, int mode) {
        if (!monst) return;
        int ti = 0, fo = 0;
        if (sell_out) {
                ti = sell_out->top_buy_item;
                fo = sell_out->focus;
		delete sell_out;
       	}
        sell_out = new trader_t(0, ship);
        if (link->base == &mb_trainer || link->base == &mb_merchant) for (int c = 0; c < 10; c++) if (hud->injector[c]) {
        	sell_out->add_item(c + FIRST_INV_INJECTOR_IDX, hud->injector[c]);
	}
        for (list <weap_t *>::iterator wp = ship->weap.begin(); wp != ship->weap.end(); wp++) {
        	int ok = (monst->base == &mb_merchant);
        	if (monst->base == &mb_orickcamp) {
                	for (int c = 0; orick_collect[c]; c++) {
            			if ((*wp)->t->mb_link == orick_collect[c]) { ok = 1; break;}
            		}
            	}
                if (ok) sell_out->add_weap(*wp, monst);
        }
        for (int c = 0; c < 12; c++) if (hud->jail[c]) {
          if (hud->jail[c]->m &&
              (
          	link->base == &mb_trainer ||
          	link->base == &mb_merchant ||
          	(link->base == &mb_bfc && hud->jail[c]->m->base->bfc_price)
              )
          ) sell_out->add_monst(hud->jail[c]->m, monst);
          else if (hud->jail[c]->lvl >= 0 && hud->jail[c]->hp > hud->jail[c]->hp / 4 &&
              (
          	link->base == &mb_trainer ||
          	link->base == &mb_merchant
              )
          ) sell_out->add_cage(hud->jail[c], monst);
        }
        sell_out->top_buy_item = MID(0, ti, (int)(sell_out->inventory.size() - 1));
        sell_out->focus = MID(0, fo, (int)(sell_out->inventory.size() - 1));
}

int  hud_t::upgrade(ivalu_t *t, int n) {
        if (!t) return 0;
	if (n < 0) n = t->grp;
	switch (t->typ) {
	case inv_none : break; //should never happen
	case inv_claw_up :
		claw_level = t->lvl;
        	claw_test  = t->sv;
        	spr_claw[0]  = t->ws1;
        	spr_claw[1]  = t->ws2;
	break;
	case inv_rope_up :
		rope_level = t->lvl;
        	claw_line  = t->sv;
	break;
	case inv_cage_stat_up :
        	cage_info_upgrade = 1; // t->lvl;
	break;
	case inv_thrust_up :
		thrust_level = t->lvl;
		ship->speed = t->sv / (float)(TURNS_PER_SEC);
		ship->lat_speed = t->tox / (float)(TURNS_PER_SEC);
		ship->turn_rate = (t->drg / 180. * M_PI) / (float)(TURNS_PER_SEC);
	break;
	case inv_claw_speed_up :
		claw_vel = (t->sv  / (float)(TURNS_PER_SEC));
        	claw_speed_level = t->lvl;
	break;
	case inv_hull_up :
		hull_level  = t->lvl;
		ship->mhp = ship->hp = t->sv;
		netship_spr_big = t->inv_sprite;
		netship_spr_chill[0].idx = t->ws1;
   	break;
	case inv_food :
		 food[t->sv] += n;
	break;
	case inv_injector :
        	injector[t->sv] += n;
	break;
	case inv_cage : for (int c = 0; c < 12; c++) if (jail[c]->destroyed()) {
                jail[c]->upgrade(t);
        	return 1;
   	}
   	return 0;
   	case inv_repair :
   		switch (t->lvl) {
                case 99: ship->hp = ship->mhp; for (int c = 0; c < 12; c++) jail[c]->repair(); // no break
                case 98: ship->hp = ship->mhp; break;
                default: jail[t->lvl]->repair();
 		}
	break;
        case inv_hint :
        	if (trader) switch (t->lvl) {
                case 1: trader->sayrand(minor_hints); break;
                case 2: trader->sayrand(major_hints); break;
                }
 	break;
	case inv_donation : alignment.befriend(org_police, 5000); break;

	case inv_mineral :
	case inv_scanner :
	case inv_power :
	case inv_tower :
	case inv_stabilizer :
	case inv_encl :
	case inv_proj : ship->give_weap(t, n); break;

	// fix later case inv_mineral : {} break;
	}
	return 1;
}

trader_t::~trader_t() {
  	for (vector <inven_t *>::iterator inv = inventory.begin(); inv != inventory.end(); inv++) {
                delete *inv;
   	}
   	inventory.clear();
}

void trader_t::make_pit_inv() {
}

void trader_t::make_trade_inv() {
}

void trader_t::scroll_in() {
	if (trader && !trader->link) {
		delete trader;
	}
	trader = this;
	hud->scroll_in();
	time_scroll = ms_count; scroll_dir = 1;
	hud->set_menu(hmenu_none);
	say((char *)"");	
	make_sell_list(link, menu_num);
}

void trader_t::scroll_out() {
	if (scroll_dir >= 0) {
		time_scroll = ms_count; scroll_dir = -1;
    	goodbye();
    }
}

void trader_t::swap_in() {
	if (trader && !trader->link) {
		delete trader;
	} else if (trader) {
		trader->ox = -kmenu->w;
		trader->scroll_dir = 0;  
		trader->time_scroll = 0;
		trader->say((char *)"");		 
	}
	trader = this;
	hud->scroll_in();
	time_scroll = 0; 
	scroll_dir = 0;
	greeting();
	ox = SCREEN_W - 800;
	hud->set_menu(hmenu_none);
	make_sell_list(link, menu_num);
}

void trader_t::park_advice() {
	int infect = 0, plant = 0, herb = 0, pred = 0, starving = 0;
	for(list<monst_t *>::iterator mon = home_ast->m.begin(); mon != home_ast->m.end(); mon++) {
		monst_t *m = *mon;
		if (m->base == &mb_krulasite || m->krulasite_inf) infect++;
		if (m->stomach && m->in_stomach <= 0.) starving++;
		if (m->ai_func == ai_plant ) plant++;
		else if (m->base->likes_to_eat) {
			monst_base **mb = m->base->likes_to_eat;
			if (*mb) { if ((*mb)->ai_func != ai_plant) pred++; else herb++; }
		}
	}
	if (starving) {
		say((char *)"There are poor starving monsters in your park.  Please feed them.");
	} else if (infect) {
		say((char *)"There appears to be an infection loose in your park.  "
		"Isolate, kill or inculcate infected monsters before your whole "
		"park will becomes infected."
		);
	} else if (pred && herb / pred < 3) {
		say((char *)"There are too few herbivores to maintain the number of carnivorous "
		"animals in your park.  Either import more feeder animals or thin out "
		"your carnivore population."
		);
	} else if (herb && plant / herb < 3) {
		say((char *)"There are too few plants in your park to maintain all herbivores "
		" you have.  You should definitely go collect more plants."
		);
	} else sayrand(pr_chatter);
}


void trader_t::greeting() {
	if (!link) return;
	int al = alignment.align(link);
	if (link->base == &mb_worker) {
		switch (link->aiu1 >> 24) {
		case 'K': sayrand(workerK_chatter  ); break; 
		case 'M': sayrand(workerM_chatter  ); break; 
		case 'R': sayrand(workerR_chatter  ); break; 
		case 'S': sayrand(link->sex == 1 ? workerSm_chatter : workerSf_chatter); break; 
		case 'C': sayrand(link->sex == 1 ? workerCm_chatter :  workerCf_chatter); break; 
		case 'V': sayrand(workerV_chatter); break; 
		default: sayrand(pr_chatter);
		}
	} else if (picture_idx == SPR_PRREP00) {
		if (link->base == &mb_astbase) park_advice();
		else park_advice();
	} else if (link->base == &mb_jmart) {
		say((char *)"Hello, welcome to JMart!");
	} else if (link->base == &mb_bfc) {
		say((char *)"#BFCWELCOME Welcome to BFC.  Where its so cheep it doesn't even have to be chicken.");
	} else if (link->base == &mb_wisehut) {
		char wgf[300];
		snprintf(wgf, sizeof wgf,
			"Hello I am the wise old guy of asteroid %03i.  "
			"I have lived on the asteroid belt, my entire life and I know many things.  "
			"Things thats I would be willing to share, for a small fee of course.",
			link->my_ast->asteroid_idx
		);
		say(wgf);
	} else if (link->base == &mb_arena) {
		if (active_battle) { 
			say(active_battle->results); active_battle = NULL;
		} else	say((char *)"So, who is going to fight today?");
	} else if (link->base == &mb_trainer) {
		if (active_battle) { 
			say(active_battle->results); active_battle = NULL;
		} else	{
			if (al > 1) say((char *)"I sense a man who walks with the Great Molber.  A good omen, I think.");
			else say((char *)"Hello.  I'm a monster trainer.  You up for a friendly monster-monster-match?");
		}
	} else if (link->base == &mb_fisec) {
		if (link->hp >= link->mhp / 4) say((char *)"#FISWELCOME *beep* wel-come to fi-sec.  If it can die, we can kill it.");
		else say((char *)"#FISERRERRERR *beep* Processing error... Error... Error..");
	} else if (link->base == &mb_policestation) {
		if (al > 2) say((char *)"#POLHELLOCITIZEN Hello citizen.  What can i do for you today?");
		else if (al < -2) say((char *)"#POLGOTNUTS You've got nuts coming to my asteroid.  Stay here long and you'll lose them.");
		else if (al < 0) say((char *)"#POLOHITSYOU Oh its you, what do you want?");
		else say((char *)"#POLRIGHTSIDEOFLAW It's always best to be on the right side of the law...  but not always cheap...");
	} else if (link->base == &mb_orickcamp) {
		if (al >= 2) say((char *)"#ORIHAPPYGREET Hello guy.  What you bring us?");
		else say((char *)"#ORINORMGREET Grrrrrrrr!!!  What it brings us?");
	} else if (link->base == &mb_merchant) {
		say((char *)"So, what can I interest you in today?");
	} else {
		say((char *)"<UNDEFINED>");
	}
}

void trader_t::view_switch_phrase() {
	if (!link) return;
	int al = alignment.align(link);
	if (picture_idx == SPR_PRREP00) {
			if (link->base == &mb_astbase) park_advice();
			else park_advice();
			switch (menu_num) {
			case MENU_GRAPHS: say((char *)"I keep track of all sorts of info about the park here.  "
        	  ); break;
        	case MENU_BUY: say((char *)"Here at your base you can purchase basic upgrades.  "
        	  "I can also preform other services for you such as repair your hull and "
        	  "cages after a monster has been a little too rough with you."); break;
        	case MENU_PARK: say((char *)"Changing your ticket price affects how much folks want to "
        	  "see your monsters.  If you are short cash or have too many monsters "
        	  "you can open your park up to hunting.  Hippies tend to hate it for some "
        	  "reason though."); break;
        	case MENU_ALIGNMENT: say((char *)"I keep tabs on how other known organizations in the "
				"asteroid belt feel about us."); break;
                case MENU_ENCLMAIN: say((char *)"This shows basic status of the enclosure."); break;
			case MENU_ENCLTAKE: say((char *)"You can take monsters out of your enclosure "
			"if you nee to move them for some reason."); break;
			case MENU_ENCLPUT: say((char *)"Each enclosure has a limited space for your monsters.  If you "
			"pack them in too tight or forget to feed them they may try to break out."); break;
				//case MENU_CRYO: say((char *)"cryo"); break;
			//case MENU_LAB: say((char *)"lab"); break;
			//case MENU_SCAN: say((char *)"scan");break;
			//case MENU_TURRET: say((char *)"turret");break;
			//case MENU_JUMP: say((char *)"jump");break;
			//case MENU_POWER: say((char *)"power");break;
			 
   			}
        } else if (link->base == &mb_jmart) {
                // only one view
        } else if (link->base == &mb_bfc) {
                switch (menu_num) {
        	case MENU_BUY: say((char *)"#BFCCANITAKE Can I take your order?"); break;
        	case MENU_SELL: say((char *)"#BFCPETS My manager says buy monsters to keep as pets."); break;
   		}
        } else if (link->base == &mb_wisehut) {
                // only one view
        } else if (link->base == &mb_arena) {
        	switch (menu_num) {
        	case MENU_FIGHT: say((char *)"So, who is going to fight today?"); break;
        	case MENU_BUY:  say((char *)"We carry a bit of monster grabbing gear."); break;
        	}
        } else if (link->base == &mb_trainer) {
            switch (menu_num) {
        	case MENU_BUY: say((char *)"I'll sell a monster sometimes."); break;
        	case MENU_SELL: say((char *)"Let's see what you have."); break;
        	case MENU_FIGHT: if (al > 1) say((char *)"I sense a man who walks with the Great Molber.  A good omen, I think.");
                else say((char *)"Hello.  I'm a monster trainer.  You up for a friendly monster-monster-match?"); break;
   			}                
        } else if (link->base == &mb_fisec) {
		// only one view
        } else if (link->base == &mb_policestation) {
		// only one view
        } else if (link->base == &mb_orickcamp) {
                // only one view
        } else if (link->base == &mb_merchant) {
        } else {
                say((char *)"<UNDEFINED>");
		}
}


void trader_t::goodbye() {
	if (!link) {
		/*nothing*/
 	} else if (link->base == &mb_worker) {
      	switch (link->aiu1 >> 24) {
		case 'K': sayrand(workerK_goodbye); break; 
		case 'M': sayrand(workerM_goodbye); break; 
		case 'R': sayrand(workerR_goodbye); break; 
		case 'S': sayrand(link->sex == 1 ? workerSm_goodbye : workerSf_goodbye); break; 
		case 'C': sayrand(link->sex == 1 ? workerCm_goodbye :  workerCf_goodbye); break; 
		case 'V': sayrand(workerV_goodbye); break; 
		default: sayrand(pr_goodbye);
		}
 	} else if (link->base == &mb_astbase) {
		sayrand(pr_goodbye);
	} else if (link->base == &mb_jmart) {
		say((char *)"Thank you, come again.");
	} else if (link->base == &mb_bfc) {
		say((char *)"#BFCENJOYYOURMEAL Enjoy your meal.");
	} else if (link->base == &mb_wisehut) {
		say((char *)"I need a real job.");
	} else if (link->base == &mb_arena) {
		if (active_battle) {
			say((char *)"Get ready.");		
		} else { 
			say((char *)"Remember to check the souvenir shop on your way out.");
		}
	} else if (link->base == &mb_trainer) {
		if (active_battle) { 
			say((char *)"Good luck.");
		} else	{
			say((char *)"May your monsters grow strong and fertile.");
		}
	} else if (link->base == &mb_fisec) {
		say((char *)"#FISBYE Shutting down. *beep*");
	} else if (link->base == &mb_policestation) {
		say((char *)"#POLSTAYOUTOFTROUBLE Stay out of trouble.");
	} else if (link->base == &mb_orickcamp) {
		say((char *)"#ORIBYE *Snort* Come gooder guns next time.");
	} else if (link->base == &mb_merchant) {
		say((char *)"Have a nice day.");
	} else {
        say((char *)"<UNDEFINED>");
	}
}

void trader_t::short_stock_phrase(inven_t *i) {
	if (!link) {
		/*nothing*/
	} else if (link->base == &mb_astbase) {
		say((char *)"I'm sorry sir, but MMaM HQ can't spare that many right now.");
	} else if (link->base == &mb_jmart) {
		say((char *)"I'm sorry, we seem to be a little bit low those right now.  "
		"Maybe you can come back after our next truck comes in.");
	} else if (link->base == &mb_bfc) {
		say((char *)"#BFCTENMINUTE Ummm, we have a 10 minute hold on those.  Would you like to order something else instead?");
	} else if (link->base == &mb_wisehut) {
		say((char *)"I don't know what else to say.");
	} else if (link->base == &mb_arena) {
		say((char *)"We're out!");
	} else if (link->base == &mb_trainer) {
		say((char *)"I don't have that many.");
	} else if (link->base == &mb_fisec) {
		say((char *)"#FISERRSTOCK *beep* Processing error...  Insufficient stock.");
	} else if (link->base == &mb_policestation) {
		// police don't sell
	} else if (link->base == &mb_orickcamp) {
	} else if (link->base == &mb_merchant) {
		say((char *)"I don't have that many right now.");
	} else {
                say((char *)"<UNDEFINED>");
	}
}

static int last_buy_phrase = 0;

void trader_t::buy_phrase(inven_t *i) {
       if (!link) {
        	/*nothing*/
        } else if (link->base == &mb_astbase) {
                if (i->inv() && i->inv()->typ == inv_repair) {
                	say((char *)(last_buy_phrase % 2  ? 
                	     "There you go.  That should be better." : 
                	     "You can see yourself in it now")
                	);
                } else say((char *)(last_buy_phrase % 2  ?
                	"I just used the NIOS, so you should be getting your item in a second." : 
                	"Its teleporting onto your ship now.")
                );
        } else if (link->base == &mb_jmart) {
          	 say((char *)(last_buy_phrase % 2  ? 
          	    "Thank you for shopping at J-Mart!" : "Here ya go.")
          	 );
        } else if (link->base == &mb_bfc) {
		     say((char *)(last_buy_phrase % 2  ?"#BFCENJOYYOURMEAL Enjoy your meal." : 
		 	"#BFCMONSTERSIZE Are you sure you don't want to \"Monster Size\" it for thirty cents extra?")
		 	);
        } else if (link->base == &mb_wisehut) {
		 //from hints.cc
        } else if (link->base == &mb_arena) {
                say((char *)(last_buy_phrase % 2  ? "Yeah good choice." : "Here you go."));
        } else if (link->base == &mb_trainer) {
                if (i && i->m) {
                	if (last_buy_phrase % 2 ) {
                    	char scrap[512];
                    	snprintf(scrap, sizeof scrap, "Take good care of %s, I've raised %s since %s was a little larva.", i->m->him(), i->m->him(), i->m->he());	
                    	say(scrap);
                	}
                	else say((char *)"Here's your critter.");
                } else say((char *)(last_buy_phrase % 2  ? "Thats one of my personal favorites.": "Good choice."));
        } else if (link->base == &mb_fisec) {
        	say((char *)(last_buy_phrase % 2 || sounds  ? 
        	   "#FISORDEROK *bzzt*  Order successfully processed." :
        	   "Order successfully processed. *bing*")
        	 );
        } else if (link->base == &mb_policestation) {
           	say((char *)(last_buy_phrase % 3 == 1 || sounds  ? 
           	   "#POLTRANSACTION Let's just keep this transaction between the two of us, shall we." : 
           	   last_buy_phrase % 3 == 2 ?
           	    "Uhh..  What suitcase full of money?  hehe." :
           	    "I think you've paid your debt to society.")
           	);
        } else if (link->base == &mb_orickcamp) {
                // you cannot buy anything from oricks
        } else if (link->base == &mb_merchant) {
          	say((char *)(last_buy_phrase % 2  ? "Good choice." : "Sweet!"));
	} else {
                say((char *)"<UNDEFINED>");
	}
	last_buy_phrase++;
}

void trader_t::no_space_phrase(inven_t *i) {
        if (!link) {
        	/*nothing*/
        } else if (link->base == &mb_astbase) {
          say((char *)"It won't fit, there's just not enough space in there.");
        } else if (link->base == &mb_jmart) {
          say((char *)"You don't have space for that!");
        } else if (link->base == &mb_bfc) {
          say((char *)"#BFCNEEDCAGE My manager says you need to have an empty cage if you want to buy a live monster.");
        } else if (link->base == &mb_wisehut) {
          // wizeguys don't sell anything that takes space
        } else if (link->base == &mb_arena) {
          say((char *)"Hahaha!  Your cute little ship can't hold that many.");
        } else if (link->base == &mb_trainer) {
          say((char *)"You need more cages.");
        } else if (link->base == &mb_fisec) {
          say((char *)"#FISERRSPACE *beep* Processing error...  Insufficient space.");
        } else if (link->base == &mb_policestation) {
          // police don't sell anything that takes space
        } else if (link->base == &mb_orickcamp) {
          // oricks don't sell
        } else if (link->base == &mb_merchant) {
          say((char *)"You need more cages.");
	} else {
                say((char *)"<UNDEFINED>");
	}
}

void trader_t::short_on_cash_phrase(inven_t *i) {
	if (!link) {
		/*nothing*/
	} else if (link->base == &mb_astbase) {
		say((char *)"I'm sorry sir, but we don't have enough cash for that.");
	} else if (link->base == &mb_jmart) {
		say((char *)"I'm sorry.  It you seem to be a little short on cash.");
	} else if (link->base == &mb_bfc) {
		say((char *)"#BFCINSUFFFUNDS Ummm, your card keeps giving me an \"Insufficient Funds\" error.  Let me go get my manager.");
	} else if (link->base == &mb_wisehut) {
		say((char *)"Knowledge is a valuable commodity.  One you can't seem to afford right now.");
	} else if (link->base == &mb_arena) {
		say((char *)"I don't have time for rift-raft.  Come back when you have some money.");
	} else if (link->base == &mb_trainer) {
		say((char *)"This monster is a little out of you price range.");
	} else if (link->base == &mb_fisec) {
		say((char *)"#FISERRFUNDS *beep* Processing error... Insufficient funds.");
	} else if (link->base == &mb_policestation) {
		say((char *)"#POLNOCASH No cash, no deal!");
	} else if (link->base == &mb_orickcamp) {
		// oricks don't sell
	} else if (link->base == &mb_merchant) {
		say((char *)"You don't have enough money to buy that.");
	} else {
			say((char *)"<UNDEFINED>");
	}
}

void trader_t::sell_phrase(inven_t *i, int n) {
        if (!link) {
        	/*nothing*/
        } else if (link->base == &mb_astbase) {
           // cannot sell to the astbase
        } else if (link->base == &mb_jmart) {
        } else if (link->base == &mb_bfc) {
            if (i && i->m) {
            	if (n ==1) say((char *)"#BFCGOODHOME Don't worry sir, we'll find a good home for him.");
            	else  say((char *)"#BFCFREEZER Hmmm, I hope they'll all fit in the freezer...  I mean kennel.");
            } else say((char *)"#BFCRETURNPOL Our return policy is 30 days on all food items.");
        } else if (link->base == &mb_wisehut) {
        } else if (link->base == &mb_arena) {
            if (n ==1) say((char *)"It looks a little shaby, but I'm sure my tech boys can get it fixed up.");
            else  say((char *)"Drop your junk off over there.  My Secretary will pay you on the your way out.");
        } else if (link->base == &mb_trainer) {
            if (n ==1) say((char *)"I've always wanted one of these!");
            else  say((char *)"I've always wanted some of these!");
        } else if (link->base == &mb_fisec) {
           say((char *)"#FISAQUIREOK Acquisition successfully processed.");
        } else if (link->base == &mb_policestation) {
          // you cannot sell to police
        } else if (link->base == &mb_orickcamp) {
             if (n ==1) say((char *)"#ORIMELIKES Me likes.");
             else  say((char *)"#ORIMELIKESALOT Me likes a lot.");
        } else if (link->base == &mb_merchant) {
            if (n ==1) say((char *)"I've always wanted one of these!");
            else  say((char *)"I've always wanted some of these!");
	} else {
                say((char *)"<UNDEFINED>");
	}
}




void  trader_t::poll() {
	BITMAP *kmenu = (BITMAP *)(sprites[SPR_KMENU].dat);
	if (input_map.menu_exit_but) scroll_out();
	int t = (ms_count - time_scroll);
	int arxs = 5;
	int arys = 2;
	if (scroll_dir == -1) {
		if (geararm_x < 0) { geararm_x += arxs; if (geararm_x > 0) geararm_x = 0; }
		else if (geararm_y < 0) { geararm_y += arys; if (geararm_y > 0) geararm_y = 0; }
	} else if (scroll_dir == 0 && t > 3600) {
		int park_x = -80, park_y = -15;
		// if (geararm_x == park_x) { geararm_y += arys; if (geararm_y > 0) geararm_y = 0; }
		//else
		if (geararm_x == 0 && geararm_y > park_y) { geararm_y -= 1; if (geararm_y < park_y) geararm_y = park_y; }
		else { geararm_x -= arxs; if (geararm_x < park_x) geararm_x = park_x; }
	}
	if (scroll_dir && (ms_count >= time_scroll)) {
		if (geararm_y == 0 && geararm_x == 0) ox += scroll_dir * 15;
		if (scroll_dir == -1 && (ox < -kmenu->w)) {
			scroll_dir = 0;
			trader = NULL;
			say((char *)"");
			hud->set_menu(hmenu_none);
		} else if ((scroll_dir == 1) && extended()) {
			greeting();
			ox = SCREEN_W - 800;
			scroll_dir = 0;
		}
	}
	if (link && trader == this) poll_menus();
	if (scroll_dir || (ms_count > said_done)) end_gibberish();
}


int trader_t::wants_monst(monst_t *monst) {
	return 1;
}



#define MONST_INFO_X (trader->ox + 186)
#define MONST_INFO_Y 180
#define MONST_INFO_R (trader->ox + 580)
#define MONST_INFO_B 474

void trader_t::draw() {
	draw_sprite(active_page, kmenu, ox, 0); // menu frame
	if (!link) return;
	draw_face();
	switch (menu_num) {
	case MENU_BUY: draw_menu_buy(); break;
	case MENU_SELL: draw_menu_sell(); break;
	case MENU_PARK: draw_menu_park(); break;
	case MENU_FIGHT: draw_menu_fight(); break;
	case MENU_CRYO: draw_menu_cryo(); break;
	case MENU_ALIGNMENT: draw_menu_alignment(); break;
	case MENU_LAB: draw_menu_lab(); break;
	case MENU_SCAN: draw_menu_scan();break;
	case MENU_TURRET:   draw_menu_target(0);break;
	case MENU_HUNTING:  draw_menu_target(1);break;
	case MENU_CRITTERS: draw_menu_target(2);break;
	case MENU_PAYROLL: draw_menu_payroll();break;
	case MENU_ENCLMAIN:
	case MENU_ENCLTAKE:
	case MENU_ENCLPUT: draw_menu_encl(menu_num); break;
    case MENU_JUMP: draw_menu_jump();break;
	case MENU_POWER: draw_menu_power();break;
	case MENU_GRAPHS: draw_menu_graphs();break;
   	case MENU_HIRE: draw_menu_hire(); break;
	case MENU_WORKER: draw_menu_worker(); break;
   	}
   	draw_gearbox();
}

void trader_t::render_gear(BITMAP *dest, BITMAP *tooth, int x, int y, int radius, int teeth, float ang ) {
  	int gcol = getpixel(tooth, tooth->w/2,tooth->h-1);
	circlefill(dest,x,y,radius,gcol);
        for (float a = ang; a < ang + (M_PI * 2. ); a += (M_PI * 2.) / teeth) {
		sprite_rsw(dest, tooth,
			(int)(x + cos(a) * radius),
			(int)(y + sin(a) * radius),
			a+(M_PI/2.), tooth->w, 255
		);
       }
       //line(dest,x,y,(int)(x + cos(ang) * radius),(int)(y + sin(ang) * radius), LightGreen);
}

void trader_t::draw_gearbox() {
   	int teeth = 24;	int radius = 37;
    float ang = 0.;//(geararm_y == 0 ? 0. : geararm_x / (float)(radius + 1.2));
	int x = 28 + geararm_x, y = 539 + geararm_y;
   	int gcx = -21 + geararm_x, gcy = 492 + geararm_y;
   	int guix = -trader->ox;
   	int chomp_num = 581;
   	int chomp2_num = 15;
   	BITMAP *gear_tooth = (BITMAP *)sprites[SPR_GEARTOOTH].dat;
   	BITMAP *gearcart = (BITMAP *)sprites[SPR_GEARCASE].dat;

    int ofx = MIN(0,chomp_num - guix);
    int ofy =  0;
    render_gear(active_page, gear_tooth,
        	x + ofx,
        	y + ofy,
        	radius, teeth,
        	- ( fabs(MIN(0,guix - chomp_num)) /  (float)(radius + 1.2))
	);
    draw_sprite(active_page, gearcart, gcx+ofx, gcy+ofy);
}



int trader_t::draw_menu_but( int x, int y, char *s, int ret, int sel, int stack_dir) {
	int xx = MONST_INFO_X + x;
	int yy = MONST_INFO_Y + y;
	int ex = xx + MID(20,text_length(&font_bold, s)+20, 180);
	
	int ey = yy + 16;
	if (
		mouse_x >= xx &&
		mouse_x <= ex &&
		mouse_y >= yy &&
		mouse_y <= ey
	) {
		rectfill(active_page,xx,yy,ex,ey,LightGreen);
		textout_ex( active_page,&font_bold, s,	xx+10, yy, Black, -1 );
		input_map.menu_hover = ret;
	} else {
		rect(active_page,xx,yy,ex,ey,LightGreen);
		rectfill(active_page,xx+1,yy+1,ex-1,ey-1,sel ? Green : Black);
		textout_ex( active_page,&font_bold, s,	xx+10, yy, LightGreen, -1 );
	}
	return (stack_dir ? ey + 6 - MONST_INFO_Y : ex + 6 - MONST_INFO_X );
}

int trader_t::draw_mini_but( int x, int y, char *s, int ret, int sel, int stack_dir) {
	int xx = MONST_INFO_X + x;
        int yy = MONST_INFO_Y + y;
        int ex = xx + MID(20,text_length(&font_tiny, s)+20, 180);
        int ey = yy + 12;
        if (
                	mouse_x >= xx &&
                	mouse_x <= ex &&
                	mouse_y >= yy &&
                	mouse_y <= ey
        ) {
       		rectfill(active_page,xx,yy,ex,ey,LightGreen);
       		textout_ex( active_page,&font_tiny, s,	xx+10, yy, Black, -1 );
       		input_map.menu_hover = ret;
	} else {
       		rect(active_page,xx,yy,ex,ey,LightGreen);
       		rectfill(active_page,xx+1,yy+1,ex-1,ey-1,sel ? Green : Black);
       		textout_ex( active_page,&font_tiny, s,	xx+10, yy, LightGreen, -1 );
	}
	return (stack_dir ? ey + 6 - MONST_INFO_Y : ex + 6 - MONST_INFO_X );
}


void trader_t::draw_exit() {
  	int xx = MONST_INFO_R - 60;
	int tx = MONST_INFO_R - 26;
	int yy = MONST_INFO_B - 26;
	int ex = tx + text_length(&font_tiny, (char *)"Exit");
	int ey = yy + 11;
	textprintf_ex(
		active_page, &font_tiny, tx, yy, LightGreen, -1, (char *)"Exit"
	);
	draw_sprite(active_page, (BITMAP *)(sprites[SPR_KEY_TAB].dat),	xx, yy);
	if (
			mouse_x >= xx &&
			mouse_x <= ex &&
			mouse_y >= yy &&
			mouse_y <= ey
	) {
			hline(active_page, tx+1, ey + 1, ex-1, LightGreen);
			input_map.menu_hover = HUD_MENU_EXIT; // menu exit
	}
	if (
			mouse_x >= 502 + ox &&
			mouse_x <= 579 + ox &&
			mouse_y >= 512 &&
			mouse_y <= 557
	) {
			hline(active_page, tx+1, ey + 1, ex-1, LightGreen);
			input_map.menu_hover = HUD_MENU_EXIT; // menu exit
  	}
}

void trader_t::draw_powertog() {
	if (!link || !link->my_ast || !link->pow_drain) return;
	int xx = MONST_INFO_X + 20;
	int tx = MONST_INFO_X + 48;
	int yy = MONST_INFO_B - 30;
	int ex = xx + 20;
	int ey = yy + 20;
	static int psl[] = {SPR_GRNOFF, SPR_GRNNOPOW, SPR_GRNON};
	int sprc = psl[MID(0,link->on + link->pow_on,2)];
	textprintf_ex(
		active_page, &font_micro, tx, yy, LightGreen, -1, (char *)"Power %s: %i",
		(link->pow_drain < 0 ? "Output" : "Load"),
		(link->pow_drain < 0 ? -link->pow_drain: link->pow_drain)
	);
	textprintf_ex(
		active_page, &font_micro, tx, yy+10, LightGreen, -1, (char *)"[global: %i/%i]",
		link->my_ast->orgs_powerr[link->org_idx],
                link->my_ast->orgs_powerg[link->org_idx]
	);
	if (sprc != SPR_GRNNOPOW || ms_count % 1000 < 500) {
   		draw_sprite(active_page, (BITMAP *)(sprites[sprc].dat),	xx, yy);
	}
        if (
               	mouse_x >= xx &&
               	mouse_x <= ex &&
               	mouse_y >= yy &&
               	mouse_y <= ey
        ) {
                rect(active_page, xx-2, yy-2, xx+22, yy+22, LightGreen);
                input_map.menu_hover = MENU_POWTOG;
        }
}

void trader_t::draw_upgrade() {
        if (!link || !link->base->inv_link) return;
	ivalu_t *upg_to = link->base->inv_link;
	upg_to++;
	if (upg_to->typ != link->base->inv_link->typ) return;
        monst_base *m = upg_to->mb_link;
  	int xx = MONST_INFO_R - 210;
        int tx = xx + 4;
        int yy = MONST_INFO_B - 30;
        int ex = xx + 130;
        int ey = yy + 20;
        int tcolor = LightGreen;
        if (
               	mouse_x >= xx &&
               	mouse_x <= ex &&
               	mouse_y >= yy &&
               	mouse_y <= ey
        ) {
        	rectfill(active_page, xx, yy, ex, ey, Green);
		tcolor = Black;
                input_map.menu_hover = MENU_UPGRADE;
        }
        rect(active_page, xx, yy, ex, ey, LightGreen);
	textprintf_ex(
		active_page, &font_micro, tx, yy, tcolor, -1, (char *)"%s", upg_to->nam
	);
	textprintf_ex(
		active_page, &font_micro, tx, yy+10, tcolor, -1, (char *)"Upgrade Cost: $%i",  upg_to->cost
	);
}


/*
void trader_t::draw_updown() {
  	int xx = MONST_INFO_R - 60;
        int tx = MONST_INFO_R - 26;
        int yy = MONST_INFO_B - 26;
        int ex = tx + text_length(&font_tiny, (char *)"Exit");
        int ey = yy + 11;
	textprintf_ex(
		active_page, &font_tiny, tx, yy, LightGreen, -1, (char *)"Exit"
	);
	draw_sprite(active_page, (BITMAP *)(sprites[SPR_KEY_TAB].dat),	xx, yy);
        if (
               	mouse_x >= xx &&
               	mouse_x <= ex &&
               	mouse_y >= yy &&
               	mouse_y <= ey
        ) {
                hline(active_page, tx+1, ey + 1, ex-1, LightGreen);
                input_map.menu_hover = HUD_MENU_EXIT; // menu exit
        }
}
*/

void trader_t::draw_scroll_bar(int numi, int topi, int dispi, int y1, int y2) {

}

void trader_t::draw_menu_frame(char *title) {
	rectfill(active_page, MONST_INFO_X, MONST_INFO_Y, MONST_INFO_R, MONST_INFO_B,  Black);
	if (title) textprintf_ex(
		active_page, &font_bold_plus,
		MONST_INFO_X + 15, MONST_INFO_Y + 22, LightGreen, -1, (char *)"%s", title
	);
    rect(active_page, MONST_INFO_X, MONST_INFO_Y, MONST_INFO_R, MONST_INFO_B, LightGreen);
    int xx = 30, yy = 6;
    
    
    switch (enable_park) {
    case 0: // all other displays.
    	if (enable_cryo)   xx = draw_menu_but( xx, yy, (char *)"Cryogenics", MENU_CRYO, (menu_num == MENU_CRYO));
		if (enable_lab)    xx = draw_menu_but( xx, yy, (char *)"Monster Lab", MENU_LAB, (menu_num == MENU_LAB));
        if (enable_fight)  xx = draw_menu_but( xx, yy, (char *)"Pit Fight", MENU_FIGHT, (menu_num == MENU_FIGHT));
    	if (enable_jump)   xx = draw_menu_but( xx, yy, (char *)"Jumpgate", MENU_JUMP, (menu_num == MENU_JUMP));

	if (enable_power || enable_park == 1)  xx = draw_menu_but( xx, yy, (char *)"Power", MENU_POWER, (menu_num == MENU_POWER));

	if (enable_scan)   xx = draw_menu_but( xx, yy, (char *)"Scanner Status", MENU_SCAN, (menu_num == MENU_SCAN));
	
	if (enable_turret ) xx = draw_menu_but( xx, yy, (char *)"Targeting", MENU_TURRET, (menu_num == MENU_TURRET));
	if (enable_buy )    xx = draw_menu_but( xx, yy, (char *)"Buy", MENU_BUY, (menu_num == MENU_BUY));
        if (enable_sell)   xx = draw_menu_but( xx, yy, (char *)"Sell", MENU_SELL, (menu_num == MENU_SELL));
	if (enable_worker) xx = draw_menu_but( xx, yy, (char *)"Worker", MENU_WORKER, (menu_num == MENU_WORKER));
	if (enable_encl)   xx = draw_menu_but( xx, yy, (char *)"Encl Status", MENU_ENCLMAIN, (menu_num == MENU_ENCLMAIN));
	if (enable_encl)   xx = draw_menu_but( xx, yy, (char *)"Add Monster", MENU_ENCLTAKE, (menu_num == MENU_ENCLTAKE));
	if (enable_encl)   xx = draw_menu_but( xx, yy, (char *)"Take Monster", MENU_ENCLPUT, (menu_num == MENU_ENCLPUT));

    break;
    case 1: { // park page 1
		xx = draw_menu_but( xx, yy, (char *)"Park", MENU_PARK, (menu_num == MENU_PARK));
		xx = draw_menu_but( xx, yy, (char *)"Hunting", MENU_HUNTING, (menu_num == MENU_HUNTING));
 		xx = draw_menu_but( xx, yy, (char *)"Hire", MENU_HIRE, (menu_num == MENU_HIRE));
	    xx = draw_menu_but( xx, yy, (char *)"Buy", MENU_BUY, (menu_num == MENU_BUY));
	} break;
	  
	case 2: { // park page 2
		xx = draw_menu_but( xx, yy, (char *)"Jumpgate", MENU_JUMP, (menu_num == MENU_JUMP));
		xx = draw_menu_but( xx, yy, (char *)"Power", MENU_POWER, (menu_num == MENU_POWER));
		xx = draw_menu_but( xx, yy, (char *)"Scanners", MENU_SCAN, (menu_num == MENU_SCAN));
		xx = draw_menu_but( xx, yy, (char *)"IFF", MENU_TURRET, (menu_num == MENU_TURRET));
	} break;
	
	case 3: { // park page 3
		xx = draw_menu_but( xx, yy, (char *)"Critters", MENU_CRITTERS, (menu_num == MENU_CRITTERS));
		xx = draw_menu_but( xx, yy, (char *)"Payroll", MENU_PAYROLL, (menu_num == MENU_PAYROLL));
		xx = draw_menu_but( xx, yy, (char *)"Align", MENU_ALIGNMENT, (menu_num == MENU_ALIGNMENT));
		xx = draw_menu_but( xx, yy, (char *)"Graphs", MENU_GRAPHS, (menu_num == MENU_GRAPHS));
	} break;
	}
	if (enable_park)	draw_menu_but( 330, yy, (char *)"[+]", MENU_MORE, 0);	
}

void trader_t::draw_face() {
	int mouth_pos = 0;
	if (raw_puppet) {
		int tweek_sound = 0;
		for (;raw_puppet[raw_puppet_idx + 1]; raw_puppet_idx +=3 ) { 
			if (ms_count < raw_puppet[raw_puppet_idx + 1]) break;
		}
		if (raw_puppet[raw_puppet_idx + 1] == 0) {
			free(raw_puppet); raw_puppet = NULL;
			if (npc_voice) deallocate_voice(npc_voice);   npc_voice = 0; 
		} else 	if (npc_voice) {
			float freq_mod_tab[5] = {1.,1.,1., 1.15, 1.33};
			int vol_mod_tab[5] = {80,80,255,255,255};
			SAMPLE *sss = voice_check(npc_voice);				
			if (raw_puppet[raw_puppet_idx + 2] >= 0 && sss) {
				int sl = sss->len;
				voice_set_position(npc_voice, (raw_puppet[raw_puppet_idx + 2]*10) % sl);
			}
			voice_set_frequency(npc_voice, 
				(int)(gibberish_freq[gibberish_idx] * 
				freq_mod_tab[MID(0,raw_puppet[raw_puppet_idx+2],4)])
			);
			
			voice_ramp_volume(npc_voice, 
				vol_mod_tab[MID(0,raw_puppet[raw_puppet_idx+2],4)], 
				MAX(0, raw_puppet[raw_puppet_idx + 4] - raw_puppet[raw_puppet_idx + 1])
			);
			
		 	
		} 
	}
	switch (picture_layout_idx) {
	case 0: // standard anim
		mouth_pos = raw_puppet ? MID(0,raw_puppet[raw_puppet_idx],4) : 1;	
		if (mouth_pos == 1) mouth_pos = ((ms_count + ms_count / 64) % 4000 < 160 ? 0 : 1);
		break;
	case 1: mouth_pos = 0;	break; //unanimated (workers)
	}	
	BITMAP *spr = (BITMAP *)sprites[picture_idx + mouth_pos].dat;
	draw_sprite(active_page,spr, ox + 18,18);
	if (said) textout_box(active_page, &font_bold, said,
        	(MONST_INFO_X), 24,
		(MONST_INFO_R - MONST_INFO_X), 122, LightGreen, -1
	);
}

void trader_t::poll_menus() {
	int imp_hi = (input_map.menu_but & 0xffff0000);
	int imp_lo = (input_map.menu_but & 0x0000ffff);
	if (input_map.menu_but == MENU_POWTOG) link->on ^= 1;
	if (input_map.menu_but == MENU_POWTOG0 && watch_monst) watch_monst->on ^= 1;
	if (input_map.menu_but == MENU_MORE) {
	    if (++enable_park == 4) enable_park = 1; 
	}
	switch (imp_hi) {
	case MENU_JUMP0:
		if (imp_lo == 0) {
			say((char *)"You are already there.");
		} else if (!link->pow_on) {
			say((char *)"This jumpgate stabilizer does not have power!");
		} else if (remote_ast) {
			say((char *)"A jumpgate is already open, so I cannot open another one until this one closes.");
		} else if (jg_respawn_time > ms_count) {
			say((char *)"The jumpgate particle refulx inductor coil is still recharging.  I cannot open a jumpgate just yet.");
		} else if (imp_lo < home_ast->scanner_count[2] * 200) {
			int lvl = (link->base->inv_link ? link->base->inv_link->lvl : 0);
			if (lvl == 4 || hud->marked[imp_lo] == 2) {
				jg_open( imp_lo );
				say((char *)"Jumpgate opened.");
			} else say((char *)"You can only choose to jump to asteroids that have been "
				"marked with a radio marker with this level of jumpgate stabilizer."
			);
			} else say((char *)"Out of scanner range.");
			break;
	}
	if (
		input_map.menu_but >= MENU_SUB0 + 1 &&
        	input_map.menu_but <= MENU_SUB0 + 32
	) {
		 menu_num = input_map.menu_but;
		 make_sell_list(link, menu_num);
		 focus = top_buy_item = 0;
		 view_switch_phrase();
	}
    if (input_map.menu_but == MENU_UPGRADE && link->base->inv_link) {
		ivalu_t *upg_to = link->base->inv_link;
		upg_to++;
		if (upg_to->typ == link->base->inv_link->typ) {
                	if (upg_to->cost <= hud->money ) {
                        	hud->money -= upg_to->cost;
                                link->building_upgrade();
                                say((char *)"Upgrade Done.");
                   	} else say((char *)"Not enough money for upgrade.");
    	}
	} else if (menu_num == MENU_JUMP) {
		jg_first = MID(0, jg_first + 100 * input_map.sel, 700);
		switch (input_map.menu_but) {
		case MENU_TOGAUTOJUMP:  jg_autojump ^= 1; break;
		case MENU_JUMPCLOSE: delete remote_ast; remote_ast = NULL; break;
		case MENU_JUMPOPEN: if (jg_respawn_time > ms_count) {
					say((char *)"The jumpgate particle refulx inductor coil is still recharging.  "
						"I cannot open a jumpgate just yet."
				);
				} else {
					jg_open();
					say((char *)"Jumpgate opened.");
				}
			break;

		}
	} else if (menu_num == MENU_HUNTING) {
		if (imp_hi == MENU_TARGET0) mb_list_all[imp_lo]->stat_valid_target ^= TARGET_HUNTER;
	} else if (menu_num == MENU_TURRET) {
		if (imp_hi == MENU_TARGET0) mb_list_all[imp_lo]->stat_valid_target ^= TARGET_AST0; 
	} else if (menu_num == MENU_WORKER) {
		monst_t *guy = link;
        if (!guy) return;
        weap_t *w = guy->armed();
        
		switch (input_map.menu_but) {
			case MENU_WORKER_NEXT: { 
				monst_t *lg = NULL, *fg = NULL; int done = 0;
				for (list<monst_t *>::iterator mon = ast->m.begin(); mon != ast->m.end(); ++mon) {
					if (lg == guy && (*mon)->base == &mb_worker) {
						done = 1;						
						(*mon)->trader_ui->swap_in();
    					break;
					}
					if ((*mon)->base == &mb_worker) lg = (*mon);
					if (!fg) fg = lg;
				}
				if (!done && fg && fg != guy) {					
					fg->trader_ui->swap_in();   					
				}			
			} break; 
			case MENU_WORKER_PREV: { 
				monst_t *lg = NULL, *fg = NULL; int done = 0;
				for (list<monst_t *>::iterator mon = ast->m.begin(); mon != ast->m.end(); ++mon) {
					if ((*mon) == guy) {
						if (lg) {
							break;
						} else {
							for (mon; mon != ast->m.end(); ++mon) {
								if ((*mon)->base == &mb_worker) lg = (*mon);
							}
							break;
						}
					} else if ((*mon)->base == &mb_worker) lg = (*mon);					
				}
				if (lg && lg != guy) { 
					lg->trader_ui->swap_in();
				}	
			} break; 
			case MENU_WORKER_PAYUP: guy->worker_salary += 10; break;
			case MENU_WORKER_PAYDN: if (guy->worker_salary > 400) {
					guy->worker_salary -= 10;
					alignment.offend(org_hippies, 500);
				} 
				break;
			case MENU_WORKER_GIVE_WEAP: 
				guy->drop_weap();
				if (ship->armed() && ship->armed()->count > ship->armed()->t->grp) { 
					guy->give_weap(ship->armed()->t, ship->armed()->t->grp);
					ship->armed()->count -= ship->armed()->t->grp;
				} else if (ship->armed()) {
					guy->give_weap(ship->armed()->t, ship->armed()->count);
					ship->armed()->count = 0; ship->fire(0.); // free the weapon arm next one.	
				} break;
			case MENU_WORKER_TRAIN: if (worker_skill_upgrade_cost[guy->worker_skill + 1] <= hud->money) {
				guy->worker_skill++;
				hud->money -= worker_skill_upgrade_cost[guy->worker_skill];
				char scrap[512]; snprintf(scrap, sizeof scrap, "%s is now better at what %s does.", guy->name, guy->he());
				say(scrap);
			} else say((char *)"You can't afford training right now.");
			break; 
			case MENU_WORKER_FIRE: if (!guy->jump_out_time) {
				say((char *)"Whatever, I didn't need this job anyway!");
				guy->jump_out_time = ms_count + (guy->aiu1 & QWERK_VENGEFUL ? 15000 : 8000);        	
			} break;
		}
    } else if (menu_num == MENU_FIGHT && extended() && battle_list.size()) {
        if (imp_hi == MENU_BUYN0) bs_grp = imp_lo;
        if (input_map.sel) say((char *)"");
        int last_focus = focus;
        focus = MID(0, focus + input_map.sel, (int)(battle_list.size() -1) );
        if (focus > top_buy_item + SHOW_ITEMS - 1) top_buy_item = focus - (SHOW_ITEMS - 1);
   		if (focus < top_buy_item) top_buy_item = focus;
   		int action = input_map.menu_but;
        if ((0xff0000 & input_map.menu_but) == MENU_BUY0) {
			int f = input_map.menu_but & 0xffff;
			action = (f == focus ? MENU_DO_BATTLE : 0); 
	   		focus = f;	    	
	    }
	    battle_desc *k = battle_list[focus];
   		if (!k) return;        
   		if (focus != last_focus) say((char *)k->long_desc); 
	    if (action == MENU_DO_BATTLE) {
			int p = k->entryfee;
			if (p <= hud->money) {
				hud->money -= p;
				k->start_battle();
			} else short_on_cash_phrase(NULL);
    	}    	
    } else if (menu_num == MENU_HIRE && extended() && applicants.size()) {
        if (imp_hi == MENU_BUYN0) bs_grp = imp_lo;
        if (input_map.sel) say((char *)"");
        int last_focus = focus;
        focus = MID(0, focus + input_map.sel, (int)(applicants.size() -1) );
        if (focus > top_buy_item + SHOW_ITEMS - 1) top_buy_item = focus - (SHOW_ITEMS - 1);
   		if (focus < top_buy_item) top_buy_item = focus;
   		int action = input_map.menu_but;
        if ((0xff0000 & input_map.menu_but) == MENU_BUY0) {
			int f = input_map.menu_but & 0xffff;
			action = (f == focus ? MENU_DO_HIRE : 0); 
	   		focus = f;
	    	
	    } 
	    struct inven_t *k = applicants[focus];
   		if (!k) return;        
   		monst_t *guy = k->m;
        if (!guy) return;
        hud->show_item_info(k->inv());  		
   		// hud->watch_monst = guy;
        if (input_map.sel || ((0xff0000 & input_map.menu_but) == MENU_BUY0)) {
	    	say(guy->worker_history);
	    }
	    int remove_guy = 0;
	    switch (action) {
	    case MENU_DO_HIRE: {
	            int p = k->price;
				if (p <= hud->money) {
					k->pointer_is_mine = 0;
					home_ast->add_monst_by(guy, ship->x, ship->y);
					home_ast->add_jumpgate(guy->x, guy->y);
					hud->money -= p;
				    remove_guy = 1;
    		    } else short_on_cash_phrase(k);
    		}
    		break;
	    case MENU_DISMIS: remove_guy = 1; break;
	    case MENU_DRUG_TEST: {
	    		int p = 100;
				if (p <= hud->money) {
					int drugs = guy->aiu1 & QWERK_DRUGS;
				    if (drugs && irand(4) == 0) {
				    	char scrap[512];
				    	static char *excuse_tab[6] = {
				    	  (char *)"left the oven on at home", (char *)"was late for a doctor's appointment",
				    	  (char *)"likes drugs too much to work here", (char *)"wouldn't pass",
				    	  (char *)"forgot something in his shuttle", (char *)"was surrounded by spiders"
				    	};
				    	snprintf(scrap, sizeof scrap, "When I started talking about a drug test, %s suddenly realized that %s %s and left quickly.", guy->name, guy->he(), excuse_tab[irand(6)]);
				    	say(scrap);
				    	remove_guy = 1;
				    } else {
				    	hud->money -= p;
				    	say((char *)(drugs ? "Test is positive for several mind altering substances." : "The test is clean."));	   
				    } 			
    		    } else short_on_cash_phrase(k);
	    	}
	    	break;
	    case MENU_CHECK_BACKGROUND: {
	    		int p = 100;
				if (p <= hud->money) {
					hud->money -= p;
				    say(guy->worker_rapsheet ? guy->worker_rapsheet : (char *)"No criminal record found.");	    			
    		    } else short_on_cash_phrase(k);
	    	}
	    	break;
	    }
		if (remove_guy) {	
			hud->show_item_info(NULL);
			delete k;						
			for (vector <inven_t *>::iterator kill = applicants.begin(); 1; kill++) if (*kill == k) {
				applicants.erase(kill); break;			
			}
		}
	} else if (menu_num == MENU_GRAPHS) {
		graphs->handle_input(1);
	} else if (menu_num == MENU_PARK) {
       	switch (input_map.menu_but) {
        	case MENU_TICKET_UP: hud->ticket_price += 10; break;
        	case MENU_TICKET_DN: if (hud->ticket_price > 0) hud->ticket_price -= 10; break;
        	case MENU_HUNT_OK:  hud->hunting_allowed ^= 1; break;
			case MENU_HUNT_UP: hud->hunting_price += 10; break;
        	case MENU_HUNT_DN: if (hud->hunting_price > 0) hud->hunting_price -= 10; break;
		}
   	} else if (menu_num == MENU_BUY && extended() && inventory.size()) {
        if (imp_hi == MENU_BUYN0) bs_grp = imp_lo;
        if (input_map.sel) say((char *)"");
        focus = MID(0, focus + input_map.sel, (int)(inventory.size() -1) );
        if (focus > top_buy_item + SHOW_ITEMS - 1) top_buy_item = focus - (SHOW_ITEMS - 1);
   		if (focus < top_buy_item) top_buy_item = focus;
   		hud->show_item_info(inventory[focus]->inv());
   		//say(inventory[focus]->inv()->desc);
   		//hud->watch_monst = inventory[focus]->m;
        if ((0xff0000 & input_map.menu_but) == MENU_BUY0) {
			int f = input_map.menu_but & 0xffff;
			if (f == focus) {
				struct ivalu_t *k = inventory[f]->inv();
				int p = inventory[f]->price;
				int n = (inventory[f]->group_buy() ? bs_grp : 1);
				if (n > inventory[f]->count && inventory[f]->count != -1) {
						short_stock_phrase(inventory[f]);
				} else if (p <= hud->money * n) for (int nc = 0; nc < n; nc++) {
					int ok = (inventory[f]->ivalu && hud->upgrade(k));
					if (inventory[f]->m) {
						for (int c = 0; c < 12; c++) if (hud->jail[c]->vacent()) {
						hud->jail[c]->pack(inventory[f]->m);
						ok = 1;
						inventory[f]->pointer_is_mine = 0;
						break;
						}
					}
					if (inventory[f]->w) {
						int take = (inventory[f]->plus ? inventory[f]->plus : inventory[f]->w->t->grp);
						ship->give_weap(k, take);
						inventory[f]->w->count -= take;
						ok = 1;
					}
					if (ok) {
						if (!k || k->typ != inv_hint) buy_phrase(inventory[f]);
						hud->money -= p;
						alignment.befriend(link, MID(10, p/50, 400), 2);
						if (inventory[f]->plus) { inventory[f]->count++;   inventory[f]->plus = 0; }
						if (inventory[f]->count > 0) {
								inventory[f]->count--;
								if (inventory[f]->count == 0) {
										delete inventory[f];
										for (vector <inven_t *>::iterator k = inventory.begin(); 1; k++) if (*k == inventory[f]) {
												inventory.erase(k); break;
										}
								}
        					}
     					} else no_space_phrase(inventory[f]);
                             	} else short_on_cash_phrase(inventory[f]);
                  	} else focus = f;
            	}
        } else if (sell_out && menu_num == MENU_SELL && extended() && sell_out->inventory.size()) {
                if (imp_hi == MENU_BUYN0) sell_out->bs_grp = imp_lo;
	        if (input_map.sel) say((char *)"");
                sell_out->focus = MID(0, sell_out->focus + input_map.sel, (int)(sell_out->inventory.size() -1) );
                if (sell_out->focus > sell_out->top_buy_item + SHOW_ITEMS - 1) sell_out->top_buy_item = sell_out->focus - (SHOW_ITEMS - 1);
                if (sell_out->focus < sell_out->top_buy_item) sell_out->top_buy_item = sell_out->focus;
   		hud->show_item_info(sell_out->inventory[sell_out->focus]->inv());
   		//hud->watch_monst = sell_out->inventory[sell_out->focus]->m;
                if ((0xff0000 & input_map.menu_but) == MENU_BUY0) {
                        int f = input_map.menu_but & 0xffff;
                        if (f == sell_out->focus) {
                          	int n = (sell_out->inventory[f]->group_buy() ? MIN(sell_out->bs_grp, sell_out->inventory[f]->count) : 1);
                          	int p = sell_out->inventory[f]->price;
	                       	sell_phrase(sell_out->inventory[f], n);
	                       	hud->money += p * n;
                                for (int nc = 0; nc < n; nc++) {
                                  	if (sell_out->inventory[f]->w) {
      						if (sell_out->inventory[f]->plus) {
        	                                	sell_out->inventory[f]->w->count -= sell_out->inventory[f]->plus;
	                                        	weap_t *wp = new weap_t;
                                	        	wp->t = sell_out->inventory[f]->w->t;
                        	                	wp->count = sell_out->inventory[f]->plus;
                	                        	add_weap(wp);
        	                                	sell_out->inventory[f]->plus = 0;
	             				} else {
							sell_out->inventory[f]->w->count -= sell_out->inventory[f]->rnd();
                   					for (int i = 0; NUM_IVALU_LIST; i++ ) if (&ivalu_list[i] == sell_out->inventory[f]->w->t) {
                                	        		add_item(i,1); break;
                        	                	}
                	   			}
        	                        	if (sell_out->inventory[f]->w->count == 0) ship->weap.remove(sell_out->inventory[f]->w);
	      				} else if (sell_out->inventory[f]->m) {
            					add_monst(sell_out->inventory[f]->m);
            					for (int c = 0; c < 12; c++) {
                  					if (hud->jail[c]->m == sell_out->inventory[f]->m) hud->jail[c]->m = NULL;
                	          		}
        	  			} else if (sell_out->inventory[f]->cage) {
	               				add_item(FIRST_INV_CAGE_IDX + sell_out->inventory[f]->cage->lvl, 1);
               					sell_out->inventory[f]->cage->upgrade(NULL);
                  			} else if (sell_out->inventory[f]->ivalu >= 0) {
                        	                int zz;
                	                	add_item(sell_out->inventory[f]->ivalu, 1);
        	                        	zz = sell_out->inventory[f]->ivalu - FIRST_INV_FOOD_IDX;
	                                	if (zz >= 0 && zz < 10) hud->food[zz]--;
                                		zz = sell_out->inventory[f]->ivalu - FIRST_INV_INJECTOR_IDX;
                                		if (zz >= 0 && zz < 10) hud->injector[zz]--;
	                  		}
					alignment.befriend(link, MID(10, p/50, 400), 2);
                                }
                                make_sell_list(link, menu_num);
                  	} else sell_out->focus = f;
            	}
        }
}

void trader_t::draw_item_list() {
  	if (link && link->base == &mb_fisec && link->hp <= link->mhp/4) {
  		textout_ex(
			active_page, &font_bold_plus, (char *)"<SYSTEM ERROR!>",
			MONST_INFO_X + 100, MONST_INFO_Y + 140, LightGreen, -1
		);
   		return;	
  	}  	
  	if (link && alignment.align(link) <= -2 && link->org_idx != org_police && link->org_idx != org_ast0) {
  		textout_ex(
			active_page, &font_bold_plus, (char *)"<WONT DO BUSINESS WITH YOU>",
			MONST_INFO_X + 60, MONST_INFO_Y + 140, LightGreen, -1
		);
   		return;	
  	}  	
  	if (top_buy_item >= (int)(inventory.size())) {
        textout_ex(
			active_page, &font_bold_plus, (char *)"<NOTHING>",
			MONST_INFO_X + 120, MONST_INFO_Y + 140, LightGreen, -1
		);
   		return;
	}
	int e = MIN((int)(inventory.size()), top_buy_item + SHOW_ITEMS);
	for (int i= top_buy_item; i < e; i++) {
   		int ty = MONST_INFO_Y + 48 + 20 * (i - top_buy_item);
   		struct ivalu_t *k = inventory[i]->inv();
   		int xx = MONST_INFO_X + 5;
   		int yy = ty - 4;
   		int ex = MONST_INFO_R - 16;
   		int ey = ty + 20;
   		if (
               		mouse_x >= xx &&
               		mouse_x <= ex &&
               		mouse_y >= yy &&
               		mouse_y <= ey
        	) {
                	input_map.menu_hover = i + MENU_BUY0;
         	}
                if (i == focus) { rectfill(active_page, xx,yy,ex,ey, Green);  }
                int xxn = text_length(&font_bold, inventory[i]->name()) + MONST_INFO_X + 17;
        	textout_ex(
			active_page, &font_bold, inventory[i]->name(),
			MONST_INFO_X + 15, ty, LightGreen, -1
		);
        	if (inventory[i]->count > -1) textprintf_ex(
			active_page, &font_micro,
			xxn, ty+4, LightGreen, -1, (char *)"(%i%s)",
			inventory[i]->count, (inventory[i]->plus ? "+" : "")
		);
                if (k && k->typ == inv_donation) {
                	ivalu_t *pd = &ivalu_list[FIRST_INV_DONATION_IDX];
  			int pa = how_aligned(police);
  			pd->nam = (char *)(pa < -2 ?  "Bribe Police" : pa < 0 ? "Pay Fines" : "Make \"Donation\"");
  			pd->cost = (pa < -2 || pa > 2 ?  30000 : pa < 0 ? 20000 : 10000);
  			pd->desc = (char *)"Sometimes a little insentive helps the police do their jobs better.";
                        inventory[i]->price = pd->cost;
                }
		if (k && k->typ == inv_repair) {
           		switch (k->lvl) {
               		case 99: {
                   		int alc = ivalu_list[FIRST_INV_HULL_IDX + hud->hull_level].cost;
                   		inventory[i]->price = (int)((float)((ship->mhp - ship->hp) / ship->mhp * alc / 2.));
                                for (int c = 0; c < 12; c++) inventory[i]->price += hud->jail[c]->repair_cost();
                 	}
                 	break;
                        case 98: {
                   		int alc = ivalu_list[FIRST_INV_HULL_IDX + hud->hull_level].cost;
                   		inventory[i]->price = (int)((float)((ship->mhp - ship->hp) / ship->mhp * alc / 2.));
                 	}
                 	break;
                 	default: inventory[i]->price = hud->jail[k->lvl]->repair_cost();
             		}
           	} else {
                        if (inventory[i]->rnd() >= 0) textprintf_ex(
				active_page, &font_bold,
				MONST_INFO_X + 220, ty, LightGreen, -1, (char *)"[rnd:%3i]",
				inventory[i]->rnd()
			); else if (inventory[i]->lvl() >= 0) textprintf_ex(
				active_page, &font_bold,
				MONST_INFO_X + 220, ty, LightGreen, -1, (char *)"[lvl:%3i]",
				inventory[i]->lvl()
			);
                }
        	textprintf_ex(
			active_page, &font_bold,
			MONST_INFO_X + 300, ty, LightGreen, -1, (char *)"$%7i",
			(int)(inventory[i]->price)
		);
   	}
   	if (inventory[focus]->group_buy()) {
   		int xx = 12, yy = MONST_INFO_B - 30 - MONST_INFO_Y;
		xx = draw_mini_but( xx, yy, (char *)" 1 ", MENU_BUYN0 +   1, (bs_grp ==   1));
		xx = draw_mini_but( xx, yy, (char *)" 5 ", MENU_BUYN0 +   5, (bs_grp ==   5));
		xx = draw_mini_but( xx, yy, (char *)" 20", MENU_BUYN0 +  20, (bs_grp ==  20));
		xx = draw_mini_but( xx, yy, (char *)"100", MENU_BUYN0 + 100, (bs_grp == 100));
   	}
}

void trader_t::draw_hire_list() {
  	if (top_buy_item >= (int)(applicants.size())) {
        textout_ex(
			active_page, &font_bold_plus, (char *)"<NOBODY AVAILABLE>",
			MONST_INFO_X + 100, MONST_INFO_Y + 140, LightGreen, -1
		);
		top_buy_item = 0; 
   		return;
	}
	int e = MIN((int)(applicants.size()), top_buy_item + SHOW_ITEMS);
	for (int i= top_buy_item; i < e; i++) {
   		int ty = MONST_INFO_Y + 48 + 20 * (i - top_buy_item);
   		struct inven_t *k = applicants[i];
   		int xx = MONST_INFO_X + 5;
   		int yy = ty - 4;
   		int ex = MONST_INFO_R - 16;
   		int ey = ty + 20;
   		if (
			mouse_x >= xx &&
			mouse_x <= ex &&
			mouse_y >= yy &&
			mouse_y <= ey
        ) {
                	input_map.menu_hover = i + MENU_BUY0;
       	}
        if (i == focus) { rectfill(active_page, xx,yy,ex,ey, Green);  }
        int xxn = text_length(&font_bold, k->name()) + MONST_INFO_X + 17;
		monst_t *guy = k->m;
        if (guy) {
			char qwerks[] = "01234567DPVpLlAv";
			for (int c = 0; c < 16; c++) {
				if (((guy->aiu1 >> c) & 0x1) == 0) qwerks[c] = '.';        
			}
			
			
			char *class_str = guy->worker_class(1);
			textout_ex(
				active_page, &font_bold, guy->name,
				MONST_INFO_X + 15, ty, LightGreen, -1
			);
        	
			if (arg_xinfo) textprintf_ex(
				active_page, &font_micro,
				MONST_INFO_X + 130, ty+2, LightGreen, -1, (char *)"[%s]",qwerks
				
			);
            
			textprintf_ex(
				active_page, &font_bold,
				MONST_INFO_X + 240, ty, LightGreen, -1, (char *)"%s", class_str
			);
            
			k->price = guy->worker_salary;
            textprintf_ex(
			    active_page, &font_bold,
			    MONST_INFO_X + 330, ty, LightGreen, -1, (char *)"$%4i",
			    (int)(k->price)
		    );
        } else {
            textout_ex(
				active_page, &font_bold, (char *)"---",
				MONST_INFO_X + 15, ty, LightGreen, -1
			);        	
        }
   	}
	
	int xx = 12, yy = MONST_INFO_B - 30 - MONST_INFO_Y;
	xx = draw_mini_but( xx, yy, (char *)"X", MENU_DISMIS, 0);
	xx = draw_mini_but( xx, yy, (char *)"Hire", MENU_DO_HIRE, 0);
	xx = draw_mini_but( xx + 8, yy, (char *)"Backgr. $100", MENU_CHECK_BACKGROUND, 0);
	// xx = draw_mini_but( xx, yy, (char *)"Verify Ref. $50", MENU_CHECK_REFS, 0);
	xx = draw_mini_but( xx, yy, (char *)"Drug Test $100", MENU_DRUG_TEST, 0);

}

void trader_t::draw_menu_buy() {
	draw_menu_frame((char *)"Buy:");
    draw_item_list();
   	draw_exit();
}

void trader_t::draw_menu_hire() {
	draw_menu_frame((char *)"Hire:");
    draw_hire_list();
   	draw_exit();
}

void trader_t::draw_menu_worker() {
	draw_menu_frame((char *)"Employee:");
    draw_exit();
    monst_t *wrkr = link;
    if (!wrkr) return;

	char *work_ethic_str[9] = {
	  (char *)"Has no plans to do any work of any type", //0
      (char *)"Extremely Lazy",  //1
      (char *)"Very Lazy", // 2
      (char *)"Lazy",  // 3
      (char *)"Normal",  // 4
      (char *)"Good Worker", // 5
      (char *)"Hard Worker", // 6
      (char *)"Excellent Worker", // 7
      (char *)"Workaholic" // 8
    };
    int work_bits = 0;
    for (int i = 0; i < 8; i++) work_bits += ((wrkr->aiu1 >> i) & 0x1); 
    char showqwerks[500] = ""; char *qo = showqwerks;

    if (wrkr->aiu1 & QWERK_POLITICAL)  qo += sprintf(qo, "political, ");
    if (wrkr->aiu1 & QWERK_LAWFUL)     qo += sprintf(qo, "lawful, ");
    if (wrkr->aiu1 & QWERK_LOYAL)      qo += sprintf(qo, "loyal, ");
    if (wrkr->flags & AF_ADVTARGET) qo += sprintf(qo, "sharpshooter, ");;
	if (wrkr->aiu1 & QWERK_DRUGS)      qo += sprintf(qo, "drug user, "); 
    if (wrkr->gestation && !(wrkr->flags & AF_FAMILY) && wrkr->sex == 2) qo += sprintf(qo, "easy, ");;
	if (!wrkr->gestation && wrkr->sex == 2) qo += sprintf(qo, "frigid, ");
	if (!wrkr->gestation && wrkr->sex == 1) qo += sprintf(qo, "dork, ");;
	if (wrkr->aiu1 & QWERK_PACIFIST)   qo += sprintf(qo, "pacifist, ");
    if (wrkr->aiu1 & QWERK_VIOLENT)    qo += sprintf(qo, "violent, ");
    if (wrkr->aiu1 & QWERK_ADHD)       qo += sprintf(qo, "adhd, ");
    if (wrkr->aiu1 & QWERK_VENGEFUL)   qo += sprintf(qo, "vengeful, ");
    if (wrkr->flags & AF_PARANOID) qo += sprintf(qo, "paranoid, ");
			
    if (qo == showqwerks) sprintf(qo, "none known"); else { qo -= 2; *qo = 0; }
    
	char *class_str = wrkr->worker_class();
	
	
    int r1 = 46;
	draw_menu_but( 30, r1, (char *)"<", MENU_WORKER_PREV, 0);
	textprintf_ex(
		active_page, &font_bold,
		MONST_INFO_X + 80, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"%s", wrkr->name
	);	
	draw_menu_but( 340, r1, (char *)">", MENU_WORKER_NEXT, 0);
    r1 += 20;
	textprintf_ex(
		active_page, &font_bold,
		MONST_INFO_X + 30, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"Weekly Pay:"
	);
    draw_menu_but( 150, r1, (char *)"+", MENU_WORKER_PAYUP, 0);
	textprintf_ex(
		active_page, &font_bold,
		MONST_INFO_X + 190, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"$%4i", wrkr->worker_salary
	);
	draw_menu_but( 250, r1, (char *)"-", MENU_WORKER_PAYDN, 0);
    draw_menu_but( 290, r1, (char *)"Fire!", MENU_WORKER_FIRE, 0);
	
    r1 += 20;
    //draw_menu_but( 150, r1, (char *)"<", MENU_WORKER_WEAPUP, 0);
	//draw_menu_but( 300, r1, (char *)">", MENU_WORKER_WEAPDN, 0);
    
	if (weap_t *weapon = wrkr->armed()) {
	
		textprintf_ex(
			active_page, &font_bold,
			MONST_INFO_X + 30, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"Weapon: %s : %i", 
			weapon->t->nam, weapon->count
		);
		enable_turret = 1;
    } else {
		textprintf_ex(
			active_page, &font_bold,
			MONST_INFO_X + 30, MONST_INFO_Y + r1 , LightGreen, -1, 
			(char *)(wrkr->aiu1 & QWERK_PACIFIST ? "Weapon: Unarmed (pacifist)" : "Weapon: Unarmed" )
		);    
        enable_turret = 0;
    }
    if (!(wrkr->aiu1 & QWERK_PACIFIST) && ship->armed()) {
    	char tt[200];
    	snprintf(tt, sizeof tt, "Give: %s", ship->armed()->t->nam );
    	draw_menu_but( 200, r1, tt, MENU_WORKER_GIVE_WEAP, 0);
    }
    r1 += 20;
    textprintf_ex(
		active_page, &font_bold,
		MONST_INFO_X + 30, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"Job Title: %s", class_str
	);
    r1 += 20;
    textprintf_ex(
		active_page, &font_bold,
		MONST_INFO_X + 30, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"Health: %i/%i",wrkr->hp,wrkr->mhp
	);
    
    r1 += 20;
    static char *skill_desc_tab[] = {
    
    	NULL, (char *)"Novice", (char *)"Experianced", (char *)"Veteran", 
    	(char *)"Master", (char *)"Grand Master"
    };
    static char *skill_desc_tab_s[] = {
    	NULL, (char *)"Rookie", (char *)"Experianced", (char *)"Veteran", 
    	(char *)"Elite", (char *)"Master Elite"
    };
    int cat = (wrkr->aiu1 >> 24);
    textprintf_ex(
		active_page, &font_bold,
		MONST_INFO_X + 30, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"Skill Level (%i): %s", 
		wrkr->worker_skill,
		cat == 'S' || cat == 'K' ? skill_desc_tab_s[MID(1,wrkr->worker_skill,5)] :
		skill_desc_tab[MID(1,wrkr->worker_skill,5)]
	);
	char skill_btn[64]; snprintf(skill_btn, sizeof skill_btn, "Train $%i", worker_skill_upgrade_cost[wrkr->worker_skill + 1]);
    if (wrkr->worker_skill < 5) draw_menu_but( 250, r1, skill_btn, MENU_WORKER_TRAIN, 0);
	r1 += 20;

    
         
    textprintf_ex(
		active_page, &font_bold,
		MONST_INFO_X + 30, MONST_INFO_Y + r1 , LightGreen, -1, 
		(char *)"Work Ethic: %s", work_ethic_str[work_bits]
	);
    r1 += 20;
    textprintf_ex(
		active_page, &font_bold,
		MONST_INFO_X + 30, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"Qwerks: %s", showqwerks
	);
    r1 += 20;
    textprintf_ex(
		active_page, &font_bold,
		MONST_INFO_X + 30, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"Attitude: *****"
	);
    r1 += 20;
}


void trader_t::draw_menu_sell() {
	draw_menu_frame((char *)"Sell:");
    if (sell_out) {
	    sell_out->draw_item_list();
		draw_exit();
   	}
}

void trader_t::draw_menu_park() {
	draw_menu_frame((char *)"Park:");
	int r1 = 46;
	textprintf_ex(
		active_page, &font_bold,
		MONST_INFO_X + 30, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"Ticket Price:"
	);
	draw_menu_but( 150, r1, (char *)"+", MENU_TICKET_UP, 0);
	textprintf_ex(
		active_page, &font_bold,
		MONST_INFO_X + 190, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"$%4i", hud->ticket_price
	);
	draw_menu_but( 240, r1, (char *)"-", MENU_TICKET_DN, 0);
	r1 += 20;
	textprintf_ex(
		active_page, &font_bold,
		MONST_INFO_X + 30, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"Hunting:"
	);
	draw_menu_but( 110, r1, (char *)(hud->hunting_allowed ? "ENABLED" : "DISABLED"), MENU_HUNT_OK, 0);
	if (hud->hunting_allowed) {
		textprintf_ex(
			active_page, &font_bold,
			MONST_INFO_X + 210, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"Price:"
			);
		draw_menu_but( 250, r1, (char *)"+", MENU_HUNT_UP, 0);
		textprintf_ex(
			active_page, &font_bold,
			MONST_INFO_X + 290, MONST_INFO_Y + r1 , LightGreen, -1, (char *)"$%4i", hud->hunting_price
		);
		draw_menu_but( 340, r1, (char *)"-", MENU_HUNT_DN, 0);
	}
   	draw_exit();
}

void trader_t::draw_menu_graphs() {
	draw_menu_frame((char *)"Graphs:");
	graphs->set_draw_zone(MONST_INFO_X + 12, MONST_INFO_Y + 45, 
		MONST_INFO_R - MONST_INFO_X - 24, 
		MONST_INFO_B - MONST_INFO_Y - 74 
	);
	graphs->display_only_user();
	graphs->draw(active_page);
   	draw_exit();
}



monst_base *light_weight[] = {
&mb_roachy,&mb_quitofly,&mb_pygment,&mb_frogert,
&mb_furbit,&mb_shruver,&mb_radrunner,&mb_squeeker,
&mb_sneeker,&mb_malcumbit,&mb_bervul,&mb_cheech,
NULL
};

int light_weightA[] = {4,3,3,4, 5,5,5,1, 1,0,1,1,-1};
int light_weightR[] = {10,4,0,4, 15,5,5,1, 1,0,1,1,-1};


monst_base *mid_weight[] = {
&mb_mubark,&mb_molber,&mb_ackatoo,&mb_meeker,&mb_vaerix,
&mb_squarger, &mb_rexrex,&mb_throbid,&mb_toothworm,
&mb_krakin, &mb_stalker, &mb_slymer, &mb_mimmack,
NULL
};

int mid_weightA[] = {4,13,10,1,4, 3,5,5,5, 1,0,1,0,-1};
int mid_weightB[] = {1,3,14,2,8, 3,15,15,5, 1,1,1,1,-1};
int mid_weightC[] = {1,1,10,12,4, 1,12,10,2, 1,2,1,2,-1};


monst_base *heavy_weight[] = {
&mb_drexler,&mb_yazzert,&mb_dragoonigon,
&mb_mamothquito,&mb_uberworm,&mb_bigasaurus,
&mb_titurtilion,&mb_golgithan,NULL
};

int heavy_weightA[] = {20,3,1, 2,7,1, 1,0,-1};
int heavy_weightB[] = {7,13,4, 2,7,4, 4,0,-1};
int heavy_weightC[] = {2,5,10, 1,5,10, 10,0,-1};

monst_base *gladiators[] = {
&mb_orick, &mb_raider, &mb_sentry,
&mb_sentinel, &mb_hoveler, &mb_hunter,
&mb_stalker, &mb_cheech, &mb_crysillian,
NULL
};

int gladiatorsA[] = {20,4,2, 1,1,1, 0,1,0,-1};
int gladiatorsB[] = {20,14,5, 4,1,1, 2,1,0,-1};
int gladiatorsC[] = {18,14,5, 14,1,3, 2,0,0,-1};


int trader_t::build_battle_list() {
	if (active_battle) return 1;
	for (int i = battle_list.size() -1; i >= 0; i--) delete battle_list[i];
	battle_list.clear();
	for (int i = 0; i < 12; i++) if (hud->jail[i]->m) {
			battle_desc *bd; 
			monst_t *m = hud->jail[i]->m;
			if (m->check_mb_list(gladiators)) {
				if  (link->base == &mb_trainer) {
					battle_list.push_back( (bd = new battle_desc()) );
					bd->set(i, (char *)"Gladiator Dual", 
						(char *)"The Gladiator dual is a one on one match between humanoids.  "
						"Unlike other battles weapons are allowed (and encouraged).  This is a "
						"pride thing.  Its to the death with bragging rights as the only prize.", 
						0, 2, 0, 100000, gladiators, gladiatorsA
					);
					bd->set_mpf(0, award_dual, 0, 50);
					bd->set_mpf(1, award_dual_lost, 0, 0);				
				} else {
					battle_list.push_back( (bd = new battle_desc()) );
					bd->set(i, (char *)"Gladiator", 
						(char *)"Gladiator matches are open to all humanoids.  Unlike "
						"other battles weapons are allowed (and encouraged).  This is a "
						"last man standing free for all with up to ten competitors.", 
						0, 10, 800, 180000, gladiators, gladiatorsA
					);
					bd->set_mpf(0, award_0r, 4000, 150);
					bd->set_mpf(1, award_1r, 1200,  75);
					bd->set_mpf(2, award_2r, 1000,  50);
					bd->set_mpf(3, award_3r,    0,  15);
					if (m->award[award_0r] ||m->award[award_1r] ||m->award[award_2r] ) {
						battle_list.push_back( (bd = new battle_desc()) );
						bd->set(i, (char *)"Advanced Gladiator", 
							(char *)"Advanced Gladiator matches are open to all "
							"humanoids who have placed in a gladiator brawl.  Unlike "
							"other battles weapons are allowed (and encouraged).  This "
							"is a last man standing free for all with six skilled competitors.", 
							0, 6, 1100, 180000, gladiators, gladiatorsB
						);
						bd->set_mpf(0, award_0ar, 7000, 450);
						bd->set_mpf(1, award_1ar, 2000, 200);
						bd->set_mpf(2, award_2ar, 1500, 150);
						bd->set_mpf(3, award_3ar,    0,  45);
					}
					if (m->award[award_0ar] ) {						
						battle_list.push_back( (bd = new battle_desc()) );					
						bd->set(i, (char *)"Elite Gladiator", 
							(char *)"Elite Gladiator matches are restricted to humanoids "
							"who have won at the advanced level.  Unlike other battles "
							"weapons are allowed (and encouraged).  This is a last man "
							"standing free for all with six elite competitors.", 
							0, 6, 1100, 180000, gladiators, gladiatorsC
						);
						bd->set_mpf(0, award_0er, 9500, 1000);
						bd->set_mpf(1, award_1er, 2500,  700);
						bd->set_mpf(2, award_2er, 1100,  500);
						bd->set_mpf(3, award_3er,    0,  450);
					}
				}
			}
			if (hud->jail[i]->m->check_mb_list(heavy_weight)) {
				if  (link->base == &mb_trainer) {
					battle_list.push_back( (bd = new battle_desc()) );
					bd->set(i, (char *)"Heavy-Weight Dual", 
						(char *)"Duals are fought for fame and bragging rights as the "
						"only prizes.", 
						0, 2, 0, 130000, heavy_weight, heavy_weightB
					);
					bd->set_mpf(0, award_dual, 0, 100);
					bd->set_mpf(1, award_dual_lost, 0, 0);				
				} else {
					battle_list.push_back( (bd = new battle_desc()) );
					bd->set(i, (char *)"Heavy Monster Rumble", 
						(char *)"This rumble is a last monster standing free for all "
						"with up to six competitors in the heavy weight class.", 
						0, 6, 4000, 180000, heavy_weight, heavy_weightA
					);
					bd->set_mpf(0, award_0r, 12000, 300);
					bd->set_mpf(1, award_1r,  5000, 100);
					bd->set_mpf(2, award_2r,  2000,  50);
					bd->set_mpf(3, award_3r,     0,  45);
					if (m->award[award_0r] ||m->award[award_1r] ) {
						battle_list.push_back( (bd = new battle_desc()) );
						bd->set(i, (char *)"Super Heavy Rumble", 
							(char *)"Super Heavy Rumble matches are open to all "
							"Heavy-weight critters who have come out of the Heavy "
							"Monster in one piece and placed in the top two.  This "
							"is a last critter standing free for all with six skilled "
							"competitors.", 
							0, 6, 6000, 180000, heavy_weight, heavy_weightB
						);
						bd->set_mpf(0, award_0ar, 20000, 1000);
						bd->set_mpf(1, award_1ar,  8000,  500);
						bd->set_mpf(2, award_2ar,  7000,  250);
						bd->set_mpf(3, award_3ar,     0,  100);
					}
					if (m->award[award_0ar] ) {						
						battle_list.push_back( (bd = new battle_desc()) );				
						bd->set(i, (char *)"Monster Monster Asteroid Madness", 
							(char *)"This elite rumble matches the twelve best and the biggest "
							"monsters from around the galaxy against each other in an epic "
							"ground shaking brawl.  The event is extremely popular and many "
							"businesses are closed during one of these events.", 
							0, 12, 10000, 180000, heavy_weight, heavy_weightC
						);
						bd->set_mpf(0, award_0er, 30000, 2200);
						bd->set_mpf(1, award_1er, 20500, 1100);
						bd->set_mpf(2, award_2er, 15000, 1050);
						bd->set_mpf(3, award_3er, 12000, 1125);
					}
					
					if (m->award[award_0r] ||m->award[award_0ar] ) {
						battle_list.push_back( (bd = new battle_desc()) );
						bd->set(i, (char *)"Heavy-Weight Tournament", 
							(char *)"The heavy-weight tournament is by invitation only and require "
							"a win during a rumble to qualify.  It is single elimination with an "
							"eight monster bracket.", 
							1, 8, 5000,  180000, heavy_weight, heavy_weightB
						);
						bd->set_mpf(0, award_0at, 18000,  900);
						bd->set_mpf(1, award_1at,  8000,  500);
						bd->set_mpf(2, award_2at,  7000,  250);
						bd->set_mpf(3, award_3at,  6000,  100);
					}
					if (m->award[award_0at] ) {
						battle_list.push_back( (bd = new battle_desc()) );				
						bd->set(i, (char *)"Galactic Heavy-Weight Tournament", 
							(char *)"The Galactic Heavy-Weight Tournament is by invitation only and "
							"require a win in a heavy-weight tournament to qualify.  This single "
							"elimination sixteen monster is considered my many as the true measure of "
							"who is indeed The Mightiest In The Galaxy.", 
							1, 16, 9000,  180000, heavy_weight, heavy_weightC
						);
						bd->set_mpf(0, award_0et, 32000, 1800);
						bd->set_mpf(1, award_1et, 20000, 1300);
						bd->set_mpf(2, award_2et, 15000, 1250);
						bd->set_mpf(3, award_3et,  9000,  900);
					}
				}
			}

			if (hud->jail[i]->m->check_mb_list(mid_weight)) {
				if  (link->base == &mb_trainer) {
					battle_list.push_back( (bd = new battle_desc()) );
					
					bd->set(i, (char *)"Mid-Weight Dual", 
						(char *)"Duals are fought for fame and bragging rights as the only prizes.",
						0, 2, 0, 180000, mid_weight, mid_weightA
					);
					bd->set_mpf(0, award_dual, 0, 70);
					bd->set_mpf(1, award_dual_lost, 0, 0);				
				} else {								
					battle_list.push_back( (bd = new battle_desc()) );
					bd->set(i, (char *)"Rumble", 
						(char *)"This rumble is a last man standing free for all with up to "
						"ten competitors in the middle weight class.", 
						0, 10, 800, 180000, mid_weight, mid_weightA
					);
					bd->set_mpf(0, award_0ar,  3000,  150);
					bd->set_mpf(1, award_1ar,  1200,   70);
					bd->set_mpf(2, award_2ar,  1000,   50);
					bd->set_mpf(3, award_3ar,     0,   25);					
					
					if (m->award[award_0at] || m->award[award_0ar] ) {
						battle_list.push_back( (bd = new battle_desc()) );
						bd->set(i, (char *)"Advanced Rumble", 
							(char *)"Advanced Rumble matches are open to all mid-weight critters "
							"who have come out of the rumble in one piece.  This is a last critter "
							"standing free for all with six skilled monsters.", 
							0, 6, 1100, 180000, mid_weight, mid_weightB
						);
						bd->set_mpf(0, award_0ar,  7000,  300);
						bd->set_mpf(1, award_1ar,  4000,  200);
						bd->set_mpf(2, award_2ar,  2000,   75);
						bd->set_mpf(3, award_3ar,     0,   50);					
					}
					
					
					if (m->award[award_0at] || m->award[award_0ar] ) {
						battle_list.push_back( (bd = new battle_desc()) );				
						bd->set(i, (char *)"Elite Rumble", 
							(char *)"Elite Rumble matches are restricted to who have won tournaments "
							"or rumbles at the advanced level.  This is a last man standing free "
							"for all with six elite competitors.", 
							0, 6, 2100, 180000, mid_weight, mid_weightC
						);
						bd->set_mpf(0, award_0er, 15000,  750);
						bd->set_mpf(1, award_1er,  8000,  500);
						bd->set_mpf(2, award_2er,  5000,  250);
						bd->set_mpf(3, award_3er,  4000,  200);					
					}
					if (m->award[award_0r] ) {
						battle_list.push_back( (bd = new battle_desc()) );
						bd->set(i, (char *)"Mid-Weight Tournament", 
							(char *)"Tournaments are by invitation only and require a win during a "
							"rumble to qualify.  It is single elimination with a 16 monster bracket.", 
							1, 16, 1100,  180000, mid_weight, mid_weightB
						);
						bd->set_mpf(0, award_0at,  8000, 500);
						bd->set_mpf(1, award_1at,  5000, 300);
						bd->set_mpf(2, award_2at,  4000, 250);
						bd->set_mpf(3, award_3at,  2000, 100);
					}
					if (m->award[award_0at] || m->award[award_0ar] ) {
						battle_list.push_back( (bd = new battle_desc()) );				
						bd->set(i, (char *)"Galactic Mid-Weight Tournament", 
							(char *)"Tournaments are by invitation only and require a win during "
							"a lesser tournament or an elite rumble to qualify.  It is single "
							"elimination with a 32 monster bracket.", 
							1, 32, 1100, 180000, mid_weight, mid_weightC
						);
						bd->set_mpf(0, award_0et, 15000, 1000);
						bd->set_mpf(1, award_1et,  8000,  500);
						bd->set_mpf(2, award_2et,  5000,  250);
						bd->set_mpf(3, award_3et,  4000,  200);
					}
				}
			}
			if (hud->jail[i]->m->check_mb_list(light_weight)) {
				if  (link->base == &mb_trainer) {
					battle_list.push_back( (bd = new battle_desc()) );
					bd->set(i, (char *)"Light-Weight Duel", 
						(char *)"Ok these are mostly silly, but the little guys want to fight too.  ", 
						0, 2, 500, 100000, light_weight, light_weightA
					);
					bd->set_mpf(0, award_dual, 0, 20);
					bd->set_mpf(1, award_dual_lost, 0, 0);				
				} else {
					battle_list.push_back( (bd = new battle_desc()) );
					bd->set(i, (char *)"Light-Weight Rumble", 
						(char *)"Ok these are mostly silly, but the little guys want to fight "
						"too.  This is a last critter standing free for all with up to fifteen "
						"competitors.", 0, 15, 500, 180000, light_weight, light_weightA
					);
					bd->set_mpf(0, -1, 2000, 150);
					battle_list.push_back( (bd = new battle_desc()) );
					bd->set(i, (char *)"Running Man", 
						(char *)"Running Man is a spectator sport of the worst kind.  The "
						"arena is filled up with small animals then a few angry predators are "
						"thrown in the middle.  The pot is split among any who not been "
						"eaten after 90 seconds or the last survivor.", 
						3, 20, 550, 180000, light_weight, light_weightR
					);
					bd->set_mpf(0, award_0rm, 8000, 300); // split amoung survivors
					battle_list.push_back( (bd = new battle_desc()) );				
					bd->set(i, (char *)"Race", 
						(char *)"Five times around the track.  Prizes given to the top three.", 
						2, 9, 2800, 180000, light_weight, light_weightR
					);
					bd->set_mpf(0, award_0race, 5500, 150);
					bd->set_mpf(1, award_1race, 4500, 100);
					bd->set_mpf(2, award_2race, 3500,  50);
					bd->set_mpf(3, award_3race,    0,  25);
				}
			}
	}
	return battle_list.size();
}

void trader_t::draw_menu_fight() {
	draw_menu_frame((char *)"Fight:");
   	if (!build_battle_list() || (top_buy_item >= (int)(battle_list.size())) ) {
   	    textout_ex(
			active_page, &font_bold_plus, (char *)"<NOBODY AVAILABLE>",
			MONST_INFO_X + 100, MONST_INFO_Y + 140, LightGreen, -1
		);
		top_buy_item = 0; 
   	} else {
		int e = MIN((int)(battle_list.size()), top_buy_item + SHOW_ITEMS);
		for (int i = top_buy_item; i < e; i++) {
			int ty = MONST_INFO_Y + 48 + 20 * (i - top_buy_item);
			battle_desc *k = battle_list[i];
			int xx = MONST_INFO_X + 5;
			int yy = ty - 4;
			int ex = MONST_INFO_R - 16;
			int ey = ty + 20;
			if (
				mouse_x >= xx && mouse_x <= ex &&
				mouse_y >= yy && mouse_y <= ey
			) {
				input_map.menu_hover = i + MENU_BUY0;
			}
			if (i == focus) { rectfill(active_page, xx,yy,ex,ey, Green);  }
			textout_ex(
				active_page, &font_bold, k->short_desc,
				MONST_INFO_X + 15, ty, LightGreen, -1
			);	
			if (hud->jail[k->player_cage]->m) textprintf_ex(
				active_page, &font_bold,
				MONST_INFO_X + 200, ty, LightGreen, -1, 
				(char *)"%2i: %s", k->player_cage, hud->jail[k->player_cage]->m->name
			); else  textprintf_ex(
				active_page, &font_bold,
				MONST_INFO_X + 200, ty, LightGreen, -1, 
				(char *)"%2i: FIGHT!", k->player_cage
			); 
			
			textprintf_ex(
				active_page, &font_bold,
				MONST_INFO_X + 330, ty, LightGreen, -1, (char *)"$%4i",
				(int)(k->entryfee)
			);
		}		
   	}
   	draw_exit();
}



void trader_t::draw_menu_cryo() {
	draw_menu_frame((char *)"Cryogenics:");
   	draw_exit();
}

void trader_t::draw_menu_lab() {
	draw_menu_frame((char *)"Lab:");
   	draw_exit();
}

static char *hosstr_list[] = {
	(char *)"Extremely Hostile",    // -5
	(char *)"Very Hostile",         // -4
	(char *)"Aggressively Hostile", // -3
	(char *)"Hostile",    // -2
	(char *)"Unfriendly", // -1
	(char *)"Neutral",   // 0
	(char *)"Somewhat Friendly", //1
	(char *)"Friendly",          //2
	(char *)"Allied",             //3
	(char *)"Strongly Allied",   //4
	(char *)"Very Strongly Allied" //5
};

void trader_t::draw_menu_alignment() {
	draw_menu_frame((char *)"Alignment:");
	int yy = MONST_INFO_Y + 48;
	for (int i= org_ast0 + 1; i < org_last; i++) if (org_list[i].known >= 1 || arg_cheater) {
		textprintf_ex(
			active_page, &font_tiny,
			MONST_INFO_X+20, yy, LightGreen, -1, (char *)"%s",
			org_list[i].nam
                );
		if (arg_cheater) textprintf_right_ex(
			active_page, &font_tiny,
			MONST_INFO_X + 360, yy, (org_list[i].known >= 1 ? LightGreen : Green), -1, (char *)"%s (%i: %i)",
			hosstr_list[MID(0,org_list[i].al+5,10)], org_list[i].rawal, org_list[i].al
		); else textprintf_right_ex(
			active_page, &font_tiny,
			MONST_INFO_X + 360, yy, LightGreen, -1, (char *)"%s (%i)",
			hosstr_list[MID(0,org_list[i].al+5,10)] , org_list[i].al
		);
		yy += 11;
   	}
   	draw_exit();
}

void trader_t::draw_menu_scan() {
	draw_menu_frame((char *)"Scanner:");
        int yy = MONST_INFO_Y + 48, xx = MONST_INFO_X + 6, lyy = 12, ind = xx + 17;
        FONT *f = &font_tiny;
        static char *scanname[] = {(char *)"Radio Marker", (char *)"Organics", (char *)"Delta Wave", (char *)"Q-Particle", (char *)"Epsilon Hi"};
        static char *infolvl[] = {(char *)"", (char *)"minor", (char *)"some", (char *)"extensive", (char *)"full" };
        for (int c = 1; c < 5; c++) {
        	textprintf_ex( active_page, f, xx,yy, LightGreen, -1, (char *)"Active %s : %02i", scanname[c], ast->scanner_count[c]);
        	yy += lyy;
        	switch (c) {
                case 0: break;
                case 1: {
                  	int z = MID(0, ast->scanner_count[c], 4);
                  	if (z) {
        			textprintf_ex( active_page, f, ind,yy, Green, -1, (char *)"Organics scaning yeilds %s info.", infolvl[z]); yy += lyy;
                     	}
                  }
                break;
                case 2: {
                  	int z = MID(0, ast->scanner_count[c], 5);
                  	if (z) {
        			textprintf_ex( active_page, f, ind,yy, Green, -1, (char *)"Delta waves track %i asteroids.", 200 * z); yy += lyy;

                     	}
                  }
                break;
                case 3: {
                  	int z = MID(0, ast->scanner_count[c], 4);
                  	if (z) {
        			textprintf_ex( active_page, f, ind,yy, Green, -1, (char *)"Epsilon Hi scaning yeilds %s info about health of your monsters.", infolvl[z]); yy += lyy;
        			if (remote_ast) {
             				int plant = 0, qmin = 0, mina[NUM_INV_MINERAL], buildings = 0, ani[4] = {0,0,0,0}, inorganics, infected;
             				memset(mina, 0, sizeof(mina));
             				for (list<monst_t *>::iterator mon = remote_ast->m.begin(); mon != remote_ast->m.end(); mon++ ) {
                        			monst_t *m = *mon;
                        			if (m->isa_plant()) {
                                                	plant++;
                             			} else if (m->isa_building()) {
                                                	buildings++;
                                  		} else if (m->flags | AF_INORGANIC) {
                                                        if (m->base->inv_link) {
                                                        	if (m->base->inv_link->typ == inv_mineral) {
                                                           		qmin++;
                                                                	mina[m->base->inv_link->lvl]++;
                                                           	} else inorganics++;
                                                        } else inorganics++;
                                      		} else {
                                                	int srt = (
                                        	  		m->size < mb_furbit.size ? 0 :
                                        	  		m->size < mb_mubark.size ? 1 :
                                        	  		m->size < mb_drexler.size ? 2 : 3
                                	  		);
                                	  		ani[srt]++;
                                  		}
                			}
             				textprintf_ex( active_page, f, ind,yy, Green, -1, (char *)"Buildings:  %i", buildings); yy += lyy;
             				textprintf_ex( active_page, f, ind,yy, Green, -1, (char *)"Plants:  %i", plant);yy += lyy;
             				textprintf_ex( active_page, f, ind,yy, Green, -1, (char *)"Animals:   tny %i   sml %i   med %i  lrg %i",ani[0],ani[1],ani[2],ani[3]); yy += lyy;
                                        textprintf_ex( active_page, f, ind,yy, Green, -1, (char *)"Inorganics:   red %i   yel %i   pur %i  grn %i  bio %i  othr %i",
                                        	mina[1],mina[2],mina[3],mina[4],mina[5], inorganics
                                        ); yy += lyy;

             			}
                     	}
                  }
                break;
                case 4: {
                  	int z = MID(0, ast->scanner_count[c], 4);
                  	if (z) {
        			textprintf_ex( active_page, f, ind,yy, Green, -1, (char *)"Q-particle scaning yeilds %s info about the remote asteroid.", infolvl[z]); yy += lyy;
        			if (remote_ast) {
             				textprintf_ex( active_page, f, ind,yy, Green, -1, (char *)"[INFCTIONS]"); yy += lyy;
             				textprintf_ex( active_page, f, ind,yy, Green, -1, (char *)"[SICK]"); yy += lyy;
             				textprintf_ex( active_page, f, ind,yy, Green, -1, (char *)"[STARVING]"); yy += lyy;
             			}
                     	}
                  }
                break;
           	}
        }
   	draw_exit();
   	draw_powertog();
}

void trader_t::draw_menu_target(int menu_style) {
	if (menu_style == 0) draw_menu_frame((char *)"Targets:");
    if (menu_style == 1) draw_menu_frame((char *)"Hunter Targets:");
    if (menu_style == 2) draw_menu_frame((char *)"Critters:");
    int sqw = 120, sqh = 12, l = MONST_INFO_X + 20, t = MONST_INFO_Y + 55, r = 16, c = 3;
    FONT *f = &font_tiny;
	int n = 1;
	monst_base **mb = mb_list_all;
	for (int x=0; x < c; x++ ) for (int y = 0; y < r; y++) {
		int gray_out = ((*mb)->ai_func == ai_plant && menu_style == 1);
		int ox = x * sqw + l;
		int oy = y * sqh + t;
		int c2x = ox + 20;
		int targ = 0;
		if (menu_style == 0) targ = (*mb)->stat_valid_target & TARGET_AST0;
		if (menu_style == 1) targ = (*mb)->stat_valid_target & TARGET_HUNTER;
		int color = (gray_out ? DarkGray :targ ? LightGreen : Green);
		textprintf_ex( active_page, f, ox,oy, color, -1, (char *)"%02i:", n);
		if ((*mb)->stat_inpark || (*mb)->stat_didscan) {
			char out[200];
			snprintf(out, 200,  "%s %s%s",
				(targ ? ">" : " "),
				(*mb)->name,
				(targ ? " <" : "")
			);
			int ex = c2x + text_length(f, out);
			textprintf_ex(active_page, f, c2x,oy, color, -1, out);
			if ( 
				!gray_out &&
				mouse_x >= ox &&
				mouse_x <= ex &&
				mouse_y >= oy &&
				mouse_y < oy + sqh
			) {
				hline(active_page, ox, oy + 11, ex, LightGreen);
				input_map.menu_hover = MENU_TARGET0 + (n - 1);
			}
		} else textprintf_ex(
			active_page, f,	c2x,oy, color, -1, (char *)"??? UNK ???"
		);
		if (*mb == &mb_golgithan) break;
		n++;   mb++;
   	}
   	draw_exit();
   	draw_powertog();
   	if (link->base == &mb_guntower) draw_upgrade();
}

void trader_t::draw_menu_payroll() {
	draw_menu_frame((char *)"Payroll:");
   	draw_exit();
	draw_powertog();
}

void trader_t::draw_menu_encl(int menu_num) {
	draw_menu_frame((char *)"Enclosure Control:");
   	draw_exit();
	draw_powertog();
}

void trader_t::draw_menu_jump(int menu_num) {
/*
jumpgate 0 (node):
   forced random autojump.  no manual anything. no marked.
jg 1
   selectable autojump. manual open only.  jump marked.
jg 2
   selectable autojump. manual open and close.  jump marked.
jg 3
   selectable autojump. manual open and close.  jump marked.
jg 4
   selectable autojump. manual open and close.  jump any.
*/
	draw_menu_frame((char *)"Jump To:");
        int lvl = ( link->base->inv_link ? link->base->inv_link->lvl : 0);
        int sqw = 18, sqh = 11, l = MONST_INFO_X + 22, t = MONST_INFO_Y + 75, r = 5 * 3, c = 20; // 40 * 25 = 1000  // 300,300,100
	int xfar = c * sqw + l;
	int yfar = r * sqh + t;
        int num = jg_first;
        int n = num;
        for (int y = 0;y <= r; y++) hline(active_page, l - (y % 5 == 0 ? 4 : 0), y * sqh + t, xfar, (y % 5 == 0 ? LightGreen : Green) );
        for (int x = 0;x <= c; x++) vline(active_page, x * sqw + l, t, yfar, Green);
        int bly = 50, blx = 22;
	if (lvl > 0) blx = draw_mini_but( blx, bly, (char *)"Autojump", MENU_TOGAUTOJUMP,  jg_autojump );
        if (remote_ast) {
        	if (lvl > 1) blx = draw_mini_but( blx, bly, (char *)"Force Closed", MENU_JUMPCLOSE,  0 );
        } else {
        	if (lvl > 0) blx = draw_mini_but( blx, bly, (char *)"Manual Open", MENU_JUMPOPEN,  0 );
        }
        if (remote_ast || jg_respawn_time > ms_count) {
		int tim = (remote_ast ? remote_ast->exit_time : jg_respawn_time);
        	int T = tim - ms_count;
        	int m = T / 60000; int s = T / 1000 - (m * 60);  int tenth = (T % 100) / 10;
        	int xx = MONST_INFO_R - 10;
        	int yy = MONST_INFO_Y + 50;
        	if (T > 20000) {
			textprintf_right_ex(active_page, &font_lcd, xx, yy, LightGreen, -1 , (char *)"%2i:%02i",m,s);
		} else {
			textprintf_right_ex(active_page, &font_lcd, xx, yy, LightGreen, -1 , (char *)"%02i.%1i",s,tenth);
		}
                textprintf_right_ex(active_page, &font_tiny, xx, yy - 12, LightGreen, -1 , ( remote_ast ? "Remote Asteroid"  : "Recharge Time") );
        }
        if (lvl > 0 && !remote_ast) {
        	for (int y = 0;y <3; y++) textprintf_ex(
        		active_page, &font_tiny, l - 8, y * 5 * sqh + t + (sqh * 2), LightGreen, -1, (char *)"%i", jg_first / 100 + y
		);
        	if (home_ast) for (int y = 0;y < r; y++) for (int x = 0;x < c; x++) {
          		int mode = (n > 0 && n < 1000 ? hud->marked[n] : 0);
          		if  (mode != 0) rectfill(active_page,
          			x * sqw + l + 1,  y * sqh + t + 1,
          			(x + 1) * sqw + l - 2,
          			(y + 1) * sqh + t - 2, (mode == 1 ? Green : LightGreen )
  			);
  			if (n < home_ast->scanner_count[2] * 200 && n != 0) {
        			textprintf_ex(active_page, &font_micro, x * sqw + l + 2,  y * sqh + t, (mode ? Black : LightGreen), -1, (char *)"%03i", n );
          		} else {
                		textprintf_ex(active_page, &font_micro, x * sqw + l + 2,  y * sqh + t, (mode ? Black : LightGreen), -1, (char *)"***" );
	        	}
			n++;
        	}
        	int ovr = -1;
        	if (mouse_x >= l && mouse_y >= t && mouse_x < xfar && mouse_y < yfar ) {
        		ovr = num + (mouse_x - l) / sqw + (mouse_y - t) / sqh * c;
        	}
        	if (ovr > -1) {
        		int ox = l + ((ovr - num) % c) * sqw - 8;
        		int oy = t + ((ovr - num) / c) * sqh - 6;
	                rectfill(active_page, ox, oy, ox + sqw + 6, oy + sqh + 4, Black);
        	        rect(active_page, ox, oy, ox + sqw + 6, oy + sqh + 4, LightGreen);
                	textprintf_ex(active_page, &font_bold, ox + 1, oy + 1, LightGreen, -1, (char *)"%03i", ovr );
	                input_map.menu_hover = ovr + MENU_JUMP0;
        	}
        }
   	draw_exit();
   	draw_powertog();
   	draw_upgrade();
}

void trader_t::draw_menu_power(){
	draw_menu_frame((char *)"Power Grid:");

        if (link && link->my_ast) {
        	int w = MONST_INFO_R - MONST_INFO_X - 60;
        	int h = MONST_INFO_B - MONST_INFO_Y - 60;
        	int t = MONST_INFO_Y + 30;
        	int l = MONST_INFO_X + 30;
        	float xc = (float)(w)/MAX(link->my_ast->pw, 1);
        	float yc = (float)(h)/MAX(link->my_ast->ph, 1);
        	static int psl[] = {SPR_GRNOFF, SPR_GRNNOPOW, SPR_GRNON};
        	int n = 0;
        	for (list<monst_t *>::iterator mon = link->my_ast->m.begin(); mon != link->my_ast->m.end(); mon++ ) {
                        monst_t *m = *mon;
                        if (m->org_idx == link->org_idx) {
                        	char ltr[3] = "VV";
                        	if (!(m->flags & AF_INORGANIC)) continue;
                        	
                        	if (m->ai_func == ai_building) { ltr[0] = ltr[1] = 'B'; }
                        	if (m->base->inv_link) {
                           		switch (m->base->inv_link->typ) {
                               		case inv_tower : ltr[0] = 'T'; break;
                               		case inv_power : ltr[0] = 'G'; break;
                               		case inv_encl  : ltr[0] = 'E'; break;
                               		case inv_stabilizer : ltr[0] = 'J'; break;
                               		case inv_scanner : ltr[0] = 'S'; break;
                               		}
                               		ltr[1] = (m->base->inv_link->lvl < 0 ? ltr[0] :
                               			m->base->inv_link->lvl > 9 ? '+':
                               			'0' + (char) (m->base->inv_link->lvl)
                       			);
       		       		}
       		       		int xx = (int)(m->x * xc) + l;
                                int yy = (int)(m->y * yc) + t;
                        	if (m->pow_drain > 0) {
				        int sprc = psl[MID(0,m->on + m->pow_on,2)];
					if (sprc != SPR_GRNNOPOW || ms_count % 1000 < 500) {
   						draw_sprite(active_page, (BITMAP *)(sprites[sprc].dat),	xx-20, yy);
					}
                                 	textprintf_ex(active_page, &font_bold, xx, yy, Black, Green, ltr);
                                 	if (
               					mouse_x >= xx - 20 &&
               					mouse_x <= xx + 10 &&
			               		mouse_y >= yy && mouse_y <= yy + 20

        				) {
                				rect(active_page, xx-22, yy-2, xx+2, yy+22, LightGreen);
                				input_map.menu_hover = MENU_POWTOG0;
                				watch_monst = m;
        				}
        				textprintf_ex(
						active_page, &font_micro, xx, yy + 12, LightGreen, -1, (char *)"%i",
						m->pow_drain
					);
                           	} else {
                              		textprintf_ex(active_page, &font_bold, xx - 8, yy, Black, Green, ltr);
                                	if (m->pow_drain < 0) textprintf_ex(
						active_page, &font_micro, xx - 8, yy + 12, LightGreen, -1, (char *)"[g:%i]",
					 	-m->pow_drain
					);
				}
                        }
                        n++;
           	}
		textprintf_ex(
			active_page, &font_bold, l, t + h + 8, LightGreen, -1, (char *)"[global: %i/%i]",
			link->my_ast->orgs_powerr[link->org_idx],
                	link->my_ast->orgs_powerg[link->org_idx]
		);
		//rect(active_page,l,t,l+w,t+h,Blue);
    	}
   	draw_exit();
}

int trader_t::jg_open(int n) {
  	if (n = -1) n = irand(999) + 1;
  	int ret = (link && link->base->inv_link && link->base->inv_link->typ == inv_stabilizer &&  jg_respawn_time < ms_count && !remote_ast);
       	if (ret) {
               	remote_ast = new asteroid_t(n);
       		hud->short_news((char *)"Jumpgate opened to nearby asteroid");
	}
	return ret;
}

void trader_t::say(char *say_phrase, int mood, int syl) {
  	if (!say_phrase) return;
	free(said);
	char scrap[8000];
	int cs = 0;
	char *s = say_phrase;
	char *ops = NULL;
	char *oo = scrap;
	int explicit_syl = 0;
	if (*s=='@' || *s=='#') {
		ops = s;
		while (*s && *s != ' ') s++;
	}
	if (*s=='[' ) {
		ops = s;
		while (*s && *s != ']') s++;
	}
	char *opsend = s;
	int found_the_ogg = 0;
	int sound_in_ms = 0;
		
	if (ops && sounds && (ops[0]=='[' || ops[0]=='#')) {
		ops++;
		char namlu[256], *nlo = namlu;
		for (;*ops && *ops != ' ' && ops != opsend; ops++, nlo++) *nlo = *ops;
		nlo[0] = '_';   nlo[1] = 'O';   nlo[2] = 'G';   nlo[3] = 'G';   nlo[4] = 0;   
		// the names are sorted, so this could be improved (but it isn't called very often)
		for (int c = 0; c < SND_COUNT; c++) { 
			if (strcmp(namlu,  get_datafile_property(&sounds[c], DAT_ID('N','A','M','E'))) == 0) {
				do_sound(c, (int)(ship ? ship->x : 0), (int)(ship ? ship->y : 0), 255);
				#ifdef USE_ALOGG 
					int channels,freq;
					#ifdef ALOGG_USE_TREMOR
						ogg_int64_t length;
					#else
						float length;
					#endif
					datogg_object *ovd;
					
					if (sounds[c].type==DAT_OGG_VORBIS || sounds[c].type == DAT_ID('O','G','G',' ' )) {
						ovd = (datogg_object*)sounds[c].dat;
						if (ovd->data!=NULL) {
							if (aloggint_get_info(ovd,&channels,&freq,&length)) {
								#ifdef ALOGG_USE_TREMOR
									sound_in_ms = length;
								#else
									sound_in_ms = (int)(1000. * length);
								#endif
							}
						}
					}
				#endif	
				if (sounds[c].type ==DAT_SAMPLE) {
					SAMPLE *smp = (SAMPLE *)sounds[c].dat;
					sound_in_ms = (smp->freq > 0 ? (1000 * smp->len / smp->freq) : 0);
				}
				found_the_ogg = 1;break;
			}
		}
		if (!found_the_ogg) {
			char warnstr[256]; sprintf(warnstr, "Missing speech entry: \"%s\". Quit?", namlu);
			tiredofplaying = askyn(warnstr);
		}
	}
	if (*s && ops) ++s;
	char *narg = s;
	for (;*s; s++) switch (*s) {
		case '.':
		case ';':
		case ',':
		case ' ': *oo = *s; ++oo; while (s[1] == ' ') s++;  break;
		case '$':
		case '0': case '1':	case '2': case '3':
		case '4': case '5':	case '6':
		case '7': case '8':	case '9': {
			int dollars = (*s == '$'); if (dollars) s++;
			int number = MIN(atoi(s), 999999999);
			if (number == 0) {
				oo += sprintf(oo,"ze`ro` ");
			} else {
				char *ttwenty[20] = {
					(char *)"", (char *)"one` ", (char *)"two` ", (char *)"three` ", 
					(char *)"four` ", (char *)"five` ", (char *)"six` ", (char *)"sev`en` ",
					(char *)"eight` ", (char *)"nine` ", (char *)"ten` ", (char *)"e`lev`en` ",
					(char *) "twelve` ", (char *)"thir`teen` ",	(char *)"four`teen` ", 
					(char *)"fif`teen` ",(char *)"six`teen` ", (char *)"sev`en`teen` ",	
					(char *)"eight`teen` ",	(char *)"nine`teen` "
				};
				char *ttens[10] = {
					(char *)"", (char *)"", (char *)"twen`ty` ", (char *)"thir`ty` ", 
					(char *)"four`ty` ", (char *)"fiv`ty` ", (char *)"six`ty` ", 
					(char *)"sev`en`ty` ",	(char *)"eigh`ty` ", (char *)"nine`ty` ", 
				};
				int twentyfrac = (number % 100); if (twentyfrac >= 20) twentyfrac = 0;
				int tens = (number / 10) % 10;
				int hds = (number / 100) % 10;
				int bigthou = (number / 1000) % 100;
				int twentythoufrac = (bigthou < 20 ? bigthou : 0);
				int tensthou = (bigthou / 10) % 10;
				int hdsthou = (bigthou / 100);
				int bigmil = (number / 1000) % 100;
				int twentymilfrac = (bigmil < 20 ? bigmil : 0);
				int tensmil = (bigmil / 10) % 10;
				int hdsmil = (bigmil / 100);
				
				if (bigmil) {
					if (hdsmil) oo += sprintf(oo, "%shun`dred` ", ttwenty[hdsmil]);
					oo += sprintf(oo, "%s", ttens[tensmil]);
					oo += sprintf(oo, "%smil`lion` ", ttwenty[twentymilfrac]);
				}
				if (bigthou) {
					if (hdsthou) oo += sprintf(oo, "%shun`dred` ", ttwenty[hdsthou]);
					oo += sprintf(oo, "%s", ttens[tensthou]);
					oo += sprintf(oo, "%sthou`sand` ", ttwenty[twentythoufrac]);
				}
				if (hds) oo += sprintf(oo, "%shun`dred` ", ttwenty[hds]);
				oo += sprintf(oo, "%s", ttens[tens]);
				oo += sprintf(oo, "%s", ttwenty[twentyfrac]);
			}
			if (dollars) oo += sprintf(oo, "bucks` ");
		} break;
		default :
			 	
		  {int lc = 0;
		    char word[512] ,*w;
		    for (w = word; *s == '`' || *s == '\'' || (tolower(*s) >= 'a' && tolower(*s) <= 'z');) {
		    	*w = tolower(*s); w++; s++;
		    }
		    --s; *w = 0; ++w; *w = 0; // simplify lookahead.
		    if (word[strlen(word)] == '`') {
		    	oo += sprintf(oo, "%s", word); explicit_syl = 1;
		    } else if (word[0]) {
				*oo = '['; *oo++;
				int vf = 0, ef = 0;
				for (w = word; *w; w++) {
					*oo = *w; oo++; 
					if (
						(*w == 'a' || *w == 'e' || *w == 'i' || 
						*w == 'o' ||*w == 'u' || (*w == 'y' &&  w != word)  
						)  
					) {
						if (ef && *w == 'e') {
							if (w[1]) {*oo = '`'; oo++;} 
							ef = 0;
						} else if (w[2] == 'e') ef = 1;		
						else vf = 1;
					} else if (vf) {
						if (					
							w[1] == 'a' || w[1] == 'e' || w[1] == 'i' || 
							w[1] == 'o' || w[1] == 'u' || w[1] == 'y' || 
							w[2] == 'a' || w[2] == 'e' || w[2] == 'i' || 
							w[2] == 'o' || w[2] == 'u' || w[2] == 'y'  
						) {*oo = '`'; oo++; vf = 0;}					
					}
				}		    
				*oo = '`'; *oo++;
				*oo = ']'; *oo++;
			} else s++;			
		  }		
	}
	*oo = 0;
	int ttime = 0;
	
	char puppet_code[2048];
	char *pc = puppet_code;
	int sylix = 0;
	pc += sprintf(pc,"b10 ");// brief pause to sync correctly 
	for (char *wt = scrap; *wt; wt++) switch (*wt) {
	case '`': 
		switch (sylix % 16) {
		case 0:		pc += sprintf(pc,"Ac40d60e80d80");  break;
		case 1:		pc += sprintf(pc,"Bc40d90c80");  break;
		case 2:		pc += sprintf(pc,"Cb30c60d80");  break;
		case 3:		pc += sprintf(pc,"Dc50d40e60d30");  break;
		case 4:		pc += sprintf(pc,"Fc40d60e80d80");  break;
		case 5:		pc += sprintf(pc,"Gc40d90c80");  break;
		case 6:		pc += sprintf(pc,"Hb30c60d80");  break;
		case 7:		pc += sprintf(pc,"Ic50d40e60d30");  break;
		case 8:		pc += sprintf(pc,"Jc40d60e80d80");  break;
		case 9:		pc += sprintf(pc,"Kc40d90c80");  break;
		case 10:	pc += sprintf(pc,"Lb30c60d80");  break;
		case 11:	pc += sprintf(pc,"Mc50d40e60d30");  break;
		case 12:	pc += sprintf(pc,"Nc40d60e80d80");  break;
		case 13:	pc += sprintf(pc,"Oc40d90c80");  break;
		case 14:	pc += sprintf(pc,"Pb30c60d80");  break;
		case 15:	pc += sprintf(pc,"Qc50d40e60d30");  break;
		}		
	case ' ': pc += sprintf(pc,"c30b20 ");  break;
	case ',':
	case ';': pc += sprintf(pc,"c60b20, ");  break;
	case '.': pc += sprintf(pc,"b80. ");  break;
	default:  sylix = *wt;
	
	} 
	*pc = 0;
	if (raw_puppet) free(raw_puppet); 
	raw_puppet = (int *)calloc(strlen(puppet_code) + 2, sizeof( int) ); // worst case
	raw_puppet_idx = 0;
	int anii = 0;
	said_done = ms_count;
	int noise_jump = -1;
	for (char *wt = puppet_code; *wt;) if (*wt >= 'a' && *wt <= 'z') {
		char vv = *wt - 'a'; wt++;
		said_done +=  strtol(wt,&wt,10);
		raw_puppet[anii++] = vv;
		raw_puppet[anii++] = said_done;		
		raw_puppet[anii++] = noise_jump; noise_jump = -1;		
	} else if (*wt >= 'A' && *wt <= 'Z') { 
		noise_jump = *wt; wt++;
		noise_jump -= 'A';
		noise_jump *= 500; 
	} else wt++;
	raw_puppet[anii++] = 0;	
	raw_puppet[anii++] = 0;	
	raw_puppet[anii++] = 0;	
	raw_puppet_idx = 0;
	if (sound_in_ms) { // rescale raw_puppet data to the timelength of the ogg file
		int talk_begin = raw_puppet[1];
		int talk_end = said_done;
		int msin = talk_end - talk_begin;
		if (msin) for (int c = 1; raw_puppet[c]; c += 2) raw_puppet[c] = sound_in_ms * (raw_puppet[c] - talk_begin) / msin + talk_begin; 
	}
	said_done += 650;
	
	char *sp = scrap;
	//said = strdup(scrap); // debug syl output
	
	for (;*narg; ++narg) if (*narg != '`') {
		*sp = *narg; ++sp;
	}
	*sp = 0;
	said = strdup(scrap); // cleaned output.
	
	//said = strdup(puppet_code);
	said_mood = mood;
	if (gibberish_idx >= 0 && sounds) {
		
		if (sounds[gibberish_idx].type == DAT_ID('O','G','G',' ') || sounds[gibberish_idx].type == DAT_ID('O','G','G','V')) {
			#ifdef USE_DFIM_OGG
				dfim_convert_ogg_to_sample(&sounds[gibberish_idx].dat, sounds[gibberish_idx].size);
				sounds[gibberish_idx].type = DAT_SAMPLE;
			#else
				#ifdef USE_ALOGG
		 			sounds[gibberish_idx].type = DAT_OGG_VORBIS;  // workaround for alogg 
					SAMPLE *sample = alogg_create_sample(&sounds[gibberish_idx]);
   					free(sounds[gibberish_idx].dat);
					sounds[gibberish_idx].dat = sample;
					sounds[gibberish_idx].type = DAT_SAMPLE;
				#endif			
			#endif
        	gibberish_freq[gibberish_idx] = ((SAMPLE *)(sounds[gibberish_idx].dat))->freq;
        }	
		
		if (npc_voice) deallocate_voice(npc_voice);
		npc_voice = allocate_voice((SAMPLE *)sounds[gibberish_idx].dat);
		if (npc_voice && said && said[0]) {
			voice_set_playmode(npc_voice, PLAYMODE_LOOP);
			//voice_set_volume(npc_voice, 0); 
			//voice_set_frequency(npc_voice, gibberish_freq[gibberish_idx]);
		 	voice_start(npc_voice);
		 } 	
	}
	//if (said[0]) start_gibberish(gibberish_idx, gibberish_nc);
	start_talk = ms_count;
}

void trader_t::sayrand(char *s[], int mood, int syl) {
	int count = 0;
	for (;s[count];count++);
	say(s[irand(count)], mood, syl);
}

void trader_t::save(PACKFILE *pf, int flags) {

}
