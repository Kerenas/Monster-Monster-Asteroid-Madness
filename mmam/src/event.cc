#include "mmam.h"
#include "rand.h"
#include "graphs.h"

event_t event;

void event_t::calc_next() {
	next = 0;
	for (int c = 1; c < event_last; c++) {
   		if (event_list[c].enabled && event_list[c].time < event_list[next].time) next = c;
	}
}


void event_t::clear() {
	next = 0;
	if (autoshot_freq_sec) set(event_autoshot, autoshot_freq_sec);
	else unset(event_autoshot);
	set(event_hippies, 60 * (20 + irand(20)));
	unset(event_police);
	set(event_pit_fight, 180 + 60 * irand(5));
	unset(event_stalker_clan);
	set( event_merchant, 120);
	set( event_tourists, 1);
	set( event_raiders, 60 * (15 + irand(45)));
	set(event_hunters, 15);
	set(event_meteor_storm, 60 * (45 + irand(60)));
	set(event_weather_change, 60 * 10);
	set(event_golgithan, 60 * (60 + irand(60)));
	set(event_ufo, 60 * (60 + irand(60)));
}



void event_t::set(int e, int in_seconds, int severity) {
	event_list[e].time = ms_count + in_seconds * 1000;
	event_list[e].severity = severity;
	event_list[e].enabled = 1;
	calc_next();
}

void event_t::unset(int e) {
	event_list[e].enabled = 0;
	calc_next();
}


char debug_sweetness_points_str[1024];
int sweetness_points = 0;
int num_meteor_rocks_left = 0;
void do_auto_screen_shot();
void check_minigame();
	

