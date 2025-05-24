#include "mmam.h"
#include "mmam_ai.h"
#include "rand.h"

/*
Notes on building new AIs:
all ais must set the hunger_level before calling any query functions. 
a hunger_level >= 3 will eat anything with a nutrish value.
each ai should call the walk function exactly one time (if my_ast != NULL)
anything that should freek out when on fire should call the _ai_onfire 
then return immidately.  my_ast may be NULL (if an animal is caged or in a hole) so 
my_ast must be checked by all ais.  Use care when walking the my_ast->m list
hitting another monster and picking up or dropping items can cause your iterator
to become invalid leading to a crash.  query functions can be considered safe.  
*/


void monst_t::plant_ai() {
	float v = z_dist(vx,vy);
    this->vx *= ( v < 2. ? 0.50 : 0.96 );
    this->vy *= ( v < 2. ? 0.50 : 0.96 );
};

void monst_t::building_ai() {}
void monst_t::item_ai() { plant_ai(); }
void monst_t::trainer_ai() { merchant_ai(); }
void monst_t::ackatoo_ai() { beholder_ai(); }
void monst_t::stalker_ai() { animal_ai(); }


void monst_t::meteor_ai() {
    // its funny that a rock gets its own ai.
    hunger_level = 0;
    walk(1.,0.,0.);
    radius *= 0.975; // per tick. really not ideal
    if (ms_count > aiu1) {
        ast->explode(x,y, 80., 1000., NULL, NULL, (irand(10) == 0 ? WP_FIRE : 0));
		hit(hp); radius *= 2.5; // the crater is huge!        
		flags = AF_NOHOOK | AF_NOSHOOT;
		vx = vy = 0;
		hud->shake_time = ms_count + 400;		
    }
}


void monst_t::worker_ai() {
    // This ai handles all classes of asteroid zero employees (who dont work indoors).
    // including personality querks
    // uses aiu1: anthropy, jobdesc, qwerks, lazyness 
    hunger_level = 0;
    int job = ((aiu1 >> 24) & 0xff);
    monst_t *fix_it = NULL; float fix_dist = my_ast->pw;
    float max_ff = 0.;
    int max_interesting = 0;
    float max_interesting_fd = my_ast->pw;
    			
    int phase_index = ((ms_count) / (aiu1 & QWERK_ADHD ? 4000 : 12000)) & 0x7; 
    int feels_lazy = ((aiu1 >> phase_index) & 0x1);
    if (_fire_run_ai() || _krulasite_inf_ai() || _slap_and_tickle_ai()) return;
	if ( // lawful workers will quit eventually if you break the law too much.
		aiu1 & QWERK_LAWFUL && org_list[org_police].al <= -2 && 
		(game_turn % (TURNS_PER_SEC * 20)) && irand(10) == 0
	) {
		hud->long_news((char *)"%s just quit for ethical reasons.", name);
		jump_out_time = ms_count;
	}
    for (list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
		monst_t *look_monst = *mon;
		if (look_monst == this) continue;
		float fd = dist(look_monst->x, look_monst->y,
					(this->x  + cos(this->ang)  * this->radius  * 1.1) ,
					(this->y  + sin(this->ang)  * this->radius  * 1.1)
				    ) - (look_monst->radius);
		if (look_monst->alive()  &&  (fd  < this->fear_dist) && (
		         hates(look_monst) || scared_of(look_monst) ||
                	(look_monst == who_hit_me_last ))
		) {
			float fr = fd / fear_dist;
			float ff = look_monst->size * (look_monst == who_hit_me_last ? 6. : 1.) *
					(fr > 0.5 ? 1. : fr > 0.2 ? 5.0 : 8. );
			if (ff > max_ff) {
				see_monst = look_monst; max_ff = ff;
			}		
		}
  		if ((job == 'M' && look_monst->base->org_idx == org_ast0 && look_monst->flags & AF_INORGANIC) ||
  		(job == 'V' && look_monst->base != base && (look_monst->flags & AF_INORGANIC == 0)) 
  		
  		) {
  		    if (((look_monst->hp != look_monst->mhp) || look_monst->krulasite_inf) && fd < fix_dist ) {
  		    	chase_last = fix_it = look_monst; fix_dist = fd;
  		    }
  		}
  		if (
  			(job == 'C' && look_monst->hunger_level > 1 && look_monst->base != &mb_krulasite && look_monst->base != &mb_slymer) && fd < fix_dist ) {
  		    	fix_it = look_monst; fix_dist = fd;
  		}
  		if (look_monst->base == base  ) {
        	int look_job = look_monst && look_monst->alive() ? ((look_monst->aiu1 >> 24) & 0xff) : 0;
    		// become unlazy when a manager is around.
     		if (fd  < (this->fear_dist + look_monst->worker_skill * this->fear_dist / 3.)) feels_lazy &=  (job != 'R' && look_job == 'R' ? 0 : 1); 
		}
  		if (look_monst->base == base ) {
        	float fr = fd / fear_dist;
            float ff = look_monst->size * (look_monst == who_hit_me_last ? 6. : 1.) *
                       (fr > 0.5 ? 1. : fr > 0.2 ? 5.0 : 8. );
			// dead guys look interesting to eyeryone (medics can revive if they havn't decayed yet).
			// injured guys look interesting to  medics.
			// guys that are standing still are interesting to managers
			int look_interesting = (!look_monst->alive() ? 4 : 
				((job == 'K' && look_monst->hp < look_monst->mhp) || 
				(job == 'R' && fabs(look_monst->vx) < 0.01 && fabs(look_monst->vy) < 0.01 ) ? 2 :
				 1)
			);
    		if (look_interesting > max_interesting || (look_interesting == max_interesting && ff > max_interesting_fd)) {
    			max_interesting = look_interesting;
    			max_interesting_fd = fd;
    			see_buddy = look_monst; 
    		}     		    
		}    
    }
    if (who_hit_me_last || scared_of(see_monst) ) { 
		if (! see_monst) see_monst = who_hit_me_last;
		weap_t *w = armed();
		if (aiu1 & QWERK_PACIFIST || (see_monst && see_monst->base == &mb_meteor)) {
	        feel(emot_surprized);
    	    evade(see_monst);		
		} else if (w || aiu1 & QWERK_VIOLENT || job == 'S' ) { // attack
			soldier_ai(); 
		} else {
	        feel(emot_surprized);
    	    evade(see_monst);		
		}
	} else if (feels_lazy || energy < 100) {
        if (aiu1 & QWERK_DRUGS) cheech_ai();
        else { // go hang out by the base, later make them 'talk' to each other on break.
			monst_t *look_monst = mainbase;
			float fd = ( dist(look_monst->x, look_monst->y,
				(this->x  + cos(this->ang)  * this->radius  * 1.1) ,
				(this->y  + sin(this->ang)  * this->radius  * 1.1)
				) - (look_monst->radius)
			);
			float ta = atan2(look_monst->y - y, look_monst->x - x);
			float tang = afix(ta - ang);
								
			if (fd > 240) aiu3 = 0; 
			if (fd < 200) aiu3 = 444;
			if (aiu3) {
					float ta = atan2(look_monst->y - this->y,   look_monst->x - this->x);
					fear_hunger_pick(); // see_buddy is closest. improve for large groups later
					if (see_buddy) {
						look_monst = see_buddy;
						ta = atan2(look_monst->y - y, look_monst->x - x);
						float tang = afix(ta - ang);
						fd = ( dist(look_monst->x, look_monst->y,
								(this->x  + cos(this->ang)  * this->radius  * 1.1) ,
								(this->y  + sin(this->ang)  * this->radius  * 1.1)
								) - (look_monst->radius)
						);
						// workers move to comfortable distance facing eachother.
						walk(tang, fd > 15 ? 0.30 : fd < 10 ? -0.08 : 0., 0.);
					} else walk(tang,  0.0, 0.0);					
			} else walk(tang, 0.30, 0.0);     
        }
    } else switch (job) {
    case 'S': /*security*/ soldier_ai(); break; // patrol collect better weapons
    case 'R': /*manager*/ if (max_interesting > 1) chase(see_buddy, 0.3); else wander(); break;
    case 'K': /*medic*/ 
		if (see_buddy ? see_buddy->hp != see_buddy->mhp : 0) { 
			float danger_lvl = see_buddy->mhp ? see_buddy->hp / see_buddy->mhp : 1.;
			float rs = (
				danger_lvl > 0.8 ? 0.3 :
				danger_lvl > 0.5 ? 0.4 :
				danger_lvl > 0.25 ? 0.6 : 1.0
			);  
			if (chase(see_buddy, rs) && attack_time < ms_count)  {
				do_act(ACT_BUILD);
				attack_time = ms_count + 1000;
				if (see_buddy->hp <= 0) { // ko instead of dead.
				    if (irand(20) < worker_skill) {
				    	static float medic_skill_table[6] = {0., 0.00, 0.15, 0.33, 0.66, 1.00 };
				    	see_buddy->energy *= medic_skill_table[MID(1,worker_skill,5)];
				    	see_buddy->forget_all();
				    	see_buddy->feel(emot_none);
				    	hud->long_news((char *)"%s revives %s.",name, see_buddy->name);
				    }
				} else see_buddy->hp = MIN( see_buddy->hp + 1 + 3 * worker_skill, see_buddy->mhp);		
			};    			    
		} else {
				if (max_interesting > 1) {
					chase(see_buddy, 0.3);    			    
				} else wander();
		} break;    
    case 'M': /*maintainance */     
    case 'V': /*vet */ 
    	// skill levels increase damage repair rate and reduces fatigue from work.
		if (fix_it && fix_it->alive()) { 
			float danger_lvl = fix_it->mhp ? fix_it->hp / fix_it->mhp : 1.;
			float rs = (
				danger_lvl > 0.9 ? 0.4 :
				danger_lvl > 0.6 ? 0.5 :
				danger_lvl > 0.15 ? 0.6 : 1.0
			);  			
			if (chase(fix_it, rs) && attack_time < ms_count)  {
				do_act(ACT_BUILD);
				static int maintenance_energy_penalty[6] = {0, 60, 30, 10, 5, 0 };
				attack_time = ms_count + 1000;
				fix_it->hp = MIN( fix_it->hp + (job == 'M' ? 15 + 10 * worker_skill : 1 + 3 * worker_skill), fix_it->mhp);
				int mip = maintenance_energy_penalty[MID(1,worker_skill,5)];
				// worker will become fatigued repairing buildings (sharp drop avoids ai ping-ponging)
				energy = (energy - mip < 100 ? MIN(50, energy) : energy - mip);		
			};    			    
		} else {
				if (max_interesting > 2) {
					chase(see_buddy, 0.3);    			    
				} else wander();
		} break;
    case 'C': /*care taker*/ 
		if (fix_it && fix_it->alive()) { 
			// lvl 1 guesses for a good can.
			// lvl 2 will pick cans that an animal would eat.
			// lvl 3-5 will pick cans an animal likes.
			// later: lvl 4-5 make animals happy also.
			// skill levels give bonus to can opening speed.  
			int which_can = -1;
			if (worker_skill == 1 )  { 
				int can = irand(4);
				if ( hud->food[can]) which_can = can; 
			} else if (worker_skill >= 3 && fix_it->likes_to_eat(&mb_stalkplant) && hud->food[1]) {
				which_can = 1;
			} else if (worker_skill >= 3 && fix_it->likes_to_eat(&mb_furbit) && hud->food[2]) {
				which_can = 2;
			} else if (worker_skill >= 3 && fix_it->likes_to_eat(&mb_mubark) && hud->food[3]) {
				which_can = 3;
			} else if (fix_it->will_eat(&mb_stalkplant) && hud->food[1]) {
				which_can = 1;
			} else if (fix_it->will_eat(&mb_furbit) && hud->food[2]) {
				which_can = 2;
			} else if (fix_it->will_eat(&mb_mubark) && hud->food[3]) {
				which_can = 3;
			} else if (
				(fix_it->will_eat(&mb_fern) || 
				fix_it->will_eat(&mb_shrub) || 
				fix_it->will_eat(&mb_happytree) ) && hud->food[0]
			) {
				which_can = 0;
			} else for (int can = 4; can >= 0; can-- ) if ( hud->food[can]) { 
				//can 4 == the unlabeled can strikes again!
				which_can = can; break;
			}
			if (chase(fix_it, 0.3) && attack_time < ms_count && which_can >= 0)  {
				do_act(ACT_BUILD);
				attack_time = ms_count + 3000 - 500 * worker_skill;
				fix_it->feed(which_can);
				hud->food[which_can]--;		
			};    			    
		} else {
			if (max_interesting > 2) {
				chase(see_buddy, 0.3);    			    
			} else wander();
		} break; 

   
    default: wander();
    }
}


