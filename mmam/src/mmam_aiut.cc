#include "mmam.h"
int drunk_test = 0;

void monst_t::walk(float tang, float exp, float lat_exp) {
    if (eff_energy() <= 0) { do_act(ACT_CHILL); return; }
	if (!my_ast) return;
	float drunkness = (drunk_test ? 1. : od_thresh == 0 || max_energy == 0 ? 0. :
		MID(0., drugged / od_thresh * (1.25 - eff_energy() / max_energy), 1.)
	);
	if (drunkness) {
		float msc = (ms_count % 62800);
		exp = MID(-1.,exp + drunkness * (sin(msc / 720.) - sin(msc / 304.))/3. ,1.);
		lat_exp = MID(-1., lat_exp + drunkness * (cos(msc / 877.) - cos(msc / 314.))/2.,1.);
  	}
	tang = afix(tang);
	float etr = eff_turn_rate();
	if (fabs(tang) > etr) {
		if (tang < 0) this->ang -= etr;
		else  this->ang += etr;
	} else this->ang += tang;
	wang = ang;
	float astr = 0, hstr = 0;
	// if (energy > 0)  energy -= (int)floor(exp * 8.0 + 0.5);

    astr = (eff_speed() ) * exp * (eff_energy() > 100 ? 1. : eff_energy() / 100.);
	hstr = (eff_lat_speed() ) * lat_exp * (eff_energy() > 100 ? 1. : eff_energy() / 100.);

	ax = cos(ang) * astr + sin(ang) * hstr - this->vx * (flags & AF_FLYING ? 0.05 : 0.1);
	ay = sin(ang) * astr - cos(ang) * hstr - this->vy * (flags & AF_FLYING ? 0.05 : 0.1);
        if ( (this == hud->hooked_monst) || (hud->hooked_monst && this == ship) ) {
        	return; // movement must be calced by the fishing code.
        }
	vx += ax;
	vy += ay;

        if ((this->x <= 0 && vx < 0) || (this->x >= my_ast->pw && vx > 0)) this->vx = 0.0;
    	if ((this->y <= 0 && vy < 0) || (this->y >= my_ast->pw && vy > 0)) this->vy = 0.0;
        traveled += z_dist(this->vx, this->vy);

    	this->x += this->vx;
    	this->y += this->vy;
    	if (this->x < 0)                 this->x = 0.0;
    	else if (this->x > my_ast->pw)   this->x = my_ast->pw;
    	if (this->y < 0)                 this->y = 0.0;
    	else if (this->y > my_ast->pw)   this->y = my_ast->pw;
    	monst_t *bump = (base == &mb_slymer || base == &mb_quiver ?
    		my_ast->monst_at(x, y, this) :
    		my_ast->monst_at(x+cos(ang)*(radius + 1), y + sin(ang)*(radius + 1), this)
	);
        if (bump) {
          	if (flammable() & bump->flammable()) {
                        if (on_fire && bump->on_fire) {
        			if (on_fire > bump->on_fire) bump->on_fire = on_fire; else on_fire = bump->on_fire;
                     	} else if (on_fire && on_fire - ms_count > 1000) {
                        	bump->on_fire = ms_count + ON_FIRE_TIME;
                        }  else if (bump->on_fire && bump->on_fire - ms_count > 1000) {
                        	on_fire = ms_count + ON_FIRE_TIME;
	              	}
                }
        	if (hates(bump)) attack(bump);
                else if (  hunger_level == 3 || (bump->dead() && hunger_level >= 2)  ) eat(bump); // starving mode will eat anything!
        	else if ( (hunger_level == 2 || bump->dead() ) && will_eat(bump) ) eat(bump);
        	else if ( (hunger_level >= 1 || bump->dead() ) && likes_to_eat(bump) ) eat(bump);
  	} else {
		if (tang != 0.|| exp != 0 || lat_exp != 0) { 
			do_act(ACT_WALK); climb_out_hole(); 
                       	int d = irand(TURNS_PER_SEC * 40 );
			switch (d) {
                        case 1: if (spore_poison_count) { my_ast->drop_spore(this, &IDF_POISONSPORE); spore_poison_count--; } break;
                        case 2: if (spore_psycho_count) { my_ast->drop_spore(this, &IDF_PSYCHOSPORE); spore_psycho_count--; } break;
                        case 3: if (spore_pyro_count) { my_ast->drop_spore(this, &IDF_PYROSPORE);  spore_pyro_count--; } break;
                   	}
		}
        	else if (action_idx == ACT_WALK) do_act(ACT_CHILL);
     	}
}

