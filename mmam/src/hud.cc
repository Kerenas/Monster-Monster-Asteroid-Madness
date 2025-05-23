#include "mmam.h"
#include "rand.h"
#include "bubbles.h"
/*
typedef enum award {	
	award_0et, award_0er, award_1et, award_1er, award_2et, award_2er, award_3et, award_3er,
	award_0at, award_0ar, award_1at, award_1ar, award_2at, award_2ar, award_3at, award_3ar, 
	award_0r, award_1r, award_2r, award_4r,	
	award_0race, award_1race, award_2race, award_3race, 
	award_0rm, award_1rm, award_2rm, award_4rm, 
	award_0eat, award_1eat, award_2eat, award_4eat, 
	award_0spit, award_1spit, award_2spit, award_4spit,
	award_dual, award_dual_lost, award_tdual, award_tdual_lost,
	award_last, award_none = -1 
}

*/
//char *award_desc[award_last];

#define	SPR_AWDGOLDA   SPR_AWD01GOLD
#define SPR_AWDSILVERA SPR_AWD02SILVER
#define SPR_AWDBRONZEA SPR_AWD03BRONZE
#define	SPR_AWDGOLDB   SPR_AWD01GOLD
#define SPR_AWDSILVERB SPR_AWD02SILVER
#define SPR_AWDBRONZEB SPR_AWD03BRONZE
#define	SPR_AWDCUP1    SPR_AWD01GOLD
#define SPR_AWDCUP2    SPR_AWD02SILVER
#define SPR_AWDCUP3    SPR_AWD03BRONZE
#define AWARD_RM       SPR_AWD04PIT
#define AWARD_EAT      SPR_AWD05PIT
#define AWARD_SPIT     SPR_AWD06PIT

int  award_metal_idx[award_last] = {
	SPR_AWD00TMITG, SPR_AWD01GOLD, SPR_AWD02SILVER, SPR_AWD02SILVER, SPR_AWD03BRONZE, SPR_AWD03BRONZE, -1,-1,
	SPR_AWDGOLDA,  SPR_AWDGOLDA, SPR_AWDSILVERA, SPR_AWDSILVERA, SPR_AWDBRONZEA, SPR_AWDBRONZEA, -1,-1, 
	SPR_AWDGOLDB, SPR_AWDSILVERB, SPR_AWDBRONZEB, -1,	
	SPR_AWDCUP1, SPR_AWDCUP2,SPR_AWDCUP3,-1, // race
	AWARD_RM, -1, -1, -1,//running man 
	AWARD_EAT, -1, -1, -1, 
	AWARD_SPIT, -1, -1, -1,
	-1,-1,-1,-1,
};


/////////////////////////////// hud_t ////////////////////////////////////////
hud_t::hud_t() {
        for (int c = 0; c < MAX_JAILCELLS; c++) {
  		jail[c] = new cage_t(
  			40 + 51 * (c % 3),
  			252 + 52 * (c / 3), c,
  			(c < 3 ? &IDF_CAGE0 : NULL)
		);
        }
	upgrade(&IDF_ROPEUP0);
	upgrade(&IDF_CLAWSPEEDUP0);
	upgrade(&IDF_THRUST0);
	upgrade(&IDF_CLAW0);
	upgrade(&IDF_HULL0);
	memset(marked, 0, sizeof marked);
        cage_info_upgrade = 0;
	hooked_monst = watch_monst = track_monst = scan_monst = NULL;
        scroll_dir = time_scroll = 0;
	menu_num = cage_sel = fire_claw_dir = fame = 0;
        claw_line_out = claw_x = claw_y = claw_vx = claw_vy = claw_ang = 0.;
        claw_drag = 80;
	hud_bmp = (BITMAP *)sprites[SPR_HUD].dat;
	ox = SCREEN_W - hud_bmp->w;
	scroll_dir = time_scroll = pin_time = 0;
	for (int c = 0; c < 10; c++) { food[c] = (arg_cheater ? 100 : 0); injector[c] = (arg_cheater ? 100 : 0); }
	short_news_str = long_news_str = NULL;
	money = (arg_money ? 99999999 : 5000 );
	last_tour_grp = ms_count;
	ticket_price = 50;
	hunting_allowed = 0;
	hunting_price = 500;
	pin_time = flash_time = shake_time = 0;
	next_cage = 0;
	high_force = 0.;
	high_force_time = 0;
	news_mode = news_std;
}

hud_t::~hud_t() {
        for (int c = 0; c < MAX_JAILCELLS; c++) delete jail[c];
        free(short_news_str);   free(long_news_str);
}

void  hud_t::poll() {
	if (scroll_dir && (ms_count >= time_scroll)) {
		ox += scroll_dir;
		if (scroll_dir == 1 && (ox > SCREEN_W)) {
			scroll_dir = 0;
		} else if (scroll_dir == -1 && (ox < SCREEN_W - hud_bmp->w)) {
			ox = SCREEN_W - hud_bmp->w;
			scroll_dir = 0;
		}
	}
	if (trader) trader->poll();
	poll_menus();
	poll_claw();
	for (int c = 0; c < MAX_JAILCELLS; c++) {
		if (jail[c]) jail[c]->poll();
	}        
	if (input_map.fire[3] && ship->base == &mb_netship) for (int c = 0; c < MAX_JAILCELLS; c++) {
  			if (jail[c]) jail[c]->dump();
	}

}

void hud_t::scroll_in() {
	time_scroll = ms_count; scroll_dir = -1;
}

void hud_t::scroll_out() {
	time_scroll = ms_count; scroll_dir = 1;
}

void hud_t::forget(monst_t *monst) { 
	if (monst == hooked_monst) hooked_monst = NULL;
	if (monst == watch_monst)  watch_monst  = NULL;
	if (monst == track_monst)  track_monst  = NULL;
	if (monst == scan_monst)   scan_monst   = NULL;
};

void hud_t::forget_all() {
	hooked_monst = watch_monst = track_monst = scan_monst = NULL;
 };


void hud_t::short_news(char *format, ...) {
	free(short_news_str);
	char buff[800];
	va_list args;
	va_start(args, format);
	vsnprintf(buff, sizeof buff, format, args);
	va_end(args);
	short_news_str = strdup(buff);
        short_news_time = ms_count;
}

void hud_t::long_news(char *format, ...) {
	if (news_mode == news_ignore) return;
	char buff[800];
	int bs;
	char *b;
        if (news_mode == news_addend && long_news_str) {
        	b = strncpy(buff, long_news_str, sizeof buff);
                bs = (int)(b - buff);
        } else {
        	bs = sizeof buff;
        	b = buff;
        }
        free(long_news_str);
	va_list args;
	va_start(args, format);
	vsnprintf(b, bs, format, args);
	va_end(args);
	long_news_str = strdup(buff);
        long_news_time = ms_count;
}

void hud_t::show_item_info(struct ivalu_t *k) {
	//if (trader)
	 {
		watch_item = k;
		menu_num = hmenu_watch_item;
	} // else menu_num = hmenu_none;
}

// normal
const float bungee_coef = 1./1500., dragforce_coef = 1./2000., bungeeL_coef = 2.;; // combo force coefs
	
// const float dragforce_coef = 1./2000., bungee_coef = 0., bungeeL_coef = 2.;// dragforce only coef	
// const float dragforce_coef = 0., bungee_coef = 1./1000., bungeeL_coef = 2.; // bungee only coef (only a little streatchy)
// const float dragforce_coef = 0., bungee_coef = 1./2000., bungeeL_coef = 2.;  // bungee only coef (really bungee like)