void monst_t::merchant_ai() {
/*  flys to near base in normal situations or engauge enemys and try to jump out if attacked 
    and run and jump out if spooked.
	will fly to near your ship if you are trading with them and delay 
	jumping out (unless spooked).	
	hopefully i fixed the number bounce bug
	uses: aiu1 for state data  */
    hunger_level = 0;
    fear_hunger_pick(); // see_monst is either scary or closest. the ai only cares about scary.
    if (scared_of(this->see_monst) || who_hit_me_last) {
	    int jot = 18000 + ms_count;
		if (trader == trader_ui && aiu1 != 131313 ) {
	          trader_ui->say((char *)"Oh s***!  I'm getting out of here!");
	          aiu1 = 131313; // really unlucky.
	    }
		if (jot < jump_out_time) {
		   jump_out_time = jot;
		} else if (trader == trader_ui && jump_out_time < 16300 + ms_count ) {
		     trader->scroll_out();
	    }
		weap_t *w = armed();
		if (w && who_hit_me_last) { // attack
			feel(emot_pissed);
			float dy = this->who_hit_me_last->y - this->y;
			float dx = this->who_hit_me_last->x - this->x;
			float tang = afix(atan2(dy, dx ) - ang);
			float d = z_dist(dy,dx) - radius - who_hit_me_last->radius;
			float v = (d < w->t->rng / 2 ? -0.5 : 1.0);
			walk(tang, v, 0.);
			if (d < w->t->rng) {
				fire_at(who_hit_me_last);
			}
		} else evade(this->see_monst);				
	} else if (trader == trader_ui) {
	    if ( jump_out_time -ms_count < 10000 ) {
		    jump_out_time = ms_count + 10000;
		}
		monst_t *look_monst = ship;
		float ta = atan2(look_monst->y - y, look_monst->x - x);
	    float tang = afix(ta - ang);
    	float fd = ( dist(look_monst->x, look_monst->y,
			(this->x  + cos(this->ang)  * this->radius  * 1.1) ,
			(this->y  + sin(this->ang)  * this->radius  * 1.1)
			) - (look_monst->radius)
		);
		// merchant goes to comfortable distance facing your ship.
		walk(tang, fd > 75 ? 0.20 : fd < 60 ? -0.08 : 0., 0.); 		    
	} else {
		monst_t *look_monst = mainbase;
		float fd = ( dist(look_monst->x, look_monst->y,
			(this->x  + cos(this->ang)  * this->radius  * 1.1) ,
			(this->y  + sin(this->ang)  * this->radius  * 1.1)
			) - (look_monst->radius)
		);
		if (fd > 220) aiu1 = 0; 
		if (fd > 180 && aiu1 == 0) {
			float ta = atan2(look_monst->y - this->y,   look_monst->x - this->x);
			walk(ta - ang, 0.20, 0.0);
		} else {
			float rezero = afix(M_PI - ang);
			if (fabs(rezero) > 0.0001) {
				walk(rezero, 0., 0.);
			}
			aiu1 = 444; // the length of the natchez trace parkway in miles.		
		}	
	}
}

void monst_t::tourist_ai() {
  	// Wander Aimlessly and try to jump out if attacked
  	// run from raiders and meteors (but longer sillier run arround)
  	hunger_level = 0;
    fear_hunger_pick(); // see_monst is either scary or closest. the ai only cares about scary.
    if (scared_of(this->see_monst)) {
		feel(emot_surprized);
		int jot = 10000 + ms_count;
		if (jot < jump_out_time) jump_out_time = jot;
		evade(this->see_monst);
	} else wander(who_hit_me_last ? 1.0 : 0.3);
  	if (who_hit_me_last) {
		feel(emot_surprized);
		int jot = 4000 + ms_count;
		if (jot < jump_out_time) jump_out_time = jot;
    }
}

static monst_base* raider_collect[] = {
	&mb_stormhand,  &mb_morningstar, &mb_sunstriker, &mb_rumbler, &mb_railgun, &mb_lancer,
	&mb_spear, NULL
};


static monst_base* raider_is_boys_with[] = {
	&mb_raider, &mb_raidship, &mb_raidercamp,
	&mb_raiderguntower, &mb_raiderdgtower, &mb_raiderxcannontower, &mb_raidergaltixtower,
	&mb_raidersolararray, &mb_raiderfissionplant, &mb_raiderfusionplant,
	NULL
};