int monst_t::check_mb_list(monst_base **base_list) {
	if (!base_list) return 0;
	for (monst_base **mb = base_list; *mb; mb++) if (this->base == *mb) return 1;
	return 0;
}

int monst_t::check_mb_list_better(monst_base **base_list, monst_base *cmp) {
	// returns 1 if this has higher (earlier) list placement than cmp (or this is in the list and cmp == NULL).
	// returns 0 on ties, lower placement, neither in the list 
	if (!cmp) return check_mb_list(base_list);
	if (!base_list) return 0;
	for (monst_base **mb = base_list; *mb && *mb != cmp; mb++) if (this->base == *mb) return 1;
	return 0;
}

int monst_t::check_mb_list_worse(monst_base **base_list, monst_base *worse) {
	if (!base_list || !worse) return 0;
	int ret = 0;
	for (monst_base **mb = base_list; *mb; mb++) {
   		if (this->base == *mb) return ret;
   		if (*mb == worse) ret = 1;
	}
	return 0;
}


static monst_base* asteroid_zero_guys[] = {
	&mb_astbase,	      &mb_netship, 	    &mb_radiomarker, &mb_organicsscanner,
	&mb_deltawavescanner, &mb_qparticlescanner, &mb_epsilonhiscanner,
	&mb_guntower,	      &mb_dgtower,	    &mb_xcannontower,	&mb_galtixtower,
	&mb_solararray,	      &mb_fissionplant,     &mb_fusionplant, &mb_worker, NULL
};

int monst_t::hates(monst_t *who) {
        if (who == NULL) return 0;
        if (who->dead()) return 0;
        if ((flags & AF_SEECLOAKING) == 0 && who->invisible()) return 0;
        if (who->ai_func == ai_item) return 0;
        if (my_ast) if (my_ast->has_arena()) return 1;
        if (flags & AF_BESERK && who->base != &mb_psychoshroom ) return 1;
        if (flags & AF_PARANOID && ((aiu1 + who->aiu1) % 50) == 0 && who->size * 0.5 * base->fear_factor <= size ) return 1; // 2%
        if (who == who_hit_me_last && who->size * base->fear_factor <= size && !scared_of(who)) return 1;
        if (agression < 100 && org_idx == org_none && who->org_idx == org_ast0) return 0;
        if (extra_hates) if (who->check_mb_list(extra_hates)) return 1;
        if (who->org_idx == org_ast0 && org_list[org_idx].al <= -2) return 1;
        if (base == &mb_hunter && who->base->stat_valid_target & TARGET_HUNTER) return 1;
        if (org_idx == org_ast0) {
	    	if (who->base->stat_valid_target & TARGET_AST0) return 1;
	       	if (org_list[who->org_idx].al <= -2 && !org_list[who->org_idx].peaceful) return 1; // automatic security
        } 
        return (who->check_mb_list(this->base->hates));
}