void hud_t::yank(float &rope_force, float &angle_2_monster, float &sina2m, float &cosa2m, float &relative_velocity_angle, float &relative_velocity_magnitude) {
	
	float distance = dist(ship->x, ship->y, hooked_monst->x, hooked_monst->y);
	rope_force = 0.;
	float fudge_dist =  distance - claw_line_out - 0.0001;
	if (fudge_dist > 0.) {
	  
	    float stretch_force = ( claw_line_out < 0.05 ? 0. : 
	    	MIN(fudge_dist / claw_line_out, bungeeL_coef) * (MIN(hooked_monst->size, ship->size) * bungee_coef)
	    ); // bunge cord 
	
	
		if (cos(relative_velocity_angle - angle_2_monster) > 0) {
			rope_force = cos(relative_velocity_angle - angle_2_monster) *  (MIN(hooked_monst->size, ship->size) * dragforce_coef) * relative_velocity_magnitude; 
		} // dragging force.
	
		 rope_force += stretch_force;
	} 
	return;
}


monst_base *things_with_hands[] = {
	&mb_orick, &mb_raider, &mb_sentry,
	&mb_sentinel, &mb_hoveler, &mb_hunter,
	&mb_worker, &mb_cheech,
	NULL
};

monst_base *stuff_i_want[] = {
	&mb_deathray, &mb_sunstriker, &mb_railgun,
	&mb_stormhand, &mb_pulsar, &mb_lancer, &mb_morningstar, &mb_rumbler,
	&mb_spear, 	&mb_gorgondarts, &mb_comadarts, &mb_heartattackdarts,
	&mb_outcolddarts,	&mb_pacifyerdarts,
	&mb_trankquildarts,	&mb_ezsleepdarts,
	NULL
};

monst_base *vehicles[] = {
	&mb_netship, &mb_raidship, &mb_enforcer, &mb_watcher,
	&mb_guntower, &mb_dgtower, &mb_xcannontower, &mb_galtixtower,
	&mb_raiderguntower, &mb_raiderdgtower, &mb_raiderxcannontower, &mb_raidergaltixtower,
	NULL
};

monst_base *full_turret_vehicles[] = {
	&mb_netship, &mb_enforcer, &mb_watcher,
	&mb_guntower, &mb_dgtower, &mb_xcannontower, &mb_galtixtower,
	&mb_raiderguntower, &mb_raiderdgtower, &mb_raiderxcannontower, &mb_raidergaltixtower,
	NULL
};