static monst_base* hunters_like_to_kill[] = {
	&mb_raidship,	&mb_raider,	&mb_mimmack,	&mb_furbit,	&mb_pygment,
	&mb_shruver,	&mb_roachy,	&mb_krulasite,	&mb_mubark,	&mb_throbid,
	&mb_squarger,	&mb_quitofly,	&mb_mamothquito,&mb_toothworm,	&mb_sneeker,
	&mb_squeeker,	&mb_meeker,	&mb_molber,	&mb_drexler,	&mb_bigasaurus,
	&mb_uberworm,	&mb_dragoonigon,&mb_krakin,	&mb_bervul,	&mb_rexrex,
	&mb_vaerix,	&mb_yazzert,	&mb_frogert,	&mb_ackatoo,	&mb_quiver,
	&mb_slymer,	&mb_crysillian,	&mb_stalker,	&mb_titurtilion,&mb_golgithan,
	NULL
};

int monst_t::huntraid_friendly(monst_t *m) {
       return ( !m || m->dead() || m->isa_plant() || m->isa_item()  || ((m->flags & AF_NOSHOOT) == 1) ||
              (this->base == &mb_hunter ?
              m->base == &mb_hunter  :
              m->check_mb_list(raider_is_boys_with)
              )
        );
}

void monst_t::hunter_ai() {
	if (!my_ast || (fear_dist <= 0)) return;
    if (_fire_run_ai() || _krulasite_inf_ai()) return;
	float biggest = 0.0, tang = 0., biggest_dist = 0.;
	monst_t *big_monst = NULL;
	hunger_level =  0;
        feel( tired() ? emot_tired : emot_none);
        if (who_hit_me_last) {
           if ( huntraid_friendly(who_hit_me_last) || who_hit_me_last->dead() ) who_hit_me_last = NULL;
           else see_monst = who_hit_me_last;
        }
        weap_t *w = armed();
        if (!w && (base == &mb_raider || base == &mb_hunter) ) {
          	float closest = my_ast->pw;   monst_t *get_weap = NULL;
		for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
			monst_t *look_monst = *mon;
			if ( look_monst->check_mb_list(raider_collect) ) {
                        	float sd = z_dist(look_monst->x - this->x, look_monst->y - this->y);
				if (sd < closest) { get_weap = look_monst; closest = sd; }
			}
		}
		if (get_weap) {
    			chase(get_weap); take_weap(get_weap);
		} else wander(0.5);
		return;
        }
	if (!who_hit_me_last) for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
		monst_t *look_monst = *mon;
		if (look_monst == this) continue;
		float fd = dist(look_monst->x, look_monst->y,
					(this->x  + cos(this->ang)  * this->radius  * 1.1) ,
					(this->y  + sin(this->ang)  * this->radius  * 1.1)
				    ) - (look_monst->radius);
		if (look_monst->alive()  &&  ((fd  < this->fear_dist) || (look_monst == who_hit_me_last )) &&
                	(this->base == &mb_hunter ?
                	look_monst->check_mb_list(hunters_like_to_kill) :
                	!huntraid_friendly(look_monst) && look_monst->alive()
                	)
		) {
                        float fr = fd / fear_dist;
                        float ff = look_monst->size * (fr > 0.5 ? 1. : fr > 0.2 ? 5.0 : 8. );
			if (ff > biggest) {
				big_monst = look_monst;
				biggest_dist = fr;
				biggest = ff;
			}
  		}
                if (big_monst) this->see_monst = big_monst;
        }
        if (see_monst && see_monst->dead()) see_monst = NULL;
	if (this->see_monst && w) { // attack
		feel(emot_pissed);
		float dy = this->see_monst->y - this->y;
		float dx = this->see_monst->x - this->x;
		tang = afix(atan2(dy, dx) - ang);
		float ld = z_dist(dy,dx);
		float sd = ld - radius - see_monst->radius;
		float v = (sd < w->t->rng / 2 ? -0.5 : 1.0);
                for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
                         monst_t *bys  = *mon;
		         float bdy = bys->y - this->y;
		         float bdx = bys->x - this->x;
		         float bd = MAX(1., z_dist(bdy, bdx));
                         if (bd >= sd) continue;
                         // float btang = afix(this->ang - atan2(bdy, bdx));
                         float adf = fabs(afix(atan2(dy, dx) - atan2(bdy, bdx)));
                         if (
                         	bys->alive() && !bys->isa_plant() &&
                          	bys->ai_func != ai_item && huntraid_friendly(bys) &&
                          	atan(bys->radius) / bd < adf
                  	) {
                            // avoid friendly fire
                            int am = ((aiu1 + ms_count) / 1200) % 3;
                            switch (am) {
                            case 0: walk(tang, v / 2., 1.0);
                            case 1: walk(tang, v / 2., -1.0);
                            case 2: wander(1.0);
                            }
                            return;
                        }
                }
                walk(tang, v, 0.);
                if (sd < w->t->rng) fire_at(see_monst);
	} else wander(0.5);
}

void monst_t::turret_ai()  {
	if (!my_ast || !pow_on) return;
	float biggest = 0.0, tang = 0., biggest_dist = 0.;
	monst_t *big_monst = NULL;
	hunger_level =  0;
        feel( tired() ? emot_tired : emot_none);
        if (who_hit_me_last) {
           if ( org_idx == org_ast0 ?
                hates(who_hit_me_last)
                : huntraid_friendly(who_hit_me_last) ) who_hit_me_last = NULL;
           else see_monst = who_hit_me_last;
        }
        weap_t *w = armed();
	if (!who_hit_me_last && w) for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
		monst_t *look_monst = *mon;
		if (look_monst == this) continue;
		float wp_rng = MIN(0.85 * w->t->rng, 600);
		float fd = z_dist(look_monst->x - x, look_monst->y - y) - (radius + look_monst->radius);
		int aix = ( (int)(64. * atan2(look_monst->y - y, look_monst->x - x) / M_PI) & 0x7f);
		int shoot_ok = (!((target_ang[aix / 32] >> (aix % 32)) & 1));
		if ( shoot_ok && look_monst->alive()  &&  (fd  < wp_rng)  &&
                        ( org_idx == org_ast0 ? hates(look_monst) :
                	 !huntraid_friendly(look_monst)
                	)
		) {
                        float fr = fd / wp_rng;
                        float ff = look_monst->size * (look_monst == who_hit_me_last ? 6. : 1.) *
                              (fr > 0.5 ? 1. : fr > 0.2 ? 5.0 : 8. ) * (look_monst->armed() ? 6. : 1.);
			if (ff > biggest) {
				big_monst = look_monst;
				biggest_dist = fr;
				biggest = ff;
			}
  		}
                if (big_monst) this->see_monst = big_monst;
        }
        if (see_monst && see_monst->dead()) see_monst = NULL;
	if (see_monst && w) { // attack
		float dy = this->see_monst->y - this->y;
		float dx = this->see_monst->x - this->x;
		tang = afix(atan2(dy, dx) - wang);
		float d = z_dist(dy,dx) - radius - see_monst->radius;
		float max_step = TURRET_TR * M_PI / 180. / TURNS_PER_SEC;
		if (fabs(tang) >  max_step) {
                	if (tang < 0.) wang -= max_step; else wang += max_step;
    		} else {
        		for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
                         monst_t *bys  = *mon;
		         float bdy = bys->y - this->y;
		         float bdx = bys->x - this->x;
		         float bd = MAX(1., z_dist(bdy, bdx));
                         float adf = fabs(afix(atan2(dy, dx) - atan2(bdy, bdx)));
                         if (bd > d || bys == see_monst || bys == this || adf > M_PI / 2. || d < 0.001 ) continue;
                         if ( // broken!
                         	bys->alive() && !bys->isa_plant() &&
                          	bys->ai_func != ai_item &&
                          	(org_idx == org_ast0 ? bys->org_idx >= org_ast0 && alignment.align(bys) >= 0 : huntraid_friendly(bys)) &&
                          	atan(bys->radius / bd) < adf
                  	) {
                            // avoid friendly fire
                            return;
                        }
                	}
                	if (d < w->t->rng) fire_at(see_monst);
      		}  	
	}
}