void monst_t::fear_hunger_pick() {
	// returns the monst that should have your attention in see_monst.
	// if you get out of range of see_monst and nothing new gets your attention, it stays in memory
	// also keeps track of the closest legal hole
	float biggest = 0.0, closest = my_ast->pw, biggest_dist = 0., closest_hole = my_ast->pw, closest_buddy = my_ast->pw;
	float hung_per = (stomach == 0 ? 1.0 : in_stomach / stomach);
	monst_t *big_monst = NULL, *close_monst = NULL;
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
		if (can_hide_in(look_monst) && (fd < closest_hole)) {
			see_hole = look_monst;
			closest_hole = fd;
		}
		if (look_monst->base == base && (fd < closest_buddy)) {
			see_buddy = look_monst;
			closest_buddy = fd;
		}
		if ( would_eat(look_monst) || hates(look_monst)) {
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
}

void monst_t::fear_hunger_pick2() {
	// this version dos not care about holes.	
	float biggest = 0.0, closest = my_ast->pw, biggest_dist = 0.;
	float hung_per = (stomach == 0 ? 1.0 : in_stomach / stomach);
	monst_t *big_monst = NULL, *close_monst = NULL;
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
		if ( would_eat(look_monst)  || (!hunger_level && irand(10) == 0)) {
			float sd = z_dist(look_monst->x - this->x, look_monst->y - this->y);
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
}


#if 1
// old chase code.
/*
void monst_t::chase(monst_t *who) {
	float tang = atan2(who->y - this->y, who->x - this->x) - this->ang;
	if (tang > M_PI)  tang -= 2.0 * M_PI;
	if (tang < -M_PI) tang = 2.0 * M_PI + tang;
	if (fabs(who->vx) < 0.00001 && fabs(who->vy) < 0.00001 ) {
		walk(tang, 0.18, 0.);
	} else {
		if (fabs(tang) < turn_rate) walk(tang, 1.0, 0.); else walk(tang,0.84,0.);
      	}
}
*/
int monst_t::chase(monst_t *who, float speed, float back_speed, float r1, float r2) {
	if (!who) return 0;	
	float dx = who->x - x;
	float dy = who->y - y;
	float tang = afix(atan2(dy, dx) - ang);
	if (fabs(vy) > 0.0001 && fabs(vx) > 0.0001) tang = afix(atan2(dy, dx) - atan2(vy, vx));
	float D = z_dist(dy, dx);
	if (r2 == 0.) { r1 = -radius/4.; } // penatrate up to 1/4 of our radius. 
	float contactD  = D - radius - who->radius - 0.001 - r1;
	float contactD2 = D - radius - who->radius - 0.001 - r2;
	if (contactD < 0.) {
		walk(0., -back_speed, 0.); // oops!  my bad.  you au'ight?  
		return 1;
	} else if (contactD2 < 0.) {
		walk(tang, 0., 0.);
		return 1;
	} else {
		if (D > 400. || (chase_last != who && (!chase_last || z_dist(chase_last->x - who->x, chase_last->x - who->x) > 150.))) {
				aiu2 = ms_count + 7000;
				chase_last = who;
		}
		int sd = aiu2 - ms_count;
		float ch_mult = speed * (sd <= 0 ? 1.0 : MAX(0.15, 1.0 - sd / 10000.));        	
		if (fabs(tang) < turn_rate || tang == 0. ) {
			walk(tang,  ch_mult , 0.);
		} else {
			// law of signs to find rad of circle that intersects target
			float B = M_PI / 2. - fabs(tang);
			float C = M_PI - 2. * B;
			float sinB = sin(B);
			float sinC = sin(C);
			float R = (sinC == 0. ? D / 2. : sinB * D / sinC);
			float maxV = 2. * M_PI * R * eff_turn_rate();
			float V = z_dist(vx,vy);
			float maxA = eff_speed();
			float DV = (D < 150. ? D / 150.  : 1.0) * (maxV - V);
			float rspd = (maxA < DV || maxA == 0 ? 1. : MID(0., DV / maxA, 1.)  );
			walk(tang, rspd * ch_mult, 0.);
		}
	}
	return 0;
}


#else
void monst_t::chase(monst_t *who) {
        float dx = who->x - x;
        float dy = who->y - y;
	float tang = afix(atan2(dy, dx) - ang);
        if (fabs(vy) > 0.0001 && fabs(vx) > 0.0001) tang = afix(atan2(dy, dx) - atan2(vy, vx));
	float D = z_dist(dy, dx);
	{
        	if (D > 400. || (chase_last != who && (!chase_last || z_dist(chase_last->x - who->x, chase_last->x - who->x) > 150.))) {
                	aiu2 = ms_count + 7000;
                	chase_last = who;
           	}
                int sd = aiu2 - ms_count;
                float ch_mult = (sd <= 0 ? 1.0 : MAX(0.15, 1.0 - sd / 10000.));        	
		if (fabs(tang) < turn_rate || tang == 0. ) {
    			walk(tang,  ch_mult , 0.);
		} else {
    			// law of signs to find rad of circle that intersects target
			float B = M_PI / 2. - fabs(tang);
			float C = M_PI - 2. * B;
			float sinB = sin(B);
			float sinC = sin(C);
			float R = (sinC == 0. ? D / 2. : sinB * D / sinC);
			float maxV = 2. * M_PI * R * eff_turn_rate();
			float V = z_dist(vx,vy);
			float maxA = eff_speed();
			float DV = (D < 150. ? D / 150.  : 1.0) * (maxV - V);
			float rspd = (maxA < DV || maxA == 0 ? 1. : MID(0., DV / maxA, 1.)  );
			walk(tang, rspd * ch_mult, 0.);
        	}
      	}
}
#endif

void monst_t::evade(monst_t *who) {
    if (hole_monst) return;
	if (z_dist(who->x - this->x, who->y - this->y) < fear_dist * 2.5) {
		feel(emot_surprized);
		int no_good = 50;
		float escape_arc = M_PI / 8.0;
		float ea = atan2(who->y - this->y, who->x - this->x);
		float ta = atan2(this->run_y - this->y, this->run_x - this->x);
                if (flags & AF_DIGS && see_hole) {
			float ha = atan2(see_hole->run_y - y, see_hole->x - x);
                  	if ( fabs(shortest_arc(ea, ha)) > escape_arc && hide_in_hole(see_hole) ) return;
                }
		while ( ( z_dist(who->x - this->run_x,
			who->y - this->run_y) < fear_dist * 2.5 ||
			fabs(shortest_arc(ea, ta)) < escape_arc ) && no_good ||
			this->run_x < 75. || this->run_y < 75. ||
			this->run_x > my_ast->pw - 75. || this->run_y > my_ast->ph - 75.
		) {
			this->run_x = irand(my_ast->pw - 150) + 75.;
			this->run_y = irand(my_ast->ph - 150) + 75.;
			ta = atan2(this->run_y - this->y,  this->run_x - this->x);
			no_good--;
		}
		if (no_good == 0) printf((char *)"No escape route found.\n");
		float tang = ta - ang;
        walk(tang, 1.0, 0.);
	} else walk(0.,0.,0.);
}

void monst_t::wander(float speed, float clamp_tr) {
    // ejk: added mods to avoid the jumpgate while wondering
    float djg = dist(my_ast->jumpgate_x, my_ast->jumpgate_y, x, y);
    float djg2 = dist(my_ast->jumpgate_x, my_ast->jumpgate_y, run_x, run_y);
    float dr = dist(x,y, run_x, run_y);    
    float ja = atan2(my_ast->jumpgate_y - this->y, my_ast->jumpgate_x - this->x);
	float ra = atan2(this->run_y - this->y, this->run_x - this->x);
	// the timer only does simi effective anti ping pong and will only work
	// at keeping the critter out of the wormhole sometimes (there is a better
	// way to keep them from getting stuck while selecting an ok route, but I'm 
	// feeling rather anti trig right now)   
    if (dr < 10 ||
		this->run_x < 75. || this->run_y < 75. ||
		this->run_x > my_ast->pw - 75. || this->run_y > my_ast->ph - 75. ||
		ms_count > aiu2 || djg2 < 140. ||
		(djg < 250. && (fabs(shortest_arc(ja,ra)) < M_PI/2.) && aiu2 - ms_count < 8200) 
	) {
		do { // could only infinate loop for an ast with pw <= 430 && ph <= 430 (those don't exist)
		   this->run_x = irand(my_ast->pw - 150) + 75.0;
		   this->run_y = irand(my_ast->ph - 150) + 75.0;
		} while (dist(my_ast->jumpgate_x, my_ast->jumpgate_y, run_x, run_y) < 140.);
		aiu2 = ms_count + (flags & AF_PARANOID ? 1900 : 9000);
        printf((char *)"%s wander to %.0f,%.0f\n",name, run_x, run_y);
	}
	float ta = atan2(run_y - y, run_x - x);
	if (flags & AF_PARANOID) { speed = MAX(speed + 0.4, 1.0); }
	float tang = afix(ta - ang);
    float max_step = M_PI * clamp_tr / 180. / TURNS_PER_SEC;
	if (fabs(tang) > max_step) {
        	tang = (tang < 0 ? -max_step : max_step);
   	}
	walk(tang, speed, 0.);
}

int monst_t::_fire_run_ai() {
	if (on_fire < ms_count) return 0;
    if (dist(this->run_x, this->run_y, this->x, this->y) < 10 ||
		this->run_x < 75. || this->run_y < 75. ||
		this->run_x > my_ast->pw - 75. || this->run_y > my_ast->ph - 75. ||
		ms_count > aiu2 || aiu2 - ms_count > 1000
	) {
		this->run_x = irand(ast->pw - 150) + 75.0;
		this->run_y = irand(ast->ph - 150) + 75.0;
		aiu2 = ms_count + 800;
	}
	float ta = atan2(run_y - y, run_x - x);
	float tang = afix(ta - ang);
	feel(emot_surprized);
	walk(tang, 1., 0.);
    return 1;
}

char *no_sex_reason = NULL;


int monst_t::buddy_wants_to_get_laid() {
	// to return 1: they both have to pick eachother (except frogerts)
	// be of opposite sex, both horny, both have a gestation count, be alive and the same species.
	// if either believes in familys: 
	//    either both unmarrieed or agree that they are married to eachother. 
	no_sex_reason = NULL;
	if (see_buddy) {
		int has_family_flag = ((flags | see_buddy->flags) & AF_FAMILY);
		if (see_buddy->horny_time > ms_count)  no_sex_reason = (char *)"You shot me down.";
		if (horny_time > ms_count) {
			no_sex_reason = (char *)"I'm not in the mood.";
		}
		if (see_buddy->see_buddy != this && base != &mb_frogert) (char *)"I'm not into orgies."; // frogerts have orgys.
	
		if ( !see_buddy->gestation ) no_sex_reason = (char *)"You are celebate.";
		if (has_family_flag && ((see_buddy->spouse && see_buddy->spouse != this )) ) no_sex_reason = (char *)"You're married.";
	
		if (has_family_flag && ((spouse && spouse != see_buddy )) ) no_sex_reason = (char *)"I'm married.";
		
		if (!see_buddy->alive()) no_sex_reason = (char *)"I'm not into necrophilia.";
		
		if (sex == 2 && see_buddy->sex == 2) no_sex_reason = (char *)"I'm not a lesbian.";
		if (sex == 1 && see_buddy->sex == 1) no_sex_reason = (char *)"I'm not gay.";	
	
		if (see_buddy->base != base) no_sex_reason = (char *)"I'm not into beastiality.";
	} else no_sex_reason = (char *)"I'm alone.";
	if (horny_time > ms_count) {
		if (time_done_pregnant > ms_count) no_sex_reason = (char *)"I'm pregnant.";
		if (time_mature > ms_count) no_sex_reason = (char *)"I'm too young.";
	}		
	if ( !gestation  ) no_sex_reason = (char *)"I'm celibate.";
	if (!alive()) no_sex_reason = (char *)"I'm dead.";
	if (no_sex_reason) return 0;	
	no_sex_reason = (char *)"Let's do it.";
	return 1;
}

int monst_t::_build_home_ai(int prefers_inside) {
	if (sex == 2 && flags & AF_FAMILY) {
		// a female always live with her husband if he's alive, but will move back 
		// with her parents if their home is destroyed. 
		if (spouse) home_hole = spouse->home_hole;
		if (!home_hole && daddy) home_hole = daddy->home_hole;
		if (!home_hole && momma) home_hole = momma->home_hole;	
	} else if (ms_count >= time_mature - 20000 ) {
		// critters move out shortly before maturity
		if (daddy && home_hole == daddy->home_hole) home_hole = NULL;
		if (momma && home_hole == momma->home_hole) home_hole = NULL;
	} else {
		if (!home_hole && daddy) home_hole = daddy->home_hole;
		if (!home_hole && momma) home_hole = momma->home_hole;	
	}
	
	if (!home_hole) {
		if (!(sex == 2 && flags & AF_FAMILY)) {
			reset_horny_time();
		}	
		if (prefers_inside) {
			home_hole = my_ast->spawn_monst_by(&mb_hole, x, y);
			if (home_hole) {
				home_hole->radius = 0.25;
				home_hole->hp = 1;
			}
			return 1;			
		}
	} else if (prefers_inside) {
			float danger_lvl = home_hole->mhp ? home_hole->hp / home_hole->mhp : 1.;
			float rs = (
				danger_lvl > 0.9 ? 0.4 :
				danger_lvl > 0.6 ? 0.5 :
				danger_lvl > 0.15 ? 0.6 : 1.0
			);  
			if (chase(home_hole, rs) && attack_time < ms_count)  {
				float req_radius = radius * 1.5;
				if (home_hole->hp >= home_hole->mhp && home_hole->radius >= req_radius ) {
					hide_in_hole(home_hole);
				} else {				
					do_act(ACT_BUILD);
					attack_time = ms_count + 1000;
					home_hole->hp = MIN( home_hole->hp + 5, home_hole->mhp);
					if (home_hole->radius < req_radius) home_hole->radius += 0.5;
				}
			}
			return 1;    			    
	} else climb_out_hole();
	return 0;
}

int monst_t::_slap_and_tickle_ai() {
	// handles critters having sex.
	if (buddy_wants_to_get_laid()) {
		if (see_buddy->sex == sex && sex > 0) {
			int *the_universe = NULL;
			*the_universe = 0; // gay sex causes a segfault (bad monsters!)
		}
	    if ( sex <= 1 ) { // male and hermaphrodite
			if (chase(see_buddy, 0.5, radius / 2, radius / 8.)) {
				if ((game_turn % 10) == (aiu1 % 10) ) {
					see_buddy->spouse = this;
					spouse = see_buddy;					
					if (irand(30) != 0) {
						see_buddy->sqwak(snd_sex);								  
						walk(0.,-0.8, 0.); // nailing it.
					} else {
						see_buddy->time_done_pregnant = ms_count + see_buddy->gestation;
						if (sex == 0) {
							// weird hermaphrodite stuff
							time_done_pregnant = ms_count + gestation;
						} else if (base == &mb_worker) {
							hud->long_news((char *)"Your %s, %s, is now pregnant.", 
								see_buddy->worker_class(2), see_buddy->name
							);
						} else if (!(see_buddy->flags & AF_EGGS)) {
							hud->long_news((char *)"Congratulations! You have a %s that is now pregnant.", 
								see_buddy->name
							);						
						}
						reset_horny_time();				
						see_buddy->reset_horny_time();
						// this is wrong. i really shouldn't sprinkle gooey hot white pixels ;) 
						ast->spatter_blood(this, ang, irand(10), 0, 1);	
						ast->spatter_blood(this, ang+0.0001, irand(9), 0, 1);	
						ast->spatter_blood(this, ang+0.002, irand(8), 0, 1);	
						ast->spatter_blood(this, ang-0.0001, irand(7), 0, 1);	
						ast->spatter_blood(this, ang, irand(6), 0, 1);	
					}
				}
			}
		} else { // female code
			if (dist(x,y,see_buddy->x, see_buddy->y) > 4. * radius) {
				chase(see_buddy, 0.4);
			} else walk(afix(ang - see_buddy->ang), 0, 0); // mount position
		}
		return 1;
	}
	return 0;
}


int monst_t::scared_of(monst_t *who) {
	if (who == NULL) return 0;
	if (who->dead() || (flags & AF_BESERK)) return 0;
	if ((flags & AF_SEECLOAKING) == 0 && who->invisible()) return 0;
	if (my_ast) if (my_ast->has_arena()) return 1;
	if (flags & AF_PARANOID && ((aiu1 + who->aiu1 + ms_count) % 500000) < 10000 ) return 1; // 5% chance for 10 sec
	if (who == who_hit_me_last && who->size * base->fear_factor > size) return 1;
	if (agression < 50 && org_idx == org_none && who->org_idx == org_ast0) return 0;
	if (extra_scared_of) if (who->check_mb_list(extra_scared_of)) return 1;
	return (who->check_mb_list(this->base->scared_of));
}

int monst_t::will_eat(monst_t *who) {
    if (who == NULL) return 0;
    if (who->nutrish <= 0) return 0;
    if ((flags & AF_BLOODSUCKER) && who->dead()) return 0;
	if (my_ast) if (my_ast->has_arena()) return 0;
    return (who->check_mb_list(this->base->will_eat));
}

int monst_t::likes_to_eat(monst_t *who) {
	if (who == NULL) return 0;
	if (who->nutrish <= 0) return 0;
	if ((flags & AF_BLOODSUCKER) && who->hp <= 0) return 0;
	if (my_ast) if (my_ast->has_arena()) return 0;
	return (who->check_mb_list(this->base->likes_to_eat));
}

int monst_t::would_eat(monst_t *who) {
	if (who == NULL || !stomach) return 0;
	if ((flags & AF_BLOODSUCKER) && who->hp <= 0) return 0;
	switch (  hunger_level ) {
	case 2 : if ( will_eat(who) ) return 1; // no break
	case 1 : return likes_to_eat(who);
	default : return (hunger_level > 2 ? ( who->nutrish > 0 ? 1 : 0) : 0);
	}
}

int monst_t::will_eat(monst_base *what) {
    monst_t *m = new monst_t(what);
    int ret = will_eat(m);
    delete m;
    return ret;
}

int monst_t::likes_to_eat(monst_base *what) {
    monst_t *m = new monst_t(what);
    int ret = likes_to_eat(m);
    delete m;
    return ret;
}

int monst_t::would_eat(monst_base *what) {
    monst_t *m = new monst_t(what);
    int ret = would_eat(m);
    delete m;
    return ret;
}


float monst_t::chubby_dist(monst_t *who) {
  	float Dx = who->x - x;
  	float Dy = who->y - y;
  	return MAX(0., z_dist(Dx, Dy) - who->radius - radius);
}

float monst_t::mdist(monst_t *who) {
  	float Dx = who->x - x;
  	float Dy = who->y - y;
  	return z_dist(Dx, Dy);
}


int monst_t::shot_lead(monst_t *who, float &shang) {
  	weap_t *w = armed();
    if (!w) return 0;
    float Vw = w->t->vel / TURNS_PER_SEC;
    float Vx = who->vx - vx;
  	float Dx = who->x - x;
  	float Vy = who->vy - vy;
  	float Dy = who->y - y;
	float t = lpquadratic(
		Vx * Vx + Vy * Vy - 2. * Vw * Vw,
		2. * (Dx * Vx + Dy * Vy),
		Dx * Dx + Dy * Dy
	);
	if (t < 0.) return 0;
	shang = atan2(Dy + Vy * t, Dx + Vx * t);
    return 1;
}


int monst_t::can_hide_in(monst_t *hole) {
	return (hole_monst == NULL && hole && hole->base == &mb_hole && (!hole->hole_monst || hole->hole_monst->base == base ) && radius <= hole->radius);
}

int monst_t::hide_in_hole(monst_t *hole) {
	// hole functions are complete hacks.
	if (!can_hide_in(hole)) return 0;
	float d = dist(x,y,hole->x,hole->y);
	if (d < hole->radius) {
		// my_ast->unhook_monst(this);   done in ast poller
		for (list <monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); ++mon) {
			// drop preditor's desire to chase you
			if ((*mon)->see_monst == this) {
				(*mon)->see_monst = NULL;
			}
			if ((*mon)->who_hit_me_last == this) {
				(*mon)->who_hit_me_last = NULL;
			}
		} 
		hole->encl->add_monst(this);		
		hole->hole_monst = this;
		hole_monst = hole;
		// it still act's like it's on the asteroid but it's not in the my_ast->m list.
		my_ast = hole->my_ast; 
	} else chase(hole);
	return 1;
}

void monst_t::climb_out_hole() {
	if (!hole_monst) return;
	// my_ast->add_monst(this); done in ast poller
	hole_monst->hole_monst = NULL;
	hole_monst->encl->unhook_monst(this);
	my_ast = hole_monst->my_ast; x = hole_monst->x; y = hole_monst->y; vx = vy = 0.;
	list <monst_t *>::iterator ii = hole_monst->encl->m.begin();
	if (ii != hole_monst->encl->m.end()) { hole_monst->hole_monst = (*ii); } 
	hole_monst = NULL;
}