void hud_t::poll_claw() {
//	printf("claw_dir %i   [%i,%i,%i]\n", fire_claw_dir, input_map.fire[0], input_map.fire[1], input_map.fire[2]);
	if (ship->dead() || ship->base != &mb_netship) {
		hooked_monst = NULL;
		claw_line_out = fire_claw_dir = 0;
	}
   	if (ship->dead()) return;
   	static int my_teleport_is_sticky = 0; 
   	static int grab_time = 0;   			
   	
   	if (ship->base == &mb_meeker && !my_teleport_is_sticky && 
   		((input_map.fire[0] && input_map.target_valid) || input_map.menu_but == HUD_CLAW)
   	) {
		// guts copied from the meeker_ai
		ship->my_ast->add_jumpgate(ship->x, ship->y, SPR_TELEPORT);
		ship->x = irand(ship->my_ast->pw);   ship->y = irand(ship->my_ast->ph);
		ship->energy -= 60;
		my_teleport_is_sticky = 1;
  	} else if (!((input_map.fire[0] && input_map.target_valid) || input_map.menu_but == HUD_CLAW)) {
   		my_teleport_is_sticky = 0; // fire buttons are autofire.
   	}   	
   	if (ship == thehuman  || ship->check_mb_list(things_with_hands)){
   		if (ship->armed() && input_map.fire[3]) {
   			ship->drop_weap();
   			grab_time = ms_count + 800;
   		} else if (ms_count > grab_time) {
			monst_t *grab_it = NULL;
			for (list<monst_t *>::iterator mon = ship->my_ast->m.begin(); mon != ship->my_ast->m.end(); ++mon) {
				if ((dist((*mon)->x,(*mon)->y,ship->x,ship->y) < ship->radius + (*mon)->radius )) {
					if ((*mon)->check_mb_list(stuff_i_want)) { grab_it = (*mon); break; }
				}
			}
			if (grab_it) {		
				ship->take_weap(grab_it);
			}
		}
   	}
   	if (ship == thehuman && ((input_map.fire[0] && input_map.target_valid) || input_map.menu_but == HUD_CLAW || input_map.repair_btn) ) {
		monst_t *fix_it = NULL;
		
		for (list<monst_t *>::iterator mon = ship->my_ast->m.begin(); mon != ship->my_ast->m.end(); ++mon) {
			if (((*mon)->org_idx == org_ast0) && (dist((*mon)->x,(*mon)->y,ship->x,ship->y) < ship->radius + (*mon)->radius ) && ((*mon)->hp) && ((*mon)->hp != (*mon)->mhp)) {
				fix_it = (*mon); break;
			}
		}
		if (fix_it) {		
			if (ship->attack_time < ms_count)  {
				// player is level 1.5 maintance
				ship->do_act(ACT_BUILD);
				ship->attack_time = ms_count + 1000;
				fix_it->hp = MIN( fix_it->hp + 10, fix_it->mhp);
				int mip = 40;
				// worker will become fatigued repairing buildings (sharp drop avoids ai ping-ponging)
				ship->energy = (ship->energy - mip < 100 ? MIN(50, ship->energy) : ship->energy - mip);		
			}
		} else input_map.repair_btn = 0;
	} else if (ship->base != &mb_netship && input_map.fire[1] && input_map.target_valid ) {
		float ta = afix(atan2(input_map.target_y - ship->y, input_map.target_x - ship->x) - ship->ang);
		if (fabs(ta) < M_PI / 3. || ship->check_mb_list(full_turret_vehicles)) { 
			ship->fire_at(input_map.target_x, input_map.target_y);
		}
	} else if (ship->base != &mb_netship && input_map.fire[1] && input_map.menu_hover == HUD_WEAPON  ) { // allows autofire
		ship->fire(ship->ang);
	} else if (ship == thehuman && input_map.vehicle_btn) {
		for (list<monst_t *>::iterator mon = ship->my_ast->m.begin(); mon != ship->my_ast->m.end(); ++mon) {
			if ((*mon)->alive() && ((*mon)->check_mb_list(vehicles)) && (dist((*mon)->x,(*mon)->y,ship->x,ship->y) < 60. )) {
				ship->forget_all(); ship->my_ast->unhook_monst(ship); ship = (*mon); 
				ship->org_idx = thehuman->org_idx;   ship->ai_func = ai_player;
				break;
			}
		}
	} else if (input_map.vehicle_btn && claw_line_out == 0 && hooked_monst == NULL && 
		ship->check_mb_list(vehicles) 
	) {
		ship->ai_func = (ship->base->org_idx == org_ast0 ? ship->base->ai_func  : ai_item);
		ship->my_ast->add_monst_by(thehuman, ship->x, ship->y); ship = thehuman;
	} 
	if (ship->base != &mb_netship) return;
	if (hooked_monst) {
		if ( input_map.fire[1]) { hooked_monst = NULL; return; }
		float D = dist(ship->x, ship->y, hooked_monst->x, hooked_monst->y);
		if ( D < ship->radius ) {
			if (!pin_time) pin_time = ms_count + 3000;
		
		} else pin_time = 0; 
		if ( (hooked_monst->tired() ||
			(pin_time && ms_count > pin_time)
			) && claw_line_out < ship->radius
		) { if (capture(hooked_monst)) return; }
		claw_drag -= input_map.sel * 5;
		claw_drag = MID(5, claw_drag, 100);
		ship->vx += ship->ax;
		ship->vy += ship->ay;
		hooked_monst->vx += hooked_monst->ax;
		hooked_monst->vy += hooked_monst->ay;
		float angle_2_monster = atan2(hooked_monst->y - ship->y,  hooked_monst->x - ship->x);
		float sina2m = sin(angle_2_monster);	//a2m = angle to monster
		float cosa2m = cos(angle_2_monster);
		float wench_pull_f = claw_speed_level / TURNS_PER_SEC;
		float wench_pull_d = (90. + claw_speed_level * 90./4.) / TURNS_PER_SEC;
		claw_line_out -= (input_map.fire[0] ? wench_pull_d : 0);
		if (claw_line_out < 1) claw_line_out = 1;
		float relative_vx = (hooked_monst->vx - ship->vx);
		float relative_vy = (hooked_monst->vy - ship->vy);
		float relative_velocity_angle = atan2(relative_vy, relative_vx);
		float relative_velocity_magnitude = z_dist(relative_vx, relative_vy);
		static float claw_drop_force_table[5] = {
			2.5, 4.0, 7.25, 13.0, 20.0 // lvl 0: some stuff reqs darts.  lvl 4: no drag no problem 
		};
		float claw_drop_force = claw_drop_force_table[MID(0,claw_level,4)];
	
	
		float rope_force;	//the force applied by the rope
		yank(rope_force, angle_2_monster, sina2m, cosa2m, relative_velocity_angle, relative_velocity_magnitude);
		// if (rope_force > drag) {rope_force = (rope_force + drag) / 2;}
	
		float drag_slip = 1. - (claw_drag / 100.);	
		float drag_thresh = (claw_drag / 100.) * claw_drop_force;	
		if (D > claw_line_out) { 
			claw_line_out = MIN(claw_line,
				claw_line_out + (D - claw_line_out) * (rope_force > drag_thresh ? 0. : drag_slip )
			);
			yank(rope_force, angle_2_monster, sina2m, cosa2m, relative_velocity_angle, relative_velocity_magnitude);
		}
	
		if (high_force < rope_force) {
			high_force_time = ms_count + 10000;
			high_force = rope_force;
		}			
		
		int hold_weapon_challenge_lvl[11] = {
		//f 0   1   2   3   4   5   6   7   8   9  10
			0,  0,  2,  4,  7,  9, 13, 15, 17, 19, 20
		}; 
		if (rope_force > claw_drop_force) {
			hooked_monst = NULL; return;
		} else if ((rope_force >= hud->high_force) && irand(20) < hold_weapon_challenge_lvl[MID(0,(int)(rope_force),10)] ) {
			hooked_monst->drop_weap();
		}
		ship->vx += rope_force * cosa2m / (ship->size / 1000. );
		ship->vy += rope_force * sina2m / (ship->size / 1000. );
		hooked_monst->vx -= rope_force * cosa2m / (hooked_monst->size / 1000. );
		hooked_monst->vy -= rope_force * sina2m / (hooked_monst->size / 1000. );
		ship->x += ship->vx;
		ship->y += ship->vy;
		hooked_monst->x += hooked_monst->vx;
		hooked_monst->y += hooked_monst->vy;
	
		if (hooked_monst->x < 0)              hooked_monst->x = 0.0; 
		else if (hooked_monst->x > ast->pw)   hooked_monst->x = ast->pw;
		if (hooked_monst->y < 0)              hooked_monst->y = 0.0;
		else if (hooked_monst->y > ast->pw)   hooked_monst->y = ast->pw;
	
		if (ship->x < 0)              ship->x = 0.0; 
		else if (ship->x > ast->pw)   ship->x = ast->pw;
		if (ship->y < 0)              ship->y = 0.0;
		else if (ship->y > ast->pw)   ship->y = ast->pw;
		claw_x = hooked_monst->x;
		claw_y = hooked_monst->y;
		claw_ang = angle_2_monster;
		claw_stress = 400. * rope_force / claw_drop_force; // scale to warning indecator.
		return;
	}
	claw_stress = 0;
	float ddx = claw_x - ship->x; float ddy = claw_y - ship->y;
	float ddc = z_dist(ddx,ddy);
	pin_time = 0;
	if (fire_claw_dir == -1) {
		if ((claw_line_out -= (90. + claw_speed_level * 90./4.)/ TURNS_PER_SEC) <= 0) { claw_line_out = 0.; fire_claw_dir = 0; return; }
		if (claw_line_out < ddc) {
			claw_x = ship->x + (ddx / ddc) * claw_line_out;
			claw_y = ship->y + (ddy / ddc) * claw_line_out;
			claw_ang = atan2(ddy,ddx);
		}

	} else if (fire_claw_dir == 1) {
		claw_line_out = ddc;
		claw_x += claw_vx; claw_y += claw_vy;
		monst_t *hm = ast->monst_at(claw_x, claw_y, ship);
		if ( hm ) {
			if ( !(hm->flags & AF_NOHOOK)) {
				hooked_monst = hm;
				hm->hit(10, ship, snd_die, -1);
				printf((char*)"hooked!\n");
				fire_claw_dir = -1;
				SHIP_NOISE(HOOK_CLAW);
			}
		}
		if (input_map.fire[1] || (claw_line_out > 3 * claw_line / 4)) { 
			fire_claw_dir = -1;
			SHIP_NOISE(MISS_CLAW);
		}
	} else if (input_map.fire[0] && input_map.target_valid == SPR_CROSSHAIRS_INTERFACE ) {
		monst_t *hm = ast->monst_at(input_map.target_x, input_map.target_y, ship);
		if (hm) {
			if (hm->alive() ) hm->open_interface();
			if (hm->base == &mb_astbase && hm->dead() && money >= REBUILD_COST) {
					hm->hp = hm->mhp; money -= REBUILD_COST;
					hm->action_idx = ACT_CHILL;
					SHIP_NOISE(BUILD);
					ast->add_jumpgate(hm->x, hm->y, SPR_BUILD);
			}
		}
	} else if ((input_map.fire[0] && input_map.target_valid) || input_map.menu_but == HUD_CLAW ) {
		claw_x = ship->x; claw_y = ship->y;
		claw_ang = (input_map.target_valid ?
			atan2( input_map.target_y - ship->y, input_map.target_x - ship->x) : 
			ship->ang
		);
		claw_vx = ship->vx + cos(claw_ang) * claw_vel;
		claw_vy = ship->vy + sin(claw_ang) * claw_vel;
		SHIP_NOISE(FIRE_CLAW);
		fire_claw_dir = 1;
	} else if (input_map.fire[1] && input_map.target_valid ) {
		ship->fire_at(input_map.target_x, input_map.target_y);
	} else if (input_map.fire[1] && input_map.menu_hover == HUD_WEAPON  ) { // allows autofire
		ship->fire(ship->ang);
	}
	if (!trader || !trader->extended()) ship->toggle_weap(input_map.sel);
}