void monst_t::dragoonigon_ai() {
        // dragoonigons are designed to make pases with dragon fire
        // goes beserk if starving or badly injured
	if (!my_ast || (fear_dist <= 0)) return;
	if (_krulasite_inf_ai() || _slap_and_tickle_ai()) return;
	float hung_per = (stomach == 0 ? 1.0 : in_stomach / stomach);
	hunger_level =  ( hung_per > 0.5 ? 0 : hung_per > 0.20 ? 1 : 2);
	if (hung_per < 0.05 || hp < mhp / 2) flags |= AF_BESERK;
        feel( hunger_level ? emot_hungry : tired() ? emot_tired : emot_none);
        fear_hunger_pick();
        if (!this->see_monst) this->see_monst = who_hit_me_last;
	if (this->see_monst) {
                float dy = this->see_monst->y - this->y;
		float dx = this->see_monst->x - this->x;
		float d = z_dist(dy,dx);
		float tang = afix(atan2(dy, dx) - ang);
		float Vv = z_dist(vy,vx);
		float Va = fabs(afix(atan2(vy,vx) - atan2(dy, dx)));
		int hate = hates(see_monst);
		weap_t *w = armed();
		int debug_ai = 1;
   		if (hate || would_eat(see_monst)) {
       			int am = (aiu1 + ms_count) % (9000 );
                        if (am < 2220) {
                        	wander(0.8);   // make pass2
                        	if (debug_ai) feel(emot_sleep);
                        } else {
                  		if (hate) feel(emot_pissed);
                                if (d < 1.2 * radius) {
                                  	walk(0., 1.0, 0.); // fly by
                                  	if (debug_ai) feel(emot_bored);
                                } else if (d > 250) {
                                   	chase(see_monst);
                  		  	if (debug_ai) feel(emot_pissed);
                                } else {
                                        if (debug_ai) feel(emot_happy);
                                  	walk(tang, MAX(0.15, fabs(tang / M_PI)), 0.); // counter-attack
                                  	
                                }
     			}
			if (fabs(tang) < M_PI / 2. && w && d < w->t->rng && (Vv < 8. || Va < M_PI / 6.)) fire_at(see_monst);
		} else if (scared_of(see_monst)) evade(see_monst);
	} else wander(0.15, 27.0);
}

void monst_t::beholder_ai()  {
        // pretty close to standard animal but fires psy attacks at things its scared of
	if (!my_ast || (fear_dist <= 0)) return;
        if (_fire_run_ai() || _krulasite_inf_ai() || _slap_and_tickle_ai()) return;
	float hung_per = (stomach == 0 ? 1.0 : in_stomach / stomach);
	hunger_level =  ( hung_per > 0.5 ? 0 : hung_per > 0.20 ? 1 : 2);
        feel( hunger_level ? emot_hungry : tired() ? emot_tired : emot_none);
        fear_hunger_pick();
        if (!this->see_monst) this->see_monst = who_hit_me_last;
	if (this->see_monst) {
        	float dy = this->see_monst->y - this->y;
		float dx = this->see_monst->x - this->x;
		float d = z_dist(dy,dx);
		float tang = afix(atan2(dy, dx) - ang);
		int hate = hates(see_monst);
		weap_t *w = armed();
   		if (hate && w) {
       			feel(emot_pissed);
       			float v = (d < 100. ? -(100. - d) / 100. : d > w->t->rng * 0.9 ? 1. : 0.);
			walk(tang, v,  0.);
			if (fabs(tang) < M_PI / 3. && w && d < w->t->rng) fire_at(see_monst);
		} else if (would_eat(see_monst)) {
    			chase(see_monst);
		} else if (scared_of(see_monst)) {
			if (d > fear_dist && w) {
     				feel(emot_pissed);
                                int am = (aiu1 + ms_count) % (9000 + (int)(3000. * cos((ms_count + aiu1 ^ 0xa3512c) /27000.)));
                            	if (am < 620) wander(0.8);   // dodge
                                else walk(tang, fabs(tang / M_PI), 0.); // counter-attack
     			} else {
          			evade(see_monst);
				feel(emot_surprized);
                     	}
			if (fabs(tang) < M_PI / 3. && w && d < w->t->rng) fire_at(see_monst);
        	}
	} else if (flags & AF_WANDERS) wander();
	else if (flags & AF_FLYING) {
          float exp = frand() * 0.04 - 0.02;
          float lat_exp = frand() * 0.04 - 0.02;
          walk(0., exp,lat_exp);
   	} else walk(0,0,0);
}

void monst_t::meeker_ai() {
	if (!my_ast || (fear_dist <= 0)) return;
        if (_fire_run_ai() || _krulasite_inf_ai() || _slap_and_tickle_ai()) return;
	float hung_per = (stomach == 0 ? 1.0 : in_stomach / stomach);
	hunger_level =  ( hung_per > 0.5 ? 0 : hung_per > 0.20 ? 1 : 2);
        feel( hunger_level ? emot_hungry : tired() ? emot_tired : emot_none);
        fear_hunger_pick();
        if (!this->see_monst) this->see_monst = who_hit_me_last;
	if (this->see_monst) {
                float dy = this->see_monst->y - this->y;
		float dx = this->see_monst->x - this->x;
		float d = MAX(0, z_dist(dy,dx) - radius - see_monst->radius);
		float tang = afix(atan2(dy, dx) - ang); // + (M_PI / 8. * (aiu1 & 1 ? 1 : -1)));
		float Vv = z_dist(vy,vx);
		float Va = fabs(afix(atan2(vy,vx) - atan2(dy, dx)));
		int hate = hates(see_monst);
		weap_t *w = armed();
   		if ( (hate || would_eat(see_monst))) {
       			int clear_shot = 1;
       			float Aa = atan2(dy, dx);
                        float Vw = (w->t->vel / TURNS_PER_SEC);
                        float Af = atan2(vy + sin(Aa) * Vw, vx + cos(Aa) * Vw);
                        if (hate) feel(emot_pissed);
               		float combat_dist = 250.;
                        if (d < 2. * radius) {
                               	walk(0., 1.0, 0.); // fly by
                        } else if (d > combat_dist) {
                        	chase(see_monst);
                  	} else {
                               	float cang = tang + ((combat_dist - d)/ combat_dist) * (aiu1 & 1 ? 1. : -1) * M_PI / 2. ;
                               	walk(cang, MAX(0.5, fabs(tang / M_PI)), 0.); // counter-attack
     			}
			if ( w && d < w->t->rng && (Vv < 10. || Va < M_PI / 5.)) fire_at(see_monst);
			wang = atan2(dy, dx);
		} else if (scared_of(see_monst)) {
            		int state = (
         			(energy <  100) ? 0 : // Run
        			(energy <  500) ? 3 : // Kamakazi
        			(energy < 1000) ? 4 : // Blast
        			((aiu1 + ms_count) % 30000 / 10000) // 0 = run   1 = teleport  2 = defensive fight
        		);
                        switch (state) {
    			case 0 : evade(see_monst); break;
                        case 1 : if (d < 150) {
                          if (hud->hooked_monst == this) hud->hooked_monst = NULL;
                          my_ast->add_jumpgate(x, y, SPR_TELEPORT);
                          x = irand(my_ast->pw);   y = irand(my_ast->ph);
                          energy -= 60;
                        } else evade(see_monst);
                        break;
                        default: evade(see_monst); break;  // later do rest of states
                        }
        	}
	} else wander(0.15);
}