void event_t::pop() {
	while (ms_count > event_list[next].time) {
		switch (next) {
        case event_autoshot:
            do_auto_screen_shot();
            set(event_autoshot, autoshot_freq_sec);
            break;
		case  event_pit_fight: {
			hud->short_news((char *)"Spacecraft Detected.");
			printf((char *)"event: pit fight\n");
				home_ast->spawn_monst(&mb_trainer, 1, 3 * 60000 + ms_count);
			}
			set(event_pit_fight,  600 + irand(600));
			break;
		case  event_merchant: {
				hud->short_news((char *)"Merchant Jumped In.");
				printf((char *)"event: visiting merchant\n");
				home_ast->spawn_monst(&mb_merchant, 1, 3 * 60000 + ms_count);
			}
			set( event_merchant, 300 + irand(500));
			break;
		case  event_tourists: {
				int gened_sweetness_points = 0;
				int num_tourists = mb_tourist.stat_inpark;
				int friend_points = 0;
				int crowd_penalty =	0;
				sprintf(debug_sweetness_points_str, "tournet is hiding"	);
				if (mb_hunter.stat_inpark == 0 &&
					mb_raider.stat_inpark == 0 && 
					mb_meteor.stat_inpark == 0 && 
					!hud->hunting_allowed
				) {
					// if fame == 1000 and ticket_price == 100
					// an uncrowded parks simple park generates 10sp/sec @ $100
					// its hard max is 9 tourists
					//  4000 fame $400 would generate 10 per sec. (assuming no penalty)
					//  4000 fame $100 would generate 40 per sec. (assuming no penalty)
					//  4000 fame $10 would generate 400 per sec. (assuming no penalty)
					static int crowd_penalty_table[41] = {
					//  0   1   2   3   4   5   6   7   8   9 
						0,  0,  0,  1,  1,  2,  3,  5,  9, 13,
					// 10  11  12  13  14  15  16  17  18  19 
						16, 20, 24, 29, 36, 40, 48, 57, 67, 78,
					// 20  21  22  23  24  25  26  27  28  29  
						90,102,115,130,146,163,182,202,224,246,
					// 30  31  32  33  34  35  36  37  38  39     
						275,305,337,368,388,410,430,450,470,500 
					};
					if (hud->fame /  MAX(hud->ticket_price, 1) >= 20.) friend_points += 5;
					if (hud->ticket_price < 30) friend_points += (
						30 - hud->ticket_price / 
						(hud->fame < 200 ? 4 : hud->fame < 500 ? 3 : hud->fame < 750 ? 2 : 1)
					);               
					int rep = (int)how_aligned(tournet);
					if (friend_points) {
						alignment.befriend(org_tournet, friend_points);                                
					} else if (rep <= -3) hud->long_news((char *)
						"Your park has a %s reputation with tourists.  "
						"You should definately go do some PR tatics.",
						(rep <= -10 ? "EXTREMELY BAD" :  rep  <= -6 ? "very bad" : "bad")
					);
					/*
					if (tval < 0.25) hud->long_news(
						"The guests think your ticket prices are outragous.  "
						"You should definately consider lowering them."
					);
					*/
					crowd_penalty = crowd_penalty_table[MID(0,num_tourists-rep,39)]; 
					gened_sweetness_points = MAX(hud->ticket_price <= 200 ? 1 : 0,  hud->fame/MAX(hud->ticket_price,10) - crowd_penalty );
					sweetness_points += gened_sweetness_points; 
					
					int tourist_cost = 60;
					sprintf(debug_sweetness_points_str, "sp:%i(%i new) tour:%i cp:%i rep:%i fp:%i", 
						sweetness_points, gened_sweetness_points, 
						num_tourists, crowd_penalty, rep, friend_points
					);
	
					if (sweetness_points >= tourist_cost) {
						// SHIP_NOISE(CHACHING); annoying.
					}	
					// feed_graphs(int money, int fame, int tourists, int ticket_price, int sweetness, int penalty)
					for (; sweetness_points >= tourist_cost; sweetness_points -= tourist_cost ) if (num_tourists < 40) {
						hud->money += hud->ticket_price;
						int t = 60 + irand(60); // avg: 90 sec                                		
						home_ast->spawn_monst(&mb_tourist, 1, t * 1000 + ms_count);
						num_tourists++;
					}
					
				}
				set( event_tourists, 1);
				graphs->feed_graphs(hud->money, hud->fame, num_tourists, hud->ticket_price, sweetness_points, gened_sweetness_points, crowd_penalty); 
			}		
			break;
		case  event_golgithan:  {
				int t = 300 + irand(8) * 60;
				SHIP_NOISE(INCOMMING);
				hud->short_news((char *)"DANGER: HUGE MONSTER DETECTED!");
				printf((char *)"event: golgathan for %i sec\n",  t);
				monst_t *m = home_ast->spawn_monst(&mb_golgithan, 1, t * 1000 + ms_count);
				m->in_stomach = m->stomach / 2;
				unset(event_golgithan); // there is only one per game  it sets when jumping out
				graphs->notestr_add('E',(char *)"Goligathan Appears");
				safty_pause();
			} 				
			break;
		case  event_ufo:  {
				int t = 200;
				SHIP_NOISE(INCOMMING);
				hud->short_news((char *)"WARNING: UNIDENTIFYED FLYING OBJECT DETECTED!");
				printf((char *)"event: ufo for %i sec\n",  t);
				monst_t *m = home_ast->spawn_monst(&mb_ufo, 1, t * 1000 + ms_count);
				set( event_ufo, 3600 + irand(3800));
				graphs->notestr_add('E',(char *)"Alien Spacecraft Sighted");
			}
			break;
		case event_hippies: {
				int ha = how_aligned(hippies);
				int n = 0;
				int t = 900 + irand(200);
				switch (ha) {
					case -2: n = 4; break;
					case -3: n = 7; break;
					case -4: n = 12; break;
					case -5: n = 18; break;
					case -6: n = 26; break;
				}
				if (n > 0) {
					SHIP_NOISE(INCOMMING);
						hud->short_news((char *)"PROTESTERS DETECTED!");
						printf((char *)"event: %i hippies for %i sec.\n", n, t);
						home_ast->spawn_monst(&mb_hippie,   n, t * 1000 + ms_count);
				}
				graphs->notestr_add('E',(char *)"Hippies Begin Protest");
				set(event_hippies, 60 * (20 + irand(20)));
			}
			break;

		case  event_raiders: {
			int n = 0;//irand(13) + 3;
			int s = 0;//MID(0, irand(-hud->karma / 250), 5);
			int t = 45 + irand(200);
			char *es = (char *)"Attacked by a Raider Scouting Party";
			switch (how_aligned(raiders)) {
			case -3: s = 0; n = 5 + irand(11); break;
			case -4: s = 1; n = 10 + irand(10); 
				es = (char *)"Attacked by a Raider Detachment"; break;
			case -5: s = 2 + irand(2); n = 15 + irand(9); 
				es = (char *)"Raiders Launch Full Scale Attack"; break;
			case -6: s = 3 + irand(5); n = 20 + irand(11); 
				es = (char *)"Raiders Launch MASSIVE Attack";break;
			}
			SHIP_NOISE(INCOMMING);
			hud->short_news((char *)"DANGER: RAIDERS DETECTED!");
			graphs->notestr_add('E',es);
				
			printf((char *)"event: %i raiders (%i raidship) for %i sec.\n", n, s, t);
			home_ast->spawn_monst(&mb_raider,   n, t * 1000 + ms_count);
			home_ast->spawn_monst(&mb_raidship, s, t * 1000 + ms_count);
			if (how_aligned(police) > 1){
										// 0   1   2   3   4   5   6				    
			    static int pol_jumpin[] = {0,  0, 75, 55, 30, 25, 24}; 
				set(event_police, pol_jumpin[MID(0,how_aligned(police),6)] );
			}
			//                                      0   1   2   3   4   5
			static int rader_camp_loss_penalty[] = {0,  3,  7, 13, 30, 65};
			int is_early_game = (ms_count < (90 * 60000) ? 1 : 0);
			int attack_again_min = ( 
			   // early game   : attacks seperated 18 to 32 minutes
			   // 1.5h playing : attacks seperated 14 to 24 minutes
			   (is_early_game ? 18 : 14) + 
			   irand(is_early_game ? 14 : 10) +
			   rader_camp_loss_penalty[MID(0, mb_raidercamp.stat_killed, 5)]  
			);
			set( event_raiders, 60 * attack_again_min );
			safty_pause();
		}
		break;
		case event_police: {
			int ha = how_aligned(police);
			int n = 0, s = 0;
			int t = 45 + irand(200);
			char *es = (char *)"A Couple of Police Show Up to Help";
			
			switch (ha) {
				case -3: n = 4; es = (char *)"Police Show Up to Arrest You"; break;
				case -4: n = 6; es = (char *)"Police Attack with SWAT Team"; break;
				case -5: s = 2; n = 8; es = (char *)"Police Send Departmental Attack";break;
				case -6: s = 4; n = 16; es = (char *)"MASSIVE Police Attack"; break;
				case 2: n = 2; es = (char *)"A Couple of Police Show Up to Help"; break;
				case 3: n = 4; es = (char *)"Several Police Officers Show Up to Help"; break;
				case 4: s = 1; n = 5; es = (char *)"Police Send a Small Detachment to Help";break;
				case 5: s = 1; n = 8; es = (char *)"Police Send a Detachment to Help"; break;
				case 6: s = 2; n = 12; es = (char *)"Police Send a Large Detachment to Help"; break;
			}
			SHIP_NOISE(INCOMMING);
			hud->short_news((char *)"POLICE DETECTED!");
			graphs->notestr_add('E',es);				
			printf((char *)"event: %i squadships (%i enforcers) for %i sec.\n", n, s, t);
			home_ast->spawn_monst(&mb_squadship,   n, t * 1000 + ms_count);
			home_ast->spawn_monst(&mb_enforcer, s, t * 1000 + ms_count);
			if (ha < -2) {
				//                                          0   1   2   3   4   5
				static int police_station_loss_penalty[] = {0,  3, 15, 30, 50, 90};
				//                                   -3  -4  -5  -6
				static int police_regroup_min_b[] = {10,  2, 35, 45};
				static int police_regroup_min_r[] = {12,  3, 12, 15};
				int ha_tix = MID(0, -ha-3, 3);
				int attack_again_min = ( 
					police_regroup_min_b[ha_tix] + irand(police_regroup_min_r[ha_tix]) + 				
				    police_station_loss_penalty[MID(0, mb_policestation.stat_killed, 5)]  
				);
				set(event_police, 60 * attack_again_min);
			}
			else unset(event_police);
			safty_pause();
		}
		break;
		case  event_stalker_clan: {
			if (how_aligned(stalker_clan) > -3) { unset(event_stalker_clan); return; }
			int ha = how_aligned(stalker_clan);
			int n = irand(6) + (ha >= -3 ? 8 : ha == -4 ? 16 : ha == -5 ? 20 : 32);
			int ret_time = 60 * (10 + irand(ha >= -3 ? 50 : ha == -4 ? 30 : 20));
			int t = 45 + irand(60);
			SHIP_NOISE(INCOMMING);
			hud->short_news((char *)"DANGER: INVISABLE SWARM DETECTED!");
			graphs->notestr_add('E', (char *)"Stalker Clan Invasion");				
			printf((char *)"event: %i stalkers for %i sec.\n", n, t);
			home_ast->spawn_monst(&mb_stalker, n, t * 1000 + ms_count);
			set(event_stalker_clan, ret_time);
			safty_pause();
		}
		break;

		case  event_hunters: if (
			mb_raider.stat_inpark == 0 && 
			mb_meteor.stat_inpark == 0 && 
			mb_tourist.stat_inpark == 0 && 
			hud->fame > 2 * hud->hunting_price && 
			hud->fame > 1000 &&
			hud->hunting_allowed &&
			how_aligned(galictic_rifle_assc) > -2
		) {
			int hunter_fame = hud->fame;
			
			for (monst_base **mb = mb_list_all; 1; mb++ ) {
				int targ = (*mb)->stat_valid_target & TARGET_HUNTER || (*mb)->ai_func == ai_plant;
				hunter_fame -= (targ ? 0 : (*mb)->stat_inpark * (*mb)->fame * 2);
				if ((*mb) == &mb_orick) hunter_fame -= (*mb)->stat_inpark * 400;
				if ((*mb) == &mb_hoveler) hunter_fame -= (*mb)->stat_inpark * 150;
				if ((*mb) == &mb_yazzert) hunter_fame -= (*mb)->stat_inpark * 150;				
				if ((*mb) == &mb_golgithan) break;				
			}   	
   			
   			if (hunter_fame > 500) { 
				int n = MID(1, irand(4) + (hunter_fame / MAX(hud->hunting_price, 100)), 10);
				int m = 1 + irand(3);
				hud->money += hud->hunting_price * n;
				hud->short_news((char *)"hunters arrived.");
				printf((char *)"event: %i hunters for %i minutes.\n", n, m);
				home_ast->spawn_monst(&mb_hunter, n, m  * 60000 + ms_count);
				set(event_hunters, 200);
			} else {
				set(event_hunters, 45); // time penalty for bad policy
				hud->long_news((char *)"Hunters think your park isn't worth hunting at.");
			}			
		} else set(event_hunters, mb_raider.stat_inpark ? 60 : 15);
		break;
		case  event_meteor_storm: {
			SHIP_NOISE(INCOMMING);
			hud->short_news((char *)"DANGER: METEOR STORM!");
			printf((char *)"event: meteor storm.\n");
			// home_ast->weather.weather_idx = wth_rain;
			// wth_meteorids
			num_meteor_rocks_left = 70; // + irand(200);
			safty_pause();
		}
		set(event_meteor_storm, 60 * (25 + irand(15)));
		break;
		case event_weather_change: {
			int wix = (home_ast->weather->weather_idx == wth_none ? wth_rain : wth_none);;
			home_ast->weather->weather_idx = wix;
			set(event_weather_change, 60 * (wix == wth_none ? 10 + irand(15) : 2 +irand(5)));
		}
		break;
		case event_weather_change_r: {
			int wix = wth_none;
			if (remote_ast) {
				wix = (remote_ast->weather->weather_idx == wth_none ? wth_rain : wth_none);;
				home_ast->weather->weather_idx = wix;
			} 
			set(event_weather_change_r, 60 * (wix == wth_none ? 6 + irand(10) : 2 +irand(5)));
		}
		break;

		}
	}
   	if (game_turn % (TURNS_PER_SEC * 60) == 1) {
		if (!hud->hunting_allowed && (mb_hippie.stat_inpark == 0)) alignment.befriend(org_hippies, 400, 2);
		else alignment.offend(org_hippies, 500);
		if (mb_crysillian.stat_inpark != 0) alignment.befriend(org_crysillians, 100, 1);
		int siz = applicants.size();
		int gen = (siz == 0 ? 2 + irand(3) : siz <= 5 ? 1 + irand(3) : siz <= 10 ? 1 + irand(2) : siz <= 15 ? 1 : siz < 30 ? irand(2) : 0);
		for (; gen; gen--) mainbase->trader_ui->generate_applicant();	
    }
    if (num_meteor_rocks_left > 0 && game_turn % (TURNS_PER_SEC / 4) == 0 && irand(4) == 0) {
		monst_t *m = home_ast->spawn_monst(&mb_meteor,1);
		if (m) m->aiu1 = 1800 + ms_count;
		num_meteor_rocks_left--;
	}
	if (hud->shake_time > ms_count) {
	    ast->camera_x += irand(3) - 1;
	    ast->camera_y += irand(7) - 3;
	}
	if (jumpgate_s->trader_ui->jg_autojump) jumpgate_s->trader_ui->jg_open();
	if (ast != remote_ast && ast != home_ast) {
		if (remote_ast) { if (ms_count + 30000 > remote_ast->exit_time) remote_ast->exit_time = ms_count + 30000;} // minigame time will probably mess up real game time
		check_minigame();
	} else if (remote_ast) {
		if ( ms_count > remote_ast->exit_time ) { delete remote_ast; remote_ast = NULL; }
	}
	home_ast->jumpgate = (remote_ast != NULL);
	if (remote_ast) remote_ast->jumpgate = (ast == remote_ast ? 1 : 0);
}