void hud_t::fishing(float &ship_va, float &hooked_monst_va) {
        float drag = (claw_drag / 100.);
	float wf = (input_map.fire[0] ? 500. : 0);
	float vt = ship_va + hooked_monst_va;
	float wlc = dist(hooked_monst->x, hooked_monst->y, ship->x, ship->y) - (claw_line_out + vt);
	int slacked = (wlc < 0.);
       	if (slacked) { // rope is limp
        	claw_stress = 0;
		if (wf) {
			claw_line_out -= MID(0., -wlc, 1.);
			// technically i should still apply force to the rope if  -wlc < 1
		}
		return;
	}
	float cp = ( ship->size * (ship_va == 0 ? 1. : ship_va > 0. ? ship_va : 0.) + ship->size + wf);
	float sp = ( hooked_monst->size * (hooked_monst_va == 0 ? 1. : hooked_monst_va > 0. ? hooked_monst_va : 0.) + hooked_monst->size);
	float brute_line_f = cp + sp;
	if (vt <= 0.) { // ship and monster are moving toward eachother
        	claw_stress = 0;
		if (wf) {
			claw_line_out -=  MID(0., -vt,1.);
		}
		return;
   	}
	float tv = drag * 20000.;
	float adj_line_f = ( brute_line_f < tv || claw_line_out >= claw_line ?
		brute_line_f :
		tv + ( (brute_line_f-tv) * drag)
	);
	float free_force = brute_line_f - adj_line_f;
	claw_stress = (int)(claw_test ?  adj_line_f / (claw_test * 100.) * 100. : 100. );
	if (claw_stress >= 120) {
		hooked_monst = NULL; claw_stress = 0;
	} else {
        	hooked_monst->stress(adj_line_f);
        	float orl = claw_line_out;
	        if (wf) claw_line_out -= (brute_line_f ? (adj_line_f / brute_line_f) : 1.);
        	claw_line_out += (ship_va + hooked_monst_va) * (brute_line_f ? (free_force / brute_line_f) : 0.);
	        float crl = claw_line_out - orl;
	        // ship_va + hooked_monst_va == crl
	        // i need some way to figure out who gets pulled better
	        if (hooked_monst->size > ship->size) ship_va = crl - hooked_monst_va;
	        else
	        	hooked_monst_va = crl - ship_va;
	}
}

int  hud_t::capture(monst_t *monst) {
	if (monst->base->inv_link && monst->base->inv_link->typ != inv_mineral) {
                if (monst->my_ast) monst->my_ast->unhook_monst(monst);
                monst->forget_all();
                if (monst->base->inv_link->typ == inv_proj ||
                    monst->base->inv_link->typ == inv_money
                ) {
                	ship->take_weap(monst);
                } else {
                	upgrade(monst->base->inv_link);
			delete monst;
                }
		return 1;
	}
	if (!(next_cage >= 0 && jail[next_cage]->vacent())) {
		next_cage = -1;
		for (int c = 0; c < 12; c++) if (jail[c]->vacent()) { next_cage = c; break; }
	}
	if (next_cage >= 0) {
		if (monst == hooked_monst) hooked_monst = NULL;
		jail[next_cage]->pack(monst);
		monst->base->stat_didscan = 1;
		next_cage = -1;
		for (int c = 0; c < 12; c++) if (jail[c]->vacent()) { next_cage = c; break; }
		return 1;
	}
	return 0;
}


#define MONST_INFO_X (ox + 21)               
#define MONST_INFO_Y 246                     
#define MONST_INFO_R (ox + hud_bmp->w - 9)   
#define MONST_INFO_B 457                     

void hud_t::draw() {
	input_map.menu_hover = -1;
	input_map.monster_hover = -1;
	if (trader) trader->draw();
	draw_bars();
	draw_sprite(active_page, hud_bmp, ox, 0); // hud frame
	textprintf_right_ex(active_page, &font_micro,
		(MONST_INFO_R) + 2, 3,   White, -1 , "$%i",(int)(money)
	);
	if (long_news_str) {
		int bw = 400;
		int n = 1 + text_length(&font_micro, long_news_str) / (bw - 16);
		int bh = MIN(12 * n + 8, ms_count - long_news_time / 400);
		int bt = SCREEN_H - bh, bb = SCREEN_H - 1, bl = 0, br = bl + bw - 1;
		rectfill(active_page, br, bt, bl, bb,  GhostGreen);
		rect(active_page, br, bt, bl, bb, LightGray);
		
		textout_box(active_page, &font_micro, long_news_str,
			bl + 4, bt + 4, bw - 8, bh - 8, Black, -1
		);
		if (ms_count - long_news_time > 8200) { free(long_news_str); long_news_str = NULL; }
  	}
  	switch (menu_num) {
        case hmenu_none: {
        	monst_t *look_monst = (
        		watch_monst ? watch_monst : track_monst ? track_monst :
        		(ast == home_ast || ast->scanner_count[1] || ast->scanner_count[4] ) && input_map.target_valid ?
        		ast->monst_at(input_map.target_x, input_map.target_y) : NULL
			);
        	if (look_monst) draw_monster_info(look_monst);
   		else for (int c = 0; c < MAX_JAILCELLS; c++) {
  			jail[c]->draw();
                	textprintf_centre_ex(active_page, &font_bold, ox + 57 + 52 * (c % 3), 240 + 52 * (c / 3), jail[c]->m ? White : Red, -1, "F%i", c +1 );
                        if (next_cage == c) draw_sprite(active_page, (BITMAP *)sprites[SPR_CAGEPTR].dat, ox + 18 + 52 * (c % 3), 262 + 52 * (c / 3));
     		}
     		break;
        }
        case hmenu_caged:  draw_menu_caged();  break;
        case hmenu_food:   draw_menu_food();   break;
        case hmenu_inject: draw_menu_inject(); break;
        case hmenu_trader: draw_menu_trader(); break;
        case hmenu_arena:  draw_menu_arena();  break;
        case hmenu_animal_info: draw_monster_info(jail[cage_sel]->m);  break;
        case hmenu_watch_item: draw_watch_item();  break;
   	}
	draw_timer_circle();
	// circlefill(active_page, ox + 60 + input_map.vt / 4, 500 + input_map.vy / 4, 10, LightGreen);
}