void monst_t::tank_ai() {
        // turreted mecha and tank design.
	if (!my_ast || (fear_dist <= 0)) return;
	if (_krulasite_inf_ai() || _slap_and_tickle_ai()) return;
	float hung_per = (stomach == 0 ? 1.0 : in_stomach / stomach);
	hunger_level =  ( hung_per > 0.5 ? 0 : hung_per > 0.20 ? 1 : 2);
        feel( hunger_level ? emot_hungry : tired() ? emot_tired : emot_none);
        fear_hunger_pick();
	if (see_monst && see_monst->dead()) see_monst = NULL;
	if (who_hit_me_last && who_hit_me_last->dead()) who_hit_me_last = NULL;
        if (!see_monst) see_monst = who_hit_me_last;
	if (see_monst) {
                float dy = see_monst->y - y;
		float dx = see_monst->x - x;
		float d = MAX(0, z_dist(dy,dx) - radius - see_monst->radius);
		float tang = afix(atan2(dy, dx) - ang); // + (M_PI / 8. * (aiu1 & 1 ? 1 : -1)));
		float Vv = z_dist(vy,vx);
		float Va = fabs(afix(atan2(vy,vx) - atan2(dy, dx)));
		int hate = hates(see_monst);
		float hi_vel = (d / 300.);
		int no_exp = (d < 200);
                int best_weap = -1;
                int best_weap_rating = 0;
                int chkw = 0;                
		for (list<weap_t *>::iterator wp = weap.begin(); wp != weap.end(); wp++, chkw++) if ((*wp)->t->typ == inv_proj) {
			int wr = 0;
			weap_t *w = *wp;
			if (d > w->t->rng) continue;
			if (w->t->wtyp & WP_EXPL && w->t->drg > 0) {
                        	float inr = d / w->t->drg;
                                wr += (inr < 0.5 ? -5000 : inr < 0.7 ? -1000 : inr < 1.0 ? -100 : inr < 1.5 ? 0 : 200);
     			}
     			float refmult = 1000. / MID(100., w->t->relms, 700.);
     			wr += (int)((hi_vel * w->t->vel) + (w->t->sv * refmult));
     			if (wr > best_weap_rating) { best_weap_rating = wr; best_weap = chkw; }
  		}
		toggle_weap(best_weap);
                weap_t *w = (best_weap >= 0 ? armed() : NULL);
   		if ( (hate || would_eat(see_monst)) && w) {
       			int clear_shot = 1;
       			float Aa = atan2(dy, dx);
                        float Vw = (w->t->vel / TURNS_PER_SEC);
                        float Af = atan2(vy + sin(Aa) * Vw, vx + cos(Aa) * Vw);
       			for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
                         		monst_t *bys  = *mon;
		         		float bdy = bys->y - y;
		         		float bdx = bys->x - x;
		         		float bd =  z_dist(bdy, bdx);
                                        float adf = fabs(afix(Af - atan2(bdy, bdx)));
                         		if (bd >= d || bys == see_monst || bd < 0.001 || adf > M_PI / 2.) continue;
                         		if (
                         			bys->alive() && !bys->isa_plant() &&
                          			bys->ai_func != ai_item && !hates(bys) &&
                          			atan(bys->radius / bd) - ((M_PI / 2000.) * bd) < adf
                  			) {
                            			clear_shot = 0;
                            			break;
                        		}
                		}
       			int am = (aiu1 + ms_count) % (15500 );
                        if (am < 2620) {
                        	wander(0.8);   // make pass2
                        } else {
                                if (hate) feel(emot_pissed);
                                if (!clear_shot) {
                                  // avoid friendly fire
                            			int am = ((aiu1 + ms_count) / 1800) % 2;
                            			feel(emot_surprized);
                            			switch (am) {
                            			case 0: walk(tang + cos((aiu1 + ms_count) / 1000.), 0.7,0.);
                            			case 1: walk(tang - cos((aiu1 + ms_count) / 1000.), 0.7,0.);
                            			}
                                } else {
                  			float combat_dist = 250.;
                                	if (d < 2. * radius) {
                                  		walk(0., 1.0, 0.); // fly by
                                  		// feel(emot_tired);
                                	} else if (d > combat_dist) {
                                   		chase(see_monst);
                  		  		// feel(emot_pissed);
                                	} else {
                                        	float cang = tang + ((combat_dist - d)/ combat_dist) * (aiu1 & 1 ? 1. : -1) * M_PI / 2. ;
                                  		walk(cang, MAX(0.5, fabs(tang / M_PI)), 0.); // counter-attack
                                  		// feel(emot_dead);
                                	}
     				}
                	}
			if ( w && clear_shot && d < w->t->rng && (Vv < 10. || Va < M_PI / 5.)) fire_at(see_monst);
			wang = atan2(dy, dx);
		} else if (scared_of(see_monst)) evade(see_monst);
	} else wander(0.15);
}

static builder_table orick_bt[] = { {&mb_stalkplant, &mb_spear, 3000} };


monst_base* orick_collect[] = {
        &mb_deathray, &mb_sunstriker, &mb_railgun,
        &mb_stormhand, &mb_pulsar, &mb_lancer, &mb_morningstar, &mb_rumbler,
	&mb_spear, &mb_stalkplant, NULL
};

monst_base* worker_collect[] = { // workers aren't as crazy as oricks and dont use spears
        &mb_sunstriker, &mb_railgun,
        &mb_stormhand, &mb_pulsar, &mb_lancer, &mb_morningstar, &mb_rumbler,
	    NULL
};



void monst_t::soldier_ai() {
	// this ai trys to win figts by both armed and unarmed combat also tries to collect weapons
	if (!my_ast || (fear_dist <= 0)) return;
	if (_fire_run_ai() || _krulasite_inf_ai() || _slap_and_tickle_ai()) return;
	float biggest = 0.0, closest = my_ast->pw, iclosest = 0., biggest_dist = 0.;
	monst_t *big_monst = NULL, *close_monst = NULL, *best_item = NULL;
	float hung_per = (stomach == 0 ? 1.0 : in_stomach / stomach);
	hunger_level =  ( hung_per > 0.5 ? 0 : hung_per > 0.20 ? 1 : 2);
	feel( hunger_level ? emot_hungry : tired() ? emot_tired : emot_none);
	weap_t *w = armed();
	monst_base *best_base = (w ? w->t->mb_link : NULL);
	for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
		monst_t *look_monst = *mon;
		if (look_monst == this) continue;
		float fd = dist(look_monst->x, look_monst->y,
					(this->x  + cos(this->ang)  * this->radius  * 1.1) ,
					(this->y  + sin(this->ang)  * this->radius  * 1.1)
				    ) - (look_monst->radius);
		if (look_monst->alive()  &&  (fd  < this->fear_dist) && (
			hates(look_monst) || scared_of(look_monst) ||
			(look_monst == who_hit_me_last && !would_eat(look_monst)))
		) {
			float fr = fd / fear_dist;
			float ff = look_monst->size * (look_monst == who_hit_me_last ? 6. : 1.) *
				(fr > 0.5 ? 1. : fr > 0.2 ? 5.0 : 8. );
			if (ff > biggest) {
				big_monst = look_monst;
				biggest_dist = fr;
				biggest = ff;
			}
  		}
  		int cca = (
  			hunger_level > 1 ? 0 :
          		look_monst->check_mb_list_better((base == &mb_orick ? orick_collect : worker_collect), best_base) ||
          		( best_base == look_monst->base && fd < iclosest)
  		);
  		if (cca) {
			best_item = look_monst;
			iclosest = fd;
			best_base = best_item->base;
        }
		if ( would_eat(look_monst)  ) {
            float sd = z_dist(look_monst->x - x, look_monst->y - y);
			if (sd < closest) {
				close_monst = look_monst;
				closest = sd;
			}
		}
	}
	if (!big_monst) big_monst = who_hit_me_last;
	if (best_item && iclosest < 120.) {
	see_monst = best_item;
	} else if (big_monst || close_monst) {
		see_monst = (big_monst && close_monst ?
			(biggest_dist < hung_per * 1.2 ? big_monst : close_monst ) :
			big_monst ? big_monst : close_monst
		);
	} else see_monst = best_item;
	if (see_monst) {
        float dy = see_monst->y - y;
		float dx = see_monst->x - x;
		float d = z_dist(dy,dx);
		float tang = afix(atan2(dy, dx) - ang);
		int poon = 69;
		int hate = hates(see_monst);
		int eat = would_eat(see_monst);
		int scared = scared_of(see_monst);
		int clear_shot = 1;
		int face_ok = fabs(tang) < M_PI / 3;
		float Aa = atan2(dy, dx);
		float Vw = (w ? w->t->vel / TURNS_PER_SEC : 0.);
		float Af = atan2(vy + sin(Aa) * Vw, vx + cos(Aa) * Vw);
		float Sd = fabs(afix(Aa - Af));
		if ( (hate || eat || scared ) && w && see_monst->alive()) {
			// check for clear shot
			for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
				monst_t *bys  = *mon;
				float bdy = bys->y - y;
				float bdx = bys->x - x;
				float bd =  z_dist(bdy, bdx);
				float adf = fabs(afix(Af - atan2(bdy, bdx)));
				if (bd >= d || bys == see_monst || bd < 0.001 || adf > M_PI / 2.) continue;
				if (
					bys->alive() && !bys->isa_plant() &&
					bys->ai_func != ai_item && !hates(bys) &&
					atan(bys->radius / bd) - ((M_PI / 2000.) * bd) < adf
				) {
					clear_shot = 0;
					break;
				}
			}
		}
		if (hate && w && see_monst->alive()) {
			feel(emot_pissed);
			float v = (d < 100. ? -(100. - d) / 100. : d > w->t->rng * 0.9 ? 1. : 0.);
			int am = (aiu1 + ms_count) % (15500 );
			if (am < 2620) {
				wander(0.8);   // make pass2
			} else {
				if (hate) feel(emot_pissed);
				if (!clear_shot) {
					// avoid friendly fire
					int am = ((aiu1 + ms_count) / 1800) % 2;
					feel(emot_surprized);
					switch (am) {
					case 0: walk(tang + cos((aiu1 + ms_count) / 1000.), 0.7,0.);
					case 1: walk(tang - cos((aiu1 + ms_count) / 1000.), 0.7,0.);
					}
				} else {
					float combat_dist = 250.;
					walk(tang, v,  0.);
					#if 0
					if (d < 2. * radius) {
					walk(0., 1.0, 0.); // fly by
					// feel(emot_tired);
					} else if (d > combat_dist) {
					chase(see_monst);
					// feel(emot_pissed);
					} else {
						float cang = tang + ((combat_dist - d)/ combat_dist) * (aiu1 & 1 ? 1. : -1) * M_PI / 2. ;
					walk(cang, MAX(0.5, fabs(tang / M_PI)), 0.); // counter-attack
					// feel(emot_dead);
					}
					#endif
				}
			}
			if ( w && clear_shot && face_ok && d < w->t->rng ) fire_at(see_monst);
			//wang = atan2(dy, dx);
		} else if (eat || hate) {
			chase(see_monst);
			if (
					w && clear_shot && face_ok &&
					eat && see_monst->alive() &&
					d > 60. && d < w->t->rng && Sd < M_PI / 10.
			) fire_at(see_monst);
		} else if (scared) {
			if (d > fear_dist && w) {
				feel(emot_pissed);
				int am = (aiu1 + ms_count) % (9000 + (int)(3000. * cos((ms_count + aiu1 ^ 0xa3512c) /27000.)));
				if (am < 620) wander(0.8);   // dodge
				else walk(tang, fabs(tang / M_PI), 0.); // counter-attack
			} else {
				evade(see_monst);
				feel(emot_surprized);
			}
			if (w && clear_shot && face_ok && d < w->t->rng) fire_at(see_monst);
		} else if (see_monst == best_item) {
			if (d < 80. && w) drop_weap();
			chase(see_monst);
			take_weap(see_monst, base==&mb_orick ? orick_bt : NULL);
		}
	} else if (flags & AF_WANDERS) wander(); else walk(0,0,0);
}