void hud_t::draw_monster_info(monst_t *look_monst) {
	if (look_monst) {
		look_monst->base->stat_didscan = 1;
		rectfill(active_page, MONST_INFO_X, MONST_INFO_Y, MONST_INFO_R, MONST_INFO_B,  Black);
		rect(active_page, MONST_INFO_X, MONST_INFO_Y, MONST_INFO_R, MONST_INFO_B, LightGray);
		BITMAP *monst_spr = ( look_monst->base == &mb_orick ?
			(BITMAP *)(sprites[orick_clan[look_monst->orick_clan()].spr_big].dat) :
			look_monst->base->spr_big[0] < 0 ? NULL : 
			(BITMAP *)(sprites[look_monst->base->spr_big[0]].dat)
		);
		if (monst_spr) draw_sprite(active_page, monst_spr, MONST_INFO_X + 8, MONST_INFO_Y + 8);
		int lat = -1; int awx = MONST_INFO_X + 6; int awy, awxmax, mark_awx;
		int awytop = MONST_INFO_Y + 100;
		if (look_monst->krulasite_inf && ast->scanner_count[4]) {
			BITMAP *inf_spr  = (BITMAP *)(sprites[SPR_INFECTED].dat);
			draw_sprite(active_page, inf_spr, MONST_INFO_X + 12, MONST_INFO_Y + 80);
		} else for (int a=0; a < award_last; a++) {
			int n = look_monst->award[a];
			if (award_metal_idx[a] >= 0 && n) {
				if (lat >= 0 && lat != award_metal_idx[a]) awx = (awy == awytop ? awx : awxmax) + 16;
				if (lat != award_metal_idx[a]) {
					mark_awx = awx; awy = awytop; 
					lat =  award_metal_idx[a];
				}
				BITMAP *awd_spr  = (BITMAP *)(sprites[award_metal_idx[a]].dat);
				for (;n ; --n) {
					// calc hover later.
					draw_sprite(active_page, awd_spr, awx, awy);
					if (awx + 4 >= mark_awx + 30) { awy+=4; awx = mark_awx; } else awx += 4;
					awx += 4;				
				}
			}
		}
		FONT *f = (text_length(&font_bold_plus, look_monst->name) < MONST_INFO_R - MONST_INFO_X - 16 ? &font_bold_plus : &font_bold);
		if (text_length(f, look_monst->name) < MONST_INFO_R - MONST_INFO_X - 16) {
			textout_right_ex(active_page, f,
				look_monst->name,
				(MONST_INFO_R) - 2,
				MONST_INFO_Y , White, -1
			);
		} else textout_box(active_page, &font_tiny,
				look_monst->name,
				MONST_INFO_X + 4, MONST_INFO_Y,
				(MONST_INFO_R) - 4,
				MONST_INFO_Y + 35, White, -1
		);
		//look_monst->drugged;	ai_type ai_func;
                int ty = MONST_INFO_Y + 22;
                int page_c = (ast->scanner_count[1] < 3 ? 0 : -8 * ((ms_count/3200) & 0x1));
                #define TTOUT(n, TO...) if (ast->scanner_count[1] >= n || home_ast->scanner_count[4]) { \
         		if (page_c >= 0 && page_c < 8) { \
         			textprintf_right_ex(active_page, &font_micro, (MONST_INFO_R) - 12,  ty, White, -1 , TO); \
         			ty += text_height(&font_micro); \
        		} page_c++; \
		}
                if (arg_cheater) { TTOUT(0, "AI: %i ORG:%s", look_monst->ai_func, org_list[look_monst->org_idx].nam); }

   		if (
   			look_monst->ai_func != ai_building &&
   			look_monst->ai_func != ai_item
   		) TTOUT(2, "Age: %i", (ms_count - look_monst->time_born) / 60000);
   		if (look_monst->sex) { TTOUT(1, "Sex: %s", look_monst->sex == 0 ? "???" : look_monst->sex == 1 ? "Male" : "Female" ); }
        	TTOUT(1, "Size: %i", look_monst->size);
   		if (ast->scanner_count[1] < 3 && look_monst->org_idx != org_ast0 ) {
       			int h = (look_monst->dead() ? 0 : ( (int)(4 * look_monst->hp / look_monst->mhp)) + 1);
       			if (h > 5) h = 5;
       			static char *hs[] = {(char*)"Dead",(char*)"Critical",(char*)"Badly Hurt",(char*)"Injured",(char*)"Slightly Injured",(char*)"Good"};
                	TTOUT(1, "Health: %s", hs[h] );
                } else {
   			TTOUT(0, "Hp: %i/%i", look_monst->hp, look_monst->mhp);
		}
                if ( look_monst->ai_func == ai_building ) {
                	if (home_ast->scanner_count[3] || look_monst->org_idx != org_ast0 ) {
                                if (look_monst->pow_drain < 0) {
                                  	TTOUT(0, "Pow Output: %i", -look_monst->pow_drain);
				} else {
      					TTOUT(0, "Pow Load: %i%s", look_monst->pow_drain,
					(look_monst->pow_on ? "":" (off)")
					);
                        	}
                                if (look_monst->my_ast) TTOUT(0, "Pow Levels: %i/%i",
                                	look_monst->my_ast->orgs_powerr[look_monst->org_idx],
                                        look_monst->my_ast->orgs_powerg[look_monst->org_idx]
				);
                   	}
       		} else if (look_monst->ai_func != ai_item) {
        		TTOUT(3, "Energy: %.0f/%i", look_monst->eff_energy(),  look_monst->max_energy);

 			TTOUT(3, "Agression: %i",   look_monst->agression);
        		TTOUT(2, "Str: %i",  look_monst->str);
        		TTOUT(3, "Attack Rate: %i", look_monst->attack_rate);
        		TTOUT(3, "Speeds: %.0f %.0f %.0f",
        			look_monst->speed * TURNS_PER_SEC, look_monst->lat_speed * TURNS_PER_SEC,
	        		look_monst->turn_rate * TURNS_PER_SEC / M_PI * 180.
 			);
        		if (look_monst->stomach) { TTOUT(2, "Stomach: %.1f%%",  (100. * look_monst->in_stomach / look_monst->stomach));}
        		TTOUT(4, "Met Rate: %i", look_monst->met_rate);
        		TTOUT(4, "Nutrish Val: %i", look_monst->nutrish);
        		if (look_monst->od_thresh) TTOUT(4, "Drug Resist: %i (tox %.0f%%)", look_monst->od_thresh, 100. * look_monst->drugged / look_monst->od_thresh);
        		if (look_monst->gestation) { TTOUT(4, "Gestation: %i",   look_monst->gestation/1000); }
   			TTOUT(4, "Fear Dist: %i", look_monst->fear_dist);
     		}
                #undef TTOUT

        	textout_box(active_page, &font_micro, look_monst->base->desc,
        		(MONST_INFO_X) + 12, MONST_INFO_Y + 130,
                        (MONST_INFO_R - MONST_INFO_X) - 24, 90, White, -1
        	);
        	if (look_monst == track_monst) {
           		textout_ex(active_page, &font_bold,
                		"Tracking.",
                		(MONST_INFO_X) + 8,
                		MONST_INFO_Y + 16 , makecol(255 * abs((ms_count % 1000) - 500)/ 1000, 0, 0), -1
        		);
        	}
		if (arg_xinfo) {
            		char info_block[2000];
            		snprintf( info_block, sizeof info_block,
    			        "AI: %i    ORG:%s  Flags: 0x%08x   "
                                "Frame: %02i:%02i   "
   				"Age: %i(%i)  "
                		"Sex: %s   "
                		"Size: %i (r:%.1f)   "
                		"Hp: %i/%i   "
                		"Energy: %.0f/%i   "
                		"Agression: %i   "
        			"Stomach: %.1f%%   "
        			"Met Rate: %i   "
        			"Nutrish Val: %i   "
                                "Drug Resist: %i (count %.2f tox %.0f%%)  "
                                "Bonuses [spd%.2f  egy%.2f  str%.2f  met%.2f  heal%.2f  agr%.2f  gro%.2f  rest%.2f  inv%.2f]  "
                                "Spores [pyr%i psy%i poi%i]   "
                                "Krul Inf: %s  ",
   		 		look_monst->ai_func, org_list[look_monst->org_idx].nam, look_monst->flags,
   		 		look_monst->action_idx, look_monst->action_frame,
   				(ms_count - look_monst->time_born) / 60000,
   				(look_monst->time_mature - look_monst->time_born) / 60000,
   				(look_monst->flags & AF_ASEXUAL ? "Asexual" : look_monst->sex == 1 ? "Male" : "Female"),
        			look_monst->size, look_monst->radius,
                                look_monst->hp, look_monst->mhp,
				look_monst->eff_energy(),  look_monst->max_energy,
				look_monst->agression,
				(100. * look_monst->in_stomach / MAX(1., look_monst->stomach)),
				look_monst->met_rate,
        			look_monst->nutrish,
        			look_monst->od_thresh, look_monst->drugged, 100. * look_monst->drugged / look_monst->od_thresh,
        			look_monst->speed_bonus, look_monst->energy_bonus, look_monst->str_bonus, look_monst->met_rate_bonus,
	      			look_monst->healing_bonus, look_monst->agression_bonus, look_monst->growth_bonus,
	      			look_monst->restlessness,  look_monst->invisability_bonus,
              			look_monst->spore_pyro_count, look_monst->spore_psycho_count, look_monst->spore_poison_count,
              			(look_monst->krulasite_inf ? "Y" : "N")
        		);
			textout_box(active_page, &font_micro, info_block,
        			0, 0, 500, 100, White, Black
        		);
  		}

	}
	draw_exit();
}