void monst_t::krulasite_ai() {
	// go after the biggest monster on our foodchain (includes almost
	// everything organic, except plants!)  bonus for being strapped.
	// if nothing is infectable: scavange the dead.
	if (!my_ast || (fear_dist <= 0)) return;
	float biggest = 0.0, tang = 0., biggest_dist = 0., dead_dist = 0.;
	monst_t *big_monst = NULL;
	monst_t *the_dead = NULL;
	hunger_level =  2;
	for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
		monst_t *look_monst = *mon;
		if (look_monst == this || look_monst->krulasite_inf) continue;
		float fd = dist(look_monst->x, look_monst->y,
					(this->x  + cos(this->ang)  * this->radius  * 1.1) ,
					(this->y  + sin(this->ang)  * this->radius  * 1.1)
				    ) - (look_monst->radius);
		if (look_monst->dead()) {
			if (would_eat(look_monst) && (!the_dead || fd < dead_dist)) {
				the_dead = look_monst; dead_dist = fd;
			}
		} else if (would_eat(look_monst)  &&  ((fd  < this->fear_dist) || (look_monst == who_hit_me_last ))
		) {
                        float fr = fd / fear_dist;
                        float ff = look_monst->size * (look_monst == who_hit_me_last ? 6. : 1.) *
                              (fr > 0.5 ? 1. : fr > 0.2 ? 5.0 : 8. ) * (look_monst->armed() ? 6. : 1.);
			if (ff > biggest) {
				big_monst = look_monst;
				biggest_dist = fr;
				biggest = ff;
			}
  		}
        }
	this->see_monst = (big_monst ? big_monst : the_dead);
        if (!this->see_monst) this->see_monst = who_hit_me_last;
	if (this->see_monst) {
		// chace see_monst
		tang = afix( atan2(this->see_monst->y - this->y,
						this->see_monst->x - this->x) - ang);
		if (tang > M_PI)  tang -= 2.0 * M_PI;
		if (tang < -M_PI) tang = 2.0 * M_PI + tang;
		if (fabs(tang) < turn_rate) walk(tang, 1.0, 0.); else walk(tang,0.84,0.);
		if (armed()) fire_at(see_monst);
	}
}

int monst_t::_krulasite_inf_ai() {
	// standard rules unless krulasite infection has < 2 minutes.
        if (! krulasite_sym()) return 0;
	if (ms_count > krulasite_inf) {
   		// host monster dies and new krulasites jump out if the Krulasites are ready to hatch.
   		hit(hp);
            	return 1;
   	}
	float closest = 0.0, tang = 0.;
	monst_t *close_monst = NULL;
	hunger_level = 3;
        for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
		monst_t *look_monst = *mon;
		if (look_monst == this) continue;
		if ((look_monst->base == this->base || would_eat(look_monst)) &&
		    ! (look_monst->krulasite_inf || look_monst->base == &mb_krulasite)) {
                        float sd = z_dist(look_monst->x - this->x, look_monst->y - this->y);
			if (sd < closest) {
				close_monst = look_monst;
				closest = sd;
			}
		}
                if (close_monst) this->see_monst =  close_monst;
        }

	if (this->see_monst) {
   		feel(emot_pissed);
		tang = afix(atan2(see_monst->y - y, see_monst->x - x) - this->ang);
  	        if (fabs(tang) < M_PI / 4) walk(tang, 1.0, 0.); else walk(tang,0.84,0.);
	} else wander(0.8);
   	return 1;
}

static monst_base* crysillian_like_to_guard[] = {
	&mb_crysillianruins, &mb_crashsite,
 	&mb_astbase, &mb_jmart,	&mb_bfc, &mb_arena,
	&mb_wisehut, &mb_policestation,
	&mb_organicsscanner, &mb_deltawavescanner, &mb_qparticlescanner, &mb_epsilonhiscanner,
	&mb_solararray,	&mb_fissionplant, &mb_fusionplant,
	NULL
};

void monst_t::crysillian_ai() {
	if (!my_ast || (fear_dist <= 0)) return;
	if (_krulasite_inf_ai()) return;  // nearly imortal no sex
	float biggest = 0.0, biggest_dist = 0., guard_dist = my_ast->pw;
        monst_t *guard_what = NULL;
	monst_t *big_monst = NULL;
	hunger_level =  0;
        feel( tired() ? emot_tired : emot_none);
	if (who_hit_me_last) {
        	see_monst = who_hit_me_last;
        }
	if (!who_hit_me_last) for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
		monst_t *look_monst = *mon;
		if (look_monst == this) continue;
		float fd = dist(look_monst->x, look_monst->y,
					(this->x  + cos(this->ang)  * this->radius  * 1.1) ,
					(this->y  + sin(this->ang)  * this->radius  * 1.1)
				    ) - (look_monst->radius);
		if (look_monst->check_mb_list(crysillian_like_to_guard) && fd < guard_dist) {
			guard_what = look_monst;
			guard_dist = fd;
    		}
		if (look_monst->alive()  &&
			((fd  < this->fear_dist) || (look_monst == who_hit_me_last )) &&
			hates(look_monst)
		) {
                        float fr = fd / fear_dist;
                        float ff = look_monst->size * (look_monst == who_hit_me_last ? 6. : 1.) *
                              (fr > 0.5 ? 1. : fr > 0.2 ? 5.0 : 8. );
			if (ff > biggest) {
				big_monst = look_monst;
				biggest_dist = fr;
				biggest = ff;
			}
  		}
                if (big_monst) this->see_monst = big_monst;
        }
        if (guard_what && !who_hit_me_last) {
        	if (guard_what->hp / guard_what->mhp > 0.9 && guard_what->who_hit_me_last) {
           		who_hit_me_last = guard_what->who_hit_me_last;
            	}
  	}
	if (who_hit_me_last) {
		feel(emot_pissed);
                chase(who_hit_me_last);
/*
		int hate = hates(see_monst);
   		if (hate || would_eat(see_monst)) {
       			// chace see_monst
			if (hate) feel(emot_pissed);
                        chase(see_monst);
		} else if (scared_of(see_monst)) {
    			evade(see_monst);
		}
*/
	} else if (guard_what) {
   		// patrol
       		if (dist(this->run_x, this->run_y, this->x, this->y) < 20. ||
			this->run_x == 0 && this->run_y == 0 // || guard_dist < 70
		) {
                	float sd = frand() * 150. + 85.;
                	float sa = frand() * 2. * M_PI;
                	this->run_x = MID(50,(int)(guard_what->x + cos(sa) * sd) ,ast->pw - 50);
                	this->run_y = MID(50,(int)(guard_what->y + sin(sa) * sd) ,ast->ph - 50);
		}
		float ta = atan2(this->run_y - this->y,   this->run_x - this->x);
		float tang = ta - ang;
		walk(tang, 0.3, 0.);
   	}
}