void hud_t::draw_watch_item() {
	if ((!trader) || (trader->ox < 0)) {
   		//menu_num = hmenu_none;
   		//return;
	}
	if (watch_item) {
   		rectfill(active_page, MONST_INFO_X, MONST_INFO_Y, MONST_INFO_R, MONST_INFO_B,  Black);
                rect(active_page, MONST_INFO_X, MONST_INFO_Y, MONST_INFO_R, MONST_INFO_B, LightGray);
   		BITMAP *spr = ((BITMAP *)(sprites[watch_item->inv_sprite].dat));
   		BITMAP *ammo_spr = (BITMAP *)(watch_item->projs < 0 ? NULL : sprites[ watch_item->projs].dat);
                if (spr)      draw_sprite(active_page, spr, MONST_INFO_X + 8, MONST_INFO_Y + 8);
		if (ammo_spr) sprite_rsw(active_page, ammo_spr, MONST_INFO_X + 26, MONST_INFO_Y + 118, M_PI, 26);
                FONT *f = (text_length(&font_bold_plus, watch_item->nam) < MONST_INFO_R - MONST_INFO_X - 16 ? &font_bold_plus : &font_bold);
                textout_right_ex(
                	active_page, f, watch_item->nam,
                	(MONST_INFO_R) - 2,
                	MONST_INFO_Y , White, -1
        	);
                int ty = MONST_INFO_Y + 22;
                textout_box(active_page, &font_micro, watch_item->desc,
        		(MONST_INFO_X) + 12, MONST_INFO_Y + 130,
                        (MONST_INFO_R - MONST_INFO_X) - 24, 90, White, -1
        	);
	} else {
		rectfill(active_page, MONST_INFO_X, MONST_INFO_Y, MONST_INFO_R, MONST_INFO_B,  Black);
		rect(active_page, MONST_INFO_X, MONST_INFO_Y, MONST_INFO_R, MONST_INFO_B, LightGray);

		FONT *f =  &font_bold_plus;
		textout_right_ex(
			active_page, f, "WTF???",
			(MONST_INFO_R) - 2,
			MONST_INFO_Y , White, -1
		);
		int ty = MONST_INFO_Y + 22;

	
	}	
}


void hud_t::draw_bars() {
  	BITMAP *stress_bar = (BITMAP *)sprites[SPR_BAR_STRESS].dat;
  	BITMAP *drag_bar = (BITMAP *)sprites[SPR_BAR_DRAG].dat;
	BITMAP *hull_bar = (BITMAP *)sprites[SPR_BAR_HULL].dat;
	if (claw_stress < 160) bar_draw_h(active_page, stress_bar, 71 + ox, 140, claw_stress);
	else rectfill(active_page, 71 + ox, 134, 71 + ox + stress_bar->w, 140 + stress_bar->h,
		((ms_count/100) & 0x1 ? LightRed : Black)
	);
	
	if (ms_count < high_force_time) {
		textprintf_ex(
			active_page, &font_micro, 
			76 + ox, 142, White, -1,
			"high: %0.2f", high_force 
       	);
	} else {
		high_force = 0.; high_force_time = 0;
	}
    bar_draw_h(active_page, drag_bar, 71 + ox, 155, claw_drag);
	bar_draw_h(active_page, /*lengh*/ stress_bar, 71 + ox, 169, (int)(100 * claw_line_out / claw_line));
    if (ship->mhp) bar_draw_h(active_page, hull_bar, 62+ox, 198, ship->hp * 100 / ship->mhp);
}

static int ast_sprite_list[] = AST_LIST_ITEMS;

void hud_t::draw_timer_circle() {
	BITMAP *ast_spr = NULL, *sign_spr = NULL, *claw_spr = NULL;
	//rectfill(active_page, ox+2, 2, ox+181, 56, Black);
	if ( remote_ast && (home_ast == ast)) {
		remote_ast->skin_num = MID(0, remote_ast->skin_num, NUM_SURFACES - 1);
		ast_spr = (BITMAP *)(
			sprites[ast_sprite_list[remote_ast->skin_num] + (ms_count / 200 % 14)].dat
		);
		if (remote_ast->building_sign) sign_spr = (BITMAP *)sprites[remote_ast->building_sign].dat;
		draw_sprite(active_page,ast_spr,ox+40,34);
		if (sign_spr) draw_sprite(active_page,sign_spr,ox+40+(ast_spr->w - sign_spr->w)/ 2,18);
		textprintf_centre_ex(active_page, &font_bold, ox + 70, 100 , White, Black , "AST #%03i",remote_ast->asteroid_idx);
	}
	if (ast != home_ast) {
		int T = ast->exit_time - ms_count;
		int m = T / 60000; int s = T / 1000 - (m * 60);  int t = (T % 100) / 10;
		if (T > 20000) {
			textprintf_centre_ex(active_page, &font_lcd, ox + 70, 100 , White, Black , "%2i:%02i",m,s);
		} else {
			textprintf_centre_ex(active_page, &font_lcd, ox + 70, 100 , (T > 15000 ? White : LightRed), Black , "%02i.%1i",s,t);
		}
	}
	weap_t *w =  ship->armed();
	if ( w ) {
		BITMAP *weap_spr; float a;
		int xx = ox + 150;
		int yy = 10;
		int ex = xx + 60;
		int ey = yy + 60;
		int os = 38;
		if (
			mouse_x >= xx &&
			mouse_x <= ex &&
			mouse_y >= yy &&
			mouse_y <= ey
		) {
			os = 42;
			input_map.menu_hover = HUD_WEAPON;
		} 
		if (w->t->projs < 0) {
			weap_spr = (BITMAP *)sprites[w->t->inv_sprite].dat; a = 0.;
		} else {
			weap_spr = (BITMAP *)sprites[w->t->projs].dat;      a = M_PI;
		}
		if (weap_spr) sprite_rsw(active_page, weap_spr,ox+170,40, a, os);
		char ss[12] = ""; strncpy(ss,w->t->nam,8);
		textprintf_right_ex(active_page, &font_super_micro, ox + 150, 10 , White, -1 , "%s",ss);
		textprintf_right_ex(active_page, &font_tiny, ox + 190, 48 , White, -1 , "x%i", w->count);
	} else {
		textprintf_right_ex(active_page, &font_tiny, ox + 190, 36 , White, -1 , "NONE");
   	}
	claw_spr = (thehuman == ship ? 	(BITMAP *)sprites[SPR_WRENCH_INV].dat :
		ship->base == &mb_netship ? (BITMAP *)sprites[spr_claw[0]].dat : 
		ship->base == &mb_meeker ? (BITMAP *)sprites[SPR_TELEPORT].dat :  NULL
	);
	if (claw_spr) {
		int xx = ox + 150;
		int yy = 78;
		int ex = xx + 60;
		int ey = yy + 60;
		int os = 38;
		if (
			mouse_x >= xx &&
			mouse_x <= ex &&
			mouse_y >= yy &&
			mouse_y <= ey
		) {
			//rectfill(active_page,xx,yy,ex,ey,LightGreen);
			os = 42;
			input_map.menu_hover = HUD_CLAW;
		} 
		sprite_rsw(active_page, claw_spr,ox+170,108,0., os);
		if (ship->base == &mb_netship) {
			textprintf_right_ex(active_page, &font_super_micro, ox + 150, 128 , White, -1 , "level:%i",claw_level);
		} else if (thehuman == ship) {
			textprintf_right_ex(active_page, &font_super_micro, ox + 150, 128 , White, -1 , "repair");
		} 
	}
	if (ms_count % 5000 > 2500) {
		textprintf_ex(active_page, &font_super_micro, ox +  98, 183 , White, -1 , "fame:%i",fame);
	} else { 
		textprintf_ex(active_page, &font_super_micro, ox +  98, 183 , White, -1 , "tourists:%i", mb_tourist.stat_inpark);
	}
	if (short_news_str) {
		textout_box(active_page, &font_micro, short_news_str,
			ox + 32, 19, 109 - 32, 90, White, -1
		);
		if ( ms_count - short_news_time  > 2900) { free(short_news_str); short_news_str = NULL; }
	}
}

void hud_t::draw_mi( int idx, char *s, int do_output) {
	if (do_output) {
    		char ss[200];
                snprintf(ss,sizeof ss, "%i. %s", idx, s);
   		int xx = MONST_INFO_X + 12;
                int yy = MONST_INFO_Y + 24 + 12 * idx;
                int ex = xx + text_length(&font_tiny, ss);
                int ey = yy + 11;
               	textout_ex( active_page,&font_tiny, ss,	xx, yy, LightGreen, -1 );
                // printf("%i %i %i\n", idx, xx, yy);
                if (
                	mouse_x >= xx &&
                	mouse_x <= ex &&
                	mouse_y >= yy &&
                	mouse_y <= ey
                ) {
                        hline(active_page, xx+1, ey + 1, ex-1, LightGreen);
                        input_map.menu_hover = idx;
                }
	}
}

void hud_t::draw_mi2( int idx, char *s, int n) {
	if (n) {
		char ss[200];
                snprintf(ss,sizeof ss, "%s [%i]", s, n);
		draw_mi( idx, ss, 1);
   	}
}

void hud_t::draw_exit() {
  	if (menu_num == hmenu_none) return;
  	int xx = MONST_INFO_R - 60;
        int tx = MONST_INFO_R - 26;
        int yy = MONST_INFO_B - 26;
        int ex = tx + text_length(&font_tiny, "Exit");
        int ey = yy + 11;
	textprintf_ex( active_page, &font_tiny, tx, yy, LightGreen, -1, "Exit");
	draw_sprite(active_page, (BITMAP *)(sprites[SPR_KEY_TAB].dat),	xx, yy);
        if (
               	mouse_x >= xx &&
               	mouse_x <= ex &&
               	mouse_y >= yy &&
               	mouse_y <= ey
        ) {
                hline(active_page, tx+1, ey + 1, ex-1, LightGreen);
                input_map.menu_hover = HUD_MENU_EXIT;
        }
        if ( // button on kmenu
               	mouse_x >= ox+ 502 &&
               	mouse_x <= 579 &&
               	mouse_y >= ox + 512 &&
               	mouse_y <= 557
       	) {
                input_map.menu_hover = HUD_MENU_EXIT;
        }
}

void hud_t::draw_menu_frame(char *title) {
	rectfill(active_page, MONST_INFO_X, MONST_INFO_Y, MONST_INFO_R, MONST_INFO_B,  Black);
	if (title) textprintf_ex(
		active_page, &font_bold,
		MONST_INFO_X + 15, MONST_INFO_Y + 8, LightGreen, -1, "%s", title
	);
        rect(active_page, MONST_INFO_X, MONST_INFO_Y, MONST_INFO_R, MONST_INFO_B, LightGreen);
}

void hud_t::draw_menu_caged() {
  	draw_menu_frame((char*)"Cage Menu");
	draw_mi( 0, (char*)"Drop Monster",   mm_drop_monst(1,   jail[cage_sel], cage_sel));  // 0
       	draw_mi( 1, (char*)"ID Monster",     mm_id_monst(1,     jail[cage_sel], cage_sel));  // 1
       	draw_mi( 2, (char*)"Feed Monster",   mm_feed_monst(1,   jail[cage_sel], cage_sel));  // 2
       	draw_mi( 3, (char*)"Inject Monster", mm_inject_monst(1, jail[cage_sel], cage_sel));  // 3
       	draw_mi( 4, (char*)"Sell Monster",   mm_sell_monst(1,   jail[cage_sel], cage_sel));  // 4
       	draw_mi( 6, (char*)"Repair Cage",    mm_repair_cage(1,  jail[cage_sel], cage_sel));  // 6
       	draw_mi( 7, (char*)"Sell Cage",      mm_sell_cage(1,    jail[cage_sel], cage_sel));  // 7
       	draw_exit();
}

void hud_t::draw_menu_food() {
        draw_menu_frame((char*)"Food Menu");
	for (int c = 0; c < NUM_INV_FOOD; c++) {draw_mi2(c, ivalu_list[FIRST_INV_FOOD_IDX + c].nam, food[c]);}
       	draw_exit();
}

void hud_t::draw_menu_inject() {
  	draw_menu_frame((char*)"Injection Menu");
	for (int c = 0; c < NUM_INV_INJECTOR; c++) {draw_mi2(c, ivalu_list[FIRST_INV_INJECTOR_IDX + c].nam, injector[c]);}
       	draw_exit();
}

void hud_t::draw_menu_trader() {
	draw_menu_frame((char*)"Trade Menu");
       	draw_exit();
}

void hud_t::draw_menu_arena() {
 	draw_menu_frame((char*)"Arena Menu");
       	draw_exit();
}


void hud_t::poll_menus() {
  	switch (menu_num) {
        case hmenu_none: {
        	weap_t *stop_cycling = ship->armed(); 
        	if ( (stop_cycling) && input_map.quick_weap_btn >= 0 ) {
        		for (ship->toggle_weap(1); ship->armed() != stop_cycling; ship->toggle_weap(1)) {
        			if (input_map.quick_weap_tab[input_map.quick_weap_btn] == ship->armed()->t) break;
        		}
        	} 
        }
        break;
        case hmenu_caged: {
           	switch (input_map.menu_but) {
			case 0: mm_drop_monst(0,   jail[cage_sel], cage_sel); break;  // 0
        	case 1: mm_id_monst(0,     jail[cage_sel], cage_sel); break;  // 1
        	case 2: mm_feed_monst(0,   jail[cage_sel], cage_sel); break;  // 2
        	case 3: mm_inject_monst(0, jail[cage_sel], cage_sel); break;  // 3
        	case 4: mm_sell_monst(0,   jail[cage_sel], cage_sel); break;  // 4
        	case 6: mm_repair_cage(0,  jail[cage_sel], cage_sel); break;  // 6
        	case 7: mm_sell_cage(0,    jail[cage_sel], cage_sel); break;  // 7
	        }
		}
		break;
        case hmenu_food: if (input_map.menu_but >= 0){
               if (food[input_map.menu_but]) {
                        food[input_map.menu_but]--;
                        jail[cage_sel]->m->feed(input_map.menu_but);
               		menu_exit();
               }
          }
          break;
        case hmenu_inject: if (input_map.menu_but >= 0){
               if (injector[input_map.menu_but]) {
                        injector[input_map.menu_but]--;
                        jail[cage_sel]->m->inject(input_map.menu_but);
               		menu_exit();
               }
          }
          break;
        case hmenu_trader: break;
        case hmenu_arena: break;
   	}
	if (input_map.monster_but > -1) {
   		cage_sel = input_map.monster_but;
		if (menu_num == hmenu_none) menu_num = hmenu_caged;
 	}
	if (input_map.menu_exit_but) {
   		if (menu_num == hmenu_none) watch_monst = NULL;
   		menu_exit();
	}
}