static builder_table stalker_bt[] = { {&mb_stalker, &mb_vojodust, 12000} };

static monst_base* cheech_collect[] = {
	&mb_marjacola, &mb_ihb77, &mb_vojodust, &mb_zozin, &mb_lykedofan,
	&mb_detox, &mb_rozolin, &mb_glowingliquid, &mb_superstim, &mb_jr19azf,
	&mb_aisormba, &mb_hx12, &mb_morodulb, &mb_project17,
	&mb_redrock, &mb_yellowrock, &mb_purplerock, &mb_greencrystals,
        &mb_unlabeledcan, &mb_mixedvegetables, &mb_eyestalkshoots, &mb_cannedfurbit,
        &mb_steamedmubark, NULL
};


static monst_base* cheech_collect_dead[] = {
	&mb_fern, &mb_stalkplant, &mb_happytree, &mb_roachy, &mb_toothworm, NULL
};

static builder_table cheech_bt[] = {
	{&mb_fern, &mb_marjacola, 5000},
	{&mb_stalkplant, &mb_ihb77,  5000},
	{&mb_happytree, &mb_zozin, 8000},
	{&mb_greencrystals, &mb_glowingliquid, 15000},
	{&mb_redrock, &mb_superstim, 10000},
	{&mb_toothworm, &mb_morodulb, 10000},
	{&mb_roachy, &mb_project17, 10000}
};


void monst_t::cheech_ai() {
	// cheeches are real drug addicts and also quite good chemists
	// they try to always stay high.  They will try to keep 5 drugs on their person
	// when they can.  They also are opertunistic with stuff that takes animal parts.
	// They get anxious if they are out of drugs and begin to wander
	// arround.
	if (!my_ast || (fear_dist <= 0)) return;
        if (_fire_run_ai() || _krulasite_inf_ai() || _slap_and_tickle_ai()) return;
	float biggest = 0.0, closest = my_ast->pw, biggest_dist = 0.;
	monst_t *big_monst = NULL, *close_monst = NULL;
	float hung_per = (stomach == 0 ? 1.0 : in_stomach / stomach);
	hunger_level =  ( hung_per > 0.5 ? 0 : hung_per > 0.20 ? 1 : 2);
        feel( hunger_level ? emot_hungry : tired() ? emot_tired : emot_none);

	int drugs = 0;
        for(list<weap_t *>::iterator wp = weap.begin(); wp != weap.end(); wp++) {
        	weap_t *w = *wp;
        	if (w->t->typ == inv_injector || w->t->typ == inv_food) drugs += w->count;
       	}
	for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
		monst_t *look_monst = *mon;
		if (look_monst == this) continue;
		float fd = dist(look_monst->x, look_monst->y,
					(this->x  + cos(this->ang)  * this->radius  * 1.1) ,
					(this->y  + sin(this->ang)  * this->radius  * 1.1)
				    ) - (look_monst->radius);
		if (look_monst->alive()  &&  (fd  < this->fear_dist) && (scared_of(look_monst) ||
                	(look_monst == who_hit_me_last && !would_eat(look_monst)))
		) {
                        float fr = fd / fear_dist;
                        float ff = look_monst->size * (look_monst == who_hit_me_last ? 6. : 1.) *
                              (fr > 0.5 ? 1. : fr > 0.2 ? 5.0 : 8. );
			if (ff > biggest) {
				big_monst = look_monst;
				biggest_dist = fr;
				biggest = ff;
			}
  		}
                int ccd = (
                	look_monst->check_mb_list(cheech_collect_dead) &&
                	( look_monst->dead() || (look_monst->base != &mb_toothworm && drugs < 2) )
                );
                int cca = look_monst->check_mb_list(cheech_collect) && drugs < 5;
		if ( would_eat(look_monst) || ccd || cca ) {
                        float sd = z_dist(look_monst->x - this->x, look_monst->y - this->y) / (ccd ? 4. : 1.);
			if (sd < closest) {
				close_monst = look_monst;
				closest = sd;
			}
		}
                if (big_monst || close_monst) {
                  	this->see_monst = (big_monst && close_monst ?
                  		(biggest_dist < hung_per * 1.2 ? big_monst : close_monst ) :
                  		big_monst ? big_monst : close_monst
          		);
                }
        }
        if (!this->see_monst) this->see_monst = who_hit_me_last;
        if ((drugged < 50) && armed()) {
        	if (armed()->t->typ == inv_injector || armed()->t->typ == inv_food) use_item(); // injects self if it does not have enough drugs in its system
        	if (drugs) toggle_weap(1);
		}
	if (this->see_monst) {
                int ccd = (
                	see_monst->check_mb_list(cheech_collect_dead) &&
                	( see_monst->dead() || (see_monst->base != &mb_toothworm && drugs < 2) )
                );
                int cca = see_monst->check_mb_list(cheech_collect) && drugs < 5;
		int hate = (drugged < 100. && hates(see_monst));
   		if (hate || would_eat(see_monst)) {
			if (hate) feel(emot_pissed);
                        chase(see_monst);
		} else if (scared_of(see_monst)) {
    			evade(see_monst);
		} else if (ccd || cca) {
    			// collect drugs
    			chase(see_monst);
                        take_weap(see_monst, cheech_bt);
    		}
	} else if (drugged == 0. && drugs == 0) { feel(emot_surprized); wander(0.8); } else walk(0.,0.,0.);
}


static builder_table hoveler_bt[] = {
	{&mb_yellowrock, &mb_pacifyerdarts, 8000},
	{&mb_purplerock, &mb_outcolddarts, 10000},
	{&mb_greencrystals, &mb_trankquildarts, 5000},
	{&mb_stalkplant, &mb_ezsleepdarts,  5000},
};

static monst_base* hoveler_collect[] = {
        &mb_unlabeledcan, &mb_mixedvegetables, &mb_eyestalkshoots, &mb_cannedfurbit,
        &mb_steamedmubark,

	&mb_gorgondarts, &mb_comadarts, &mb_heartattackdarts,
	&mb_purplerock, &mb_outcolddarts,
	&mb_yellowrock, &mb_pacifyerdarts,
	&mb_greencrystals, &mb_trankquildarts,
	&mb_stalkplant, &mb_ezsleepdarts,

         NULL
};

static monst_base* hoveler_collect_nm[] = {
	&mb_gorgondarts, &mb_comadarts, &mb_heartattackdarts,
	&mb_outcolddarts, &mb_purplerock,
	&mb_pacifyerdarts, &mb_yellowrock,
	&mb_trankquildarts, &mb_greencrystals,
	&mb_ezsleepdarts, &mb_stalkplant,
         NULL
};



void monst_t::hoveler_ai() {
	// based partially on the beholder Ai and cheech Ai
	// hovelers try to collect [20 - 30] of the best darts they can make.
	// they trank food before killing it they also try to trank enemys.
	if (!my_ast || (fear_dist <= 0)) return;
        if (_fire_run_ai() || _krulasite_inf_ai()) return;
	float biggest = 0.0, closest = my_ast->pw, iclosest =  0., biggest_dist = 0.;
	monst_t *big_monst = NULL, *close_monst = NULL, *best_item = NULL;
	float hung_per = (stomach == 0 ? 1.0 : in_stomach / stomach);
	hunger_level =  ( hung_per > 0.5 ? 0 : hung_per > 0.20 ? 1 : 2);
        feel( hunger_level ? emot_hungry : tired() ? emot_tired : emot_none);
	weap_t *w = armed();
	int darts = 0; int count = 0, lolvl = -1, loidx = 0;
        for(list<weap_t *>::iterator wp = weap.begin(); wp != weap.end(); wp++) {
        	weap_t *w = *wp; if (w->t->typ == inv_proj) {
           		darts += w->count;
           		if (w->t->lvl < lolvl && w->count < 15) { lolvl = w->t->lvl;  loidx = count; }
            	}
            	count++;
       	}
       	if (darts > 30 && lolvl >= 0) {
          	for (int c = 0; c < loidx; c++) toggle_weap(1);
          	drop_weap();
  	}
	monst_base *best_base = (w ? w->t->mb_link : NULL);
	for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
		monst_t *look_monst = *mon;
		if (look_monst == this) continue;
		float fd = dist(look_monst->x, look_monst->y,
					(this->x  + cos(this->ang)  * this->radius  * 1.1) ,
					(this->y  + sin(this->ang)  * this->radius  * 1.1)
				    ) - (look_monst->radius);
		if (look_monst->alive()  &&  (fd  < this->fear_dist) && (scared_of(look_monst) ||
                	(look_monst == who_hit_me_last && !would_eat(look_monst)))
		) {
                        float fr = fd / fear_dist;
                        float ff = look_monst->size * (look_monst == who_hit_me_last ? 6. : 1.) *
                              (fr > 0.5 ? 1. : fr > 0.2 ? 5.0 : 8. );
			if (ff > biggest) {
				big_monst = look_monst;
				biggest_dist = fr;
				biggest = ff;
			}
  		}
  		int cca = (
  			hunger_level > 1 ? 0 :
          		look_monst->check_mb_list_better(
          			(darts < 20 ? hoveler_collect : hoveler_collect_nm), best_base
  			) ||
          		(best_base == look_monst->base && (darts < 30) && fd < iclosest)
  		);
  		if (cca) {
                       	best_item = look_monst;
                       	iclosest = fd;
                       	best_base = best_item->base;
              	}

		if ( would_eat(look_monst) ) {
                        float sd = z_dist(look_monst->x - this->x, look_monst->y - this->y);
			if (sd < closest) {
				close_monst = look_monst;
				closest = sd;
			}
		}
        }
        if (big_monst || close_monst) {
                this->see_monst = (big_monst && close_monst ?
                	(biggest_dist < hung_per * 1.2 ? big_monst : close_monst ) :
                	big_monst ? big_monst : close_monst
          	);
        }
        if (!this->see_monst) this->see_monst = who_hit_me_last;
        if (!this->see_monst) this->see_monst = best_item;
	if (this->see_monst) {
        	float dy = this->see_monst->y - this->y;
		float dx = this->see_monst->x - this->x;
		float d = z_dist(dy,dx);
		float tang = afix(atan2(dy, dx) - ang);
		int hate = hates(see_monst);
		int ko = (see_monst->eff_energy() < 20) || see_monst->isa_plant();
		weap_t *w = armed();
   		if (hate && w && !ko) {
       			feel(emot_pissed);
       			float v = (d < 100. ? -(100. - d) / 100. : d > w->t->rng * 0.9 ? 1. : 0.);
			walk(tang, v, 0.);
			if (fabs(tang) < M_PI / 2. && w && d < w->t->rng) fire_at(see_monst);
		} else if (hate || would_eat(see_monst)) {
    			chase(see_monst);
    			if (!ko && !hate && d > 30. && fabs(tang) < M_PI / 2. && w && d < w->t->rng ) fire_at(see_monst);
		} else if (scared_of(see_monst) && w && !ko) {
			if (d > fear_dist * 2. && w) {
     				walk(tang, fabs(tang/M_PI), 0.);
     			} else {
          			evade(see_monst);
				feel(emot_surprized);
                     	}
			if (fabs(tang) < M_PI / 2. && w && d < w->t->rng) fire_at(see_monst);
        	} else if (scared_of(see_monst)) {
                	evade(see_monst);
			feel(emot_surprized);
        	} else if (see_monst == best_item) {
    			chase(see_monst);
                        take_weap(see_monst, hoveler_bt);
           	}
	} else {
		int inside_time = ((ms_count / 300000) < 60000);
		if (!_build_home_ai(inside_time)) wander();
	}
}



void monst_t::animal_ai() {
	if (!my_ast || (fear_dist <= 0)) return;
    if (_fire_run_ai() || _krulasite_inf_ai()) return;
	float hung_per = (stomach == 0 ? 1.0 : in_stomach / stomach);
	hunger_level =  ( hung_per > 0.5 ? 0 : hung_per > 0.20 ? 1 : 2);
	feel( hunger_level ? emot_hungry : tired() ? emot_tired : emot_none);
    fear_hunger_pick();
    if (!this->see_monst) this->see_monst = who_hit_me_last;
	if (this->see_monst) {
		int hate = hates(see_monst);
   		if (hate || would_eat(see_monst)) {
       			// chace see_monst
			if (hate) feel(emot_pissed);
                        chase(see_monst);
		} else if (scared_of(see_monst)) {
    			evade(see_monst);
		}
	} else if (base == &mb_mimmack) {
               float rezero = shortest_arc(ang, 1.5 * M_PI);
               if (fabs(rezero) > 0.0001) {
			walk(rezero, 1.0, 0.);
               }
	} else if (flags & AF_WANDERS) wander(); else walk(0.,0.,0.);
}

void monst_t::slymer_ai() {
        // very hungry. favors eating the dead. likes dragging arround items with it.
	if (!my_ast || (fear_dist <= 0)) return;
	if (_krulasite_inf_ai()) return;
	float hung_per = (stomach == 0 ? 1.0 : in_stomach / stomach);
	hunger_level = ( hung_per > 0.90 ? 0 : hung_per > 0.70 ? 1 : 2); // higher than normal
	feel( hunger_level ? emot_hungry : tired() ? emot_tired : emot_none);
	float biggest = 0.0, closest = my_ast->pw, closest_slime = my_ast->pw, biggest_dist = 0.;
	monst_t *big_monst = NULL, *close_monst = NULL,  *take = NULL, *close_slime = NULL;
        int drop = ((aiu1 + ms_count % 80000) < 10000);
        int u_lead = 0;
	for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
		monst_t *look_monst = *mon;
		if (look_monst == this) { u_lead = 1; continue; }
		float fd = dist(look_monst->x, look_monst->y, x,y) - look_monst->radius - radius;
		if (fd < 0 && !drop && look_monst->ai_func == ai_item) take = look_monst;
		if (look_monst->base == this->base && u_lead) {
                	if (fd < radius * 1.1 + look_monst->radius) if (fd < closest_slime) {
                        	close_slime = look_monst;
                                closest_slime = fd;
                   	}
    		}
		if (look_monst->alive()  &&  (fd  < this->fear_dist) && (scared_of(look_monst) ||
                	(look_monst == who_hit_me_last && !would_eat(look_monst)))
		) {
                        float fr = fd / fear_dist;
                        float ff = look_monst->size * (look_monst == who_hit_me_last ? 6. : 1.) *
                              (fr > 0.5 ? 1. : fr > 0.2 ? 5.0 : 8. );
			if (ff > biggest) {
				big_monst = look_monst;
				biggest_dist = fr;
				biggest = ff;
			}
  		}
		if ( would_eat(look_monst) || hates(look_monst)) {
                        float sd = z_dist(look_monst->x - this->x, look_monst->y - this->y) / (look_monst->dead() ? 10. : 1);
			if (sd < closest) {
				close_monst = look_monst;
				closest = sd;
			}
		}
        }
        if (big_monst || close_monst) {
        	this->see_monst = (big_monst && close_monst ?
                  		(biggest_dist < hung_per * 1.2 ? big_monst : close_monst ) :
                  		big_monst ? big_monst : close_monst
      		);
        }
        if (drop) drop_weap();
        if (take) {
        	take_weap(take);
                weap_t *w = armed();
                if (w) {
                  if (w->t->typ == inv_food || w->t->typ == inv_injector) use_item();
                }
        }
        if (!this->see_monst) this->see_monst = who_hit_me_last;
        if (close_slime && ((ms_count + aiu1) % 31000 < 8000)) {
          	float dy = close_slime->y - y;   float dx = close_slime->x - x;
		float tang = afix(atan2(dy, dx) - ang);
		walk(0., -cos(tang), sin(tang)); // walk away
	} else if (this->see_monst) {
		int hate = hates(see_monst);
   		if (hate || would_eat(see_monst)) {
       			// chace see_monst
			if (hate) feel(emot_pissed);
			float dy = see_monst->y - y;   float dx = see_monst->x - x;
			float d = z_dist(dy,dx);
			float tang = afix(atan2(dy, dx) - ang);
			if (d < 200.) walk(0., cos(tang), -sin(tang));
			else chase(see_monst);
		} else if (scared_of(see_monst)) {
    			evade(see_monst);
		}
	} else if (flags & AF_WANDERS) wander(); else walk(0.,0.,0.);
}