#undef MONST_INFO_X
#undef MONST_INFO_Y




int  hud_t::mm_id_monst(int query, cage_t *cage, int cage_num) {
	int ok =  (ast == home_ast && cage->m && cage->m->alive());
	if (query || !ok) return ok;
	watch_monst = cage->m;
        menu_exit();
	return 0;
}

int  hud_t::mm_drop_monst(int query, cage_t *cage, int cage_num) {
	int ok =  (cage->m != NULL);
	if (query || !ok) return ok;
        cage->dump(1);
        menu_exit();
	return 0;
}



int  hud_t::mm_feed_monst(int query, cage_t *cage, int cage_num) {
        int ok = (cage->m && cage->m->alive() && cage->m->stomach);
        if (!ok) return 0;
        ok = 0;
        for (int c = 0; c < 10; c++) if (food[c]) {ok = 1; break;}
	if (query || !ok) return ok;
	menu_num = hmenu_food;
	return 0;
}

int  hud_t::mm_inject_monst(int query, cage_t *cage, int cage_num) {
        int ok = (cage->m && cage->m->alive());
        if (!ok) return 0;
        ok = 0;
        for (int c = 0; c < 10; c++) if (injector[c]) {ok = 1; break;}
	if (query || !ok) return ok;
	menu_num = hmenu_inject;
	return 0;
}

int  hud_t::mm_sell_monst(int query, cage_t *cage, int cage_num) {
	int ok =  (cage->m && cage->m && cage->m->alive()); // trader->wants_monster(cage->m);
	if (query || !ok) return ok;
	//trader->sell_monster(cage->dump());
        menu_exit();
	return 0;
}

int  hud_t::mm_repair_cage(int query, cage_t *cage, int cage_num) {
	return 0;
}

int  hud_t::mm_sell_cage(int query, cage_t *cage, int cage_num) {
	return 0;
}

void hud_t::save(PACKFILE *pf, int flags) {
	// this function is probbally unsafe the buffer _could_ overflow but its rather unlikely
	char buff[8000];
	char *b = buff;
        b += sprintf(b, "hud.ms_count %i\n", ms_count);
        b += sprintf(b, "hud.shortnews %i %s\n", short_news_time, short_news_str);
        b += sprintf(b, "hud.longnews %i %s\n", long_news_time, long_news_str);
        b += sprintf(b, "hud.intblock1 %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i\n",
	 menu_num, scroll_dir, time_scroll,
  	 cage_sel, inventory_act,
         claw_test, claw_drag, claw_stress, ox,
         fire_claw_dir, pin_time, flash_time, shake_time,
         cage_info_upgrade, claw_level, rope_level,
         thrust_level, hull_level, claw_speed_level, spr_claw[0], spr_claw[1], money, karma, fame,
         last_tour_grp, ticket_price, hunting_allowed, hunting_price
        );
        b += sprintf(b, "hud.floatblock1 %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f\n",
	   claw_line_out, claw_line, claw_x, claw_y, claw_vx, claw_vy, claw_ang, claw_vel
	);
        b += sprintf(b, "hud.foodlist %i %i %i %i %i %i %i %i %i %i\n",
         food[0],food[1],food[2],food[3],food[4],food[5],food[6],food[7],food[8],food[9]
        );
        b += sprintf(b, "hud.injlist %i %i %i %i %i %i %i %i %i %i\n",
         injector[0],injector[1],injector[2],injector[3],injector[4],
         injector[5],injector[6],injector[7],injector[8],injector[9]
        );
        b += sprintf(b, "hud.watch_item %s\n", (watch_item ? watch_item->nam : "NONE"));
        pfwrite(pf, buff); b = buff;
        for (int c = 0; c <MAX_JAILCELLS; c++) {
            sprintf(buff, "cage.setcage %i\n", c);
            pfwrite(pf, buff);
            jail[c]->save(pf, flags);
        }
        if (remote_ast) {
        	pfwrite(pf,"ast.hook remote\n");
        	pfwrite(pf,"ast.clear\n");
        	if (ast == remote_ast) pfwrite(pf,"ast.setast\n");
		remote_ast->save(pf,flags);
        }
        pfwrite(pf,"ast.hook home\n");
        pfwrite(pf,"ast.clear\n");
        if (ast == home_ast) pfwrite(pf,"ast.setast\n");
	home_ast->save(pf,flags);
}

int  hud_t::parse(char *cmd, char *next, int line) {
        if (0 == strcmp(cmd, "ms_count")) { ms_count = atoi(next);
        } else if (0 == strcmp(cmd, "shortnews")) { // free(short_news_str); sscanf(next, "%i %s", &short_news_time, short_news_str);
        } else if (0 == strcmp(cmd, "longnews")) { // sscanf(next, "%i %s", &long_news_time, long_news_str);
        } else if (0 == strcmp(cmd, "intblock1")) {
        	sscanf(next, "%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i",
	 	&menu_num, &scroll_dir, &time_scroll, &cage_sel, &inventory_act, &claw_test,
	 	&claw_drag, &claw_stress, &ox, &fire_claw_dir, &pin_time, &flash_time, &shake_time,
         	&cage_info_upgrade, &claw_level, &rope_level,
         	&thrust_level, &hull_level, &claw_speed_level, &spr_claw[0], &spr_claw[1],
         	&money, &karma, &fame,
         	&last_tour_grp, &ticket_price, &hunting_allowed, &hunting_price
        	);
        } else if (0 == strcmp(cmd, "floatblock1")) {
        	sscanf(next, "%f %f %f %f %f %f %f %f",
	   		&claw_line_out, &claw_line, &claw_x, &claw_y,
	   		&claw_vx, &claw_vy, &claw_ang, &claw_vel
		);
        } else if (0 == strcmp(cmd, "foodlist")) {
        	sscanf(next, "%i %i %i %i %i %i %i %i %i %i",
         		&food[0],&food[1],&food[2],&food[3],&food[4],
         		&food[5],&food[6],&food[7],&food[8],&food[9]
        	);
        } else if (0 == strcmp(cmd, "injlist")) {
        	sscanf(next, "%i %i %i %i %i %i %i %i %i %i",
         		&injector[0],&injector[1],&injector[2],&injector[3],&injector[4],
         		&injector[5],&injector[6],&injector[7],&injector[8],&injector[9]
        	);
        } else if (0 == strcmp(cmd, "watch_item")) {
                watch_item = NULL;
        	for (int c = 0; c < NUM_IVALU_LIST; c++ ) if ( strcmp(next, ivalu_list[c].nam) == 0) {
           		watch_item = &ivalu_list[c];   break;
           	}
        } else { script_fuss((char*)"hud command \"%s\" unreconized",cmd); return 0; }
        return 1;
}


