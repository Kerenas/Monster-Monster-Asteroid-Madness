#include "mmam.h"
#include "rand.h"
#include "bubbles.h"

#if 0
void asteroid_t::populate(int idx) {
#define SPAWN_NUM(m) int num_ ## m = 0;
	FOR_EACH_SPAWN_NUM
#undef SPAWN_NUM
        switch (idx) {
        case 323 /* my bday */: artifact_idx = 1; break;   case 777: artifact_idx = 2; break;
        case 504: artifact_idx = 3; break;     case 911: artifact_idx = 4; break;
        case 411: artifact_idx = 5; break;     case 666: artifact_idx = 6; break;// very pissed monsters + guisers.
        default:;
        }
        int plants = (idx % 13 == 1 ? 0 : idx % 13 == 5 ? deviate(30) :
           deviate(80)  // 230
        );
        int vulcanos = 0, guisers = 0;
        int norms = deviate(12);
        int herds = (idx % 5 == 4 ? deviate(6) : 0);
        if (idx == 0) {
        	num_shrub = num_fern = 12;
        } else {
	        num_toothworm = (idx % 11 == 0 ? 3 : 0 );
        	num_throbid = (idx % 33 < 7 ? deviate(3) : 0);
	        num_uberworm = 0;
        	num_roachy = (idx % 5 < 2 ? deviate(8) : 0);
                if (idx % 100 == 77) num_crysillianruins = 1;
                if (idx % 100 == 84) num_policestation = 1;
                if (idx % 100 == 96) num_wisehut = 1;
                if (idx % 100 == 92) num_crashsite = 1;
	        if (idx % 100 == 33) num_stalker = 6;
	        if (idx % 100 == 14) num_krulasite = 6;
	        if (idx % 100 == 24) num_cheech = 2;
	        if (idx % 100 == 48) num_hoveler = 8;
                if (idx %  50 == 24) { num_orickcampfire = 1; }
                if (idx % 200 == 75) { num_orickcamp = 1; num_orickcampfire = 6; } // each clan has a main base.
        	switch (idx % 33) {
	        case 0: /* void */ break;
	        case 1: plants /= 3; num_roachy = deviate(90);/* roachy infested */ break;
	        case 2: num_rexrex  = deviate(8);  if (irand(5)==0) num_bigasaurus = 1; /* rexrex */ break;
        	case 3: num_ackatoo = deviate(7);  num_throbid = deviate(22); if (irand(5)==0) num_bigasaurus = 1; /* dinosaur */ break;
	        case 4: vulcanos = deviate(20); /* volcanic */ break;
	        case 7: // Quito swamp
	           num_quitofly = deviate(40);
	           if (irand(5)==0) num_mamothquito = deviate(9);
	           if (irand(5) < 3) num_frogert = deviate(6);
                   if (irand(1)) num_furbit = deviate(16);
                   else if (irand(1)) num_roachy = deviate(12);
                   if (irand(2)) num_quiver = deviate(4); else num_slymer = deviate(5);
                   num_krakin = deviate(2);
                   if (irand(8)==0) num_uberworm = 1;
                   break;
	        case 8: // slime world
                   num_bervul  = deviate(3);
            	   num_frogert   =  deviate(9);
            	   num_quiver = deviate(6);
                   num_slymer = deviate(12);
	        break;
        	case 9: guisers = deviate(80); /* guiser */ break;
	        case 10: num_squarger  = deviate(idx % 7); /* sneekers */ break;
        	case 11: num_sneeker = (idx % 7); /* sneekers */ break;
        	case 12: num_raidercamp = 1; num_raider = deviate(30); num_raidship = deviate(4); break;
	        case 13: // squeeker, meeker, sneeker
	          num_squeeker = deviate(20);
        	  if (irand(5)==0) num_meeker = 1;
        	  if (irand(5)==0) num_sneeker = (idx % 4);
        	break;
        	case 14: // dragonigon world
            	  if (irand(3))	num_yazzert = deviate(10);
            	  if (irand(3))	num_vaerix  = deviate(8);
                  if (irand(4) == 0) num_dragoonigon = 1;
                  num_squarger = deviate(16);
                case 5:
	    	   num_molber  = deviate(20);
	           num_drexler  = deviate(6);
                   if (irand(4)) num_krakin  = deviate(3); else num_titurtilion = 1;
        	break;
        	case 17: /* worm asteroid */
	             norms /= 2;
        	     herds = 0;
	             num_uberworm = (idx % 6);
        	     num_toothworm = deviate(40);
	        break;

        	case 18: num_jmart = 1; building_sign = SPR_SIGN_JMART; break;
        	case 19: num_arena = 1; building_sign = SPR_SIGN_ARENA; break;
        	case 20: num_bfc = 1;   building_sign = SPR_SIGN_BFC; break;
        	default:;
        	}
        	if (num_jmart || num_bfc || num_arena || num_raidercamp || num_policestation) {
			num_shrub =  num_fern = plants / 2;  num_stalkplant =  num_happytree = 0;
			num_toothworm = num_throbid = num_uberworm = num_roachy = 0;
			num_crysillian =  num_stalker = 0;
           	} else switch (idx % 8) {
		case 0: num_shrub =  num_stalkplant = num_fern = plants / 4; num_happytree = 0;
        		num_furbit = plants / 8;   num_pygment = deviate(3);  num_shruver = deviate(3);
		break;
        	case 1: num_shrub = plants; num_stalkplant = num_fern = num_happytree = 0;
        		if (irand(1)) num_furbit = plants / 2; else num_shruver = plants / 2;
        		num_mubark = deviate(5);
        	break;
		case 2: num_fern = num_shrub = plants / 3;  num_happytree = num_stalkplant = 0;
                          num_molber = plants / 6;
			break;
        	case 3: num_fern = plants;  num_shrub = plants / 3; break;

		case 4: num_shrub = num_stalkplant = num_fern = 0;  num_happytree = plants;
		if (plants > 30) num_mimmack = 1;  num_mubark = plants / 6+1; num_throbid = num_mubark / 6;
		  num_squeeker = 6;
		  if (irand(3) == 0) num_squarger = 1;
		  if (irand(3) == 0) num_titurtilion = 1;
                  if (irand(3) == 0) num_meeker = 1;
		break;
		case 5: num_stalkplant = plants;  num_happytree = plants / 3; num_pygment = plants / 6;	break;
		case 6: num_shrub = num_stalkplant = num_fern = plants / 4;  num_happytree = plants / 8; break;
		case 7: num_stalkplant = plants; num_pygment = plants / 3; break;
        	}
       	}

        for (;num_crysillianruins; num_crysillianruins--) {
        	monst_t *r = spawn_monst(&mb_crysillianruins);
        	if (r) for (int c = 0; c < 3; c++) add_monst_by(new monst_t(&mb_crysillian), r->x, r->y);
        }
        for (;num_orickcampfire; num_orickcampfire--) {
        	monst_t *f = spawn_monst(&mb_orickcampfire);
        	if (f) for (int c = 0; c < 5; c++) add_monst_by(new monst_t(&mb_orick), f->x, f->y);
        }
        for (;num_crashsite; num_crashsite--) {
        	monst_t *s = spawn_monst(&mb_crashsite);
        	if (s) {
                        monst_t *cryo;
           		for (int c = 3 + irand(6); c; c--) add_monst_by(new monst_t(&mb_biocite), s->x, s->y);
           		switch (irand(6)) {
                        case 0:  add_monst_by(new monst_t(&mb_deathray), s->x, s->y); break;
                        case 1:  add_monst_by(new monst_t(&mb_aisormba), s->x, s->y); break;
                        case 2:
                        	cryo = new monst_t(&mb_cage5);
                                cryo->weap_rounds = 0;
                        	add_monst_by(cryo, s->x, s->y);
                	break;
                        case 3:
                        	cryo = new monst_t(&mb_cage5);
                                cryo->weap_rounds = 1;
                        	add_monst_by(cryo, s->x, s->y);
                        break;
               		}
            	}
        }
        if (num_policestation) {
		spawn_city();
        } // end cityspawn
	monst_base *rock = NULL;
        int rock_num = irand(12);
        switch (skin_num) {
	case 1 : rock = &mb_purplerock; break;
        case 2 : rock = &mb_yellowrock; break;
        case 3 : rock = &mb_greencrystals; rock_num /= 2; break;
        case 6 : rock = &mb_biocite; rock_num /= 4; break;
        case 9 : rock = &mb_redrock; break;
        }

        if (rock && rock_num) spawn_monst(rock, rock_num);
#define SPAWN_NUM( m) spawn_monst(&mb_ ## m, num_ ## m );
	FOR_EACH_SPAWN_NUM
#undef  SPAWN_NUM
	if (num_raidercamp) for(list<monst_t *>::iterator mon = m.begin(); mon != m.end(); mon++) {
		monst_t *b = *mon;
		if (b->base == &mb_raidercamp) {
    			int dc = irand(8);
                	for (int c = 0; c < dc; c++) {
                   		float a = M_PI * 2. * c / dc;
                   		float d = 260.;
                   		int r = irand(100);
                   		monst_t *nm = new monst_t(
					r < 20 ? &mb_raiderguntower :
				 	r < 60 ? &mb_raiderdgtower :
				 	r < 70 ? &mb_raiderxcannontower : &mb_raidergaltixtower
				);
                   		add_monst_by( nm, b->x + cos(a) * d, b->y + sin(a) * d );
            		}
			if (dc) add_monst_by(new monst_t(&mb_raiderfissionplant), b->x, b->y);
    		}
        }
        list<monst_t *> hole_list;
	for(list<monst_t *>::iterator mon = m.begin(); mon != m.end(); mon++) {
		monst_t *b = *mon;
		if (b->flags & AF_DIGS) {
    			monst_t *nm = new monst_t(&mb_hole);
                        if (nm) {
                        	nm->size = b->size;
                        	nm->radius = b->radius;
                        	nm->x = b->x; nm->y = b->y;
	                }
                        hole_list.push_front(nm);
    		}
        }
        for(list<monst_t *>::iterator mon = hole_list.begin(); mon != hole_list.end(); mon++) add_monst(*mon);
        hole_list.clear();
}
#endif

void asteroid_t::spawn_city(int idx, int city_flags) {
        	// city spawn
                int num_policestation = 1;
                int num_bfc = 1;
                int num_jmart = 1;                
        	num_jmart = irand(2);
        	num_bfc =   irand(2);
        	int more_buildings = 150;
        	int num_suburbs = irand(8) + 1;
        	struct burb {int t,l,r,b, rows, cols;  } burb_stat[8];
        	int misses = 50;
        	int bsp = 80;
                for (int c = 0; c < num_suburbs && more_buildings; c++) {
                        burb *b = &burb_stat[c];
                        int ok;
                	do {
                   		b->cols =  3 + irand(5);
                   		b->rows =  3 + irand(5);
                   		b->t = (irand(ph - 160 - (b->rows + 1) * bsp) + 80);
                   		b->l = (irand(pw - 160 - (b->cols + 1) * bsp) + 80);
                   		b->r = b->l + (b->cols + 1) * bsp;
                   		b->b = b->t + (b->rows + 1) * bsp;
                   		ok = 1;
                                for (burb *f = burb_stat; ok && f != b; f++) {
                                        ok = (
                                        	(b->t < f->b) || (b->b > f->t) ||
                                        	(b->l < f->r) || (b->r > f->l)
                                	);
                                }
                		if ((--misses) == 0) {num_suburbs = c; break; }
        		} while (!ok);
                        if (ok) for (int yy = 0; yy < b->rows; yy++) for (int xx = 0; xx < b->cols; xx++) if (more_buildings) {
                            int r = irand(more_buildings);
                            monst_base *mb = (r % 22 == 0 ? NULL : r % 5 ? &mb_cityblda : &mb_citybldb);
                            if (num_policestation && r % 50 == 0) { mb = &mb_policestation; num_policestation--; }
                            if (num_bfc && r % 50 == 1) { mb = &mb_bfc; num_bfc--; }
                            if (num_jmart && r % 50 == 2) { mb = &mb_jmart; num_jmart--; }
                            if (mb) {
                      		add_monst_by(new monst_t(mb), b->l + xx * bsp, b->t + yy * bsp);
                                more_buildings--;
                            }
                        }
		}
}


static int jg_recharge[5] = {
	100000, //L0 = 1:40
	 85000, //L1 = 1:25
	 75000, //L2 = 1:15
	 45000, //L3 = 0:45
     10000  //L4 = 0:10
};

static int jg_open_time[5] = {
	 90000, //L0 = 1:30
	120000, //L1 = 2:00
	180000, //L2 = 3:00
	300000, //L3 = 5:00
    480000  //L4 = 8:00
};



void asteroid_t::add_bullet(monst_t *m, float ang, struct ivalu_t *w ) {
	if (!m || !w) return;
	float r = m->radius * 0.9;
	int bc = temp_bullets.size() + bullets.size();
	int ok = (bc < 800 || m == ship ? 1 : bc > 1200 ? 0 : irand(400) > bc - 800);
	if (ok) temp_bullets.push_back(new killer_particle_t(this, m, m->x + r * cos(ang), m->y + r * sin(ang), ang, w));
}

void asteroid_t::drop_spore(monst_t *m, struct ivalu_t *w ) {
	if (!m || !w) return;
	killer_particle_t *k= new killer_particle_t(this, m, m->x, m->y, 0., w);
	k->spore_freeze();
	temp_bullets.push_back(k);
}


//============================================================================

asteroid_t::asteroid_t(int idx, int in_script, monst_t *set_encl) {
	asteroid_idx = idx;
	for (int c = 0; c < org_last; c++) orgs_here[c] = 0;
	if (set_encl) {
		tw = th = (int)(set_encl->radius * 2 / TILE_SIZE + 1);
			pw = ph = tw * TILE_SIZE;
		skin_num = 6;
		jumpgate = jumpgate_x = jumpgate_y = 0;
	} else {
		tw = th = 28 + ((idx % 15) * 2); // always multiple of 2
		if (idx == 0) tw = th = 50; // always multiple of 2
		pw = ph = tw * TILE_SIZE;
		skin_num = (asteroid_idx %  NUM_SURFACES);
		jumpgate = (idx != 0);
		jumpgate_x = (idx ? irand(pw / 2) + 60 : 80 );
		jumpgate_y = (idx ? irand(ph-120) + 60 : 80 );
	}
	arena_glyph = -1;
	start_time = 0;
	grid = (int **) calloc(sizeof (int *), th);
	for (int c = 0; c < th; c++) {
		grid[c] = (int *)calloc(sizeof(int), tw);
	}
	ugly_reseed(idx);
	for (int yy = 0; yy < th; yy++) for (int xx = 0; xx < tw; xx++) grid[yy][xx] = ugly_rand() % 16 + (16 * skin_num);
exit_time = (idx == 0 ? 0 : ms_count + (jumpgate_s ? jg_open_time[jumpgate_s->base->inv_link->lvl] : 0));
	building_sign = 0;
	artifact_idx = 0;
	camera_x = camera_y = 0;
	weather = new weather_t(this);
	if (!in_script && !set_encl) {
   		char sfn[20];
   		snprintf(sfn, sizeof sfn, "ast%03i.mmam", idx);
        	if (! script_load(sfn, this) ) populate(idx);
		printf((char *)"AST #%i has %i monsters\n",asteroid_idx, m.size());

   	}
   	for (list<monst_t *>::iterator mon = m.begin(); mon != m.end(); ++mon) {
   		if ((*mon)->base == &mb_arena) building_sign = SPR_SIGN_ARENA;
   		if ((*mon)->base == &mb_bfc)   building_sign = SPR_SIGN_BFC;
   		if ((*mon)->base == &mb_fisec) building_sign = SPR_SIGN_FISEC; 
   		if ((*mon)->base == &mb_jmart) building_sign = SPR_SIGN_JMART;  
   		if ((*mon)->base == &mb_policestation) building_sign = SPR_SIGN_POLICE;
   	}
}

void asteroid_t::clear() {
	for(list<monst_t *>::iterator mon = m.begin(); mon != m.end();) {
		if (((*mon == ship) || ((*mon)->ai_func == ai_player) || ((*mon)->base == &mb_worker)) && this != home_ast) {
			home_ast->jump_in(*mon);
		} else {
			delete *mon; 
		}
       	mon = m.erase(mon);
    }
	for(list<blood_t *>::iterator bl = blood.begin(); bl != blood.end();) {
		delete *bl;
		bl = blood.erase(bl);
	}
	for(list<jg_particle_t *>::iterator jgp = jg_particle.begin(); jgp != jg_particle.end();) {
		delete *jgp;
		jgp = jg_particle.erase(jgp);
	}
	for(list<killer_particle_t *>::iterator killerp = bullets.begin(); killerp != bullets.end();) {
		delete *killerp;
		killerp = bullets.erase(killerp);
	}
	if (ast == this) ast = home_ast;

}

asteroid_t::~asteroid_t() {
	if ( arg_nobones == 0 && make_nobones == 0 && asteroid_idx ) {
		char bones_fn[200];
		snprintf(bones_fn, sizeof bones_fn, "%sbones/ast%03i.mmam", home_dir, asteroid_idx);
		asteroid_t *aa = ast;  ast = this;
		dump_ast(bones_fn, SAVE_BONES);
		ast = aa;
	}
	printf((char *)"BEGIN DIE ASTEROID\n");
	clear();
	delete weather;
	for (int c = 0; c < th; c++) free(grid[c]);
	free(grid);
	if (this == remote_ast && jumpgate_s) {
		jumpgate_s->trader_ui->jg_respawn_time = ms_count + jg_recharge[jumpgate_s->base->inv_link->lvl];
	}
	printf((char *)"END DIE ASTEROID\n");
}


void asteroid_t::explode(float x, float y, float r, float inten, monst_t *who_fired, monst_t *ignore, int typ) {
        if (r<1. || inten < 1.) return;
       	for(list<monst_t *>::iterator kill = m.begin(); kill != m.end(); ++kill) {
                monst_t *mm = *kill;
                if (mm != ignore && (mm->flags & AF_NOSHOOT) == 0) {
                	float d = MID(0., 1. - dist(x,y, mm->x, mm->y)/ r, 1.);
                        int dam = (int)(floor((d * d + d) / 2. * inten));
                        if (dam > 0) mm->hit(dam, who_fired);
                        if (d > 0) {
	                        if (typ & WP_PSY) {
        	                         mm->energy -= MIN(dam / 5, mm->energy);
                	        }
                        	if ((typ & WP_FIRE) && mm->flammable()) {
                               		mm->on_fire = ms_count + ON_FIRE_TIME;
	                        }
                        }
                }
                if (monst_iter_unsafe) for (kill = m.begin();kill != m.end(); ++kill) {
                    if (mm == *kill) break; // cannot set to safe. I'm in a loop.
                } 
        }

}

int monst_t::sort_height() {
	if (dead()) return 0;
	int gr;
	switch (ai_func) {
	case ai_building: gr = 50; break;
	case ai_turret: gr = 80; break;
	case ai_slymer: gr = 100; break;
   	case ai_item: gr = 150; break;
   	case ai_plant: gr = 220; break;
   	default: gr = 250;
 	} 	
    if (gr > 200) {
		if (base->flags & AF_FLYING) gr += 500;
		gr += (base->size < mb_rexrex.size ? 0 : base->size < mb_drexler.size ? 10 : 20);
    }
    return gr;
}

void asteroid_t::add_monst(monst_t *monst) {
	if (!monst) return;
	monst_iter_unsafe = 1;
	list<monst_t *>::iterator ibeg = m.begin();
	list<monst_t *>::iterator iend = m.end();
	list<monst_t *>::iterator isorter = ibeg;
	int steps = m.size();
	int mh = monst->sort_height();
	while (ibeg != iend) {
		isorter = ibeg;
		steps = (int)(steps / 2);
	  	for (int i = 0; i < steps; i++) isorter++;
	  	if ( mh >= (*isorter)->sort_height() ) {
			ibeg = isorter;
		 	if (steps == 0 && ibeg != iend) ibeg++;
	  	} else {
		 	iend = isorter;
		 	if (steps == 0 && iend != ibeg) iend--;
	  	}
	}
	isorter = iend;
	m.insert(isorter, monst);
	monst->my_ast = this;
	if (monst->ast_from == -1) monst->ast_from = asteroid_idx;
	if (!arg_mute_add) printf((char *)"monst %s added to asteroid %i.\n", monst->name, asteroid_idx);
}

void asteroid_t::add_monst(monst_t *monst, float sx, float sy) {
	if (!monst) return;
	add_monst(monst);
	monst->x = sx;
    monst->y = sy;
}

monst_t *asteroid_t::monst_near(float atx, float aty, float atr, monst_t *ignore, int show_dead) {
	for(list<monst_t *>::iterator mi = m.begin(); mi != m.end(); mi++) {
		monst_t *chk = *mi;
        if ((chk != ignore && (show_dead || chk->alive())) && ( dist(atx, aty, chk->x, chk->y)) < (chk->radius + atr) ) return chk;
   	}
	return NULL;
}


monst_t *asteroid_t::monst_at(float atx, float aty, monst_t *ignore, int show_dead) {
	for(list<monst_t *>::iterator mi = m.begin(); mi != m.end(); mi++) {
		monst_t *chk = *mi;
        if ((chk != ignore && (show_dead || chk->alive())) && ( dist(atx, aty, chk->x, chk->y)) < chk->radius) return chk;
   	}
	return NULL;
}

monst_t *asteroid_t::monst_at(PMASK *mask, float atx, float aty, monst_t *ignore, int show_dead) {
    if (!mask) return monst_at(atx, aty, ignore,  show_dead);
	for(list<monst_t *>::iterator mi = m.begin(); mi != m.end(); mi++) {
		monst_t *chk = *mi;
		if ((chk != ignore && (show_dead || chk->alive())) &&
			check_pmask_collision(mask, &chk->mask, (int)(atx), (int)(aty), (int)(chk->x), (int)(chk->y))
		) return chk;
    }
	return NULL;
}

int asteroid_t::add_monst_by(monst_t *monst, float x, float y, int avoid_danger) {
        float sof = (M_PI / 3.) * frand();
        int checks = 0;
        for (float r = 20.; r < pw; r += 20.) for (float cc = sof; cc < (2. * M_PI); cc += (M_PI / 9.)) {
        	float cx = (x + r * cos(cc));
        	float cy = (y + r * sin(cc));
                if ( cx < 0. || cx > pw || cy < 0 || cy > ph || dist(cx,cy,jumpgate_x,jumpgate_y) < 90 ) ; else {
                	checks++;
                        int space_clean = 1;
                	if (avoid_danger) {
                  		//later
                	}
			for(list<monst_t *>::iterator mi = m.begin(); mi != m.end(); mi++) {
    				monst_t *chk = *mi;
				if (dist(chk->x, chk->y, cx, cy) < (1.2 * (chk->radius + monst->radius)) ) {
                                	space_clean = 0;
			      		break;
        			}
        		}
                	if (space_clean) {
                		add_monst(monst);
                		monst->x = cx;
                		monst->y = cy;
                		return 1;
                	}
       		}
        }
        printf((char *)"monst %s add failed! (%i checks)\n",monst->name, checks);
        return 0;

}


void asteroid_t::delete_monst(monst_t *monst) {
	if (monst == ship) ship = NULL;
    unhook_monst(monst);
 	delete monst;
}

list <monst_t *>::iterator asteroid_t::delete_monst(list <monst_t *>::iterator mon) {
	monst_t *monst = *mon;
	if (monst == ship) ship = NULL;
    delete monst;
 	return m.erase(mon);
}


void asteroid_t::_forget_monst(monst_t *monst) {
  	for(list<monst_t *>::iterator mon = m.begin(); mon != m.end(); ++mon) (*mon)->forget(monst);
    for(list<killer_particle_t *>::iterator killerp = bullets.begin(); killerp != bullets.end(); ++killerp) {
	     if ((*killerp)->who_fired == monst) (*killerp)->who_fired = NULL;
    }
    for(list<killer_particle_t *>::iterator killerp = temp_bullets.begin(); killerp != temp_bullets.end(); ++killerp) {
	     if ((*killerp)->who_fired == monst) (*killerp)->who_fired = NULL;
    }

}

void asteroid_t::unhook_monst(monst_t *monst) {
  	m.remove(monst);
  	_forget_monst(monst);
  	monst_iter_unsafe = 1;
}


int asteroid_t::spawn_monst(monst_t *nmonst, int jump_out) {
	if (!nmonst) return 0;
	monst_t *king_of_herd = NULL;
	int herd_size = MAX(1, nmonst->base->herd_size);
	if (herd_size > 1) {
		int hc = -1;
		for(list<monst_t *>::iterator mon = m.begin(); mon != m.end(); mon++) if ((*mon)->base == nmonst->base) {
			if ((++hc) % herd_size == 0) king_of_herd = *mon;
		}
		if ((++hc) % herd_size == 0) king_of_herd = NULL;
	}
	int spawn_x = irand(pw);
	int spawn_y = irand(ph);
	if (king_of_herd) {
        add_monst_by(nmonst, king_of_herd->x, king_of_herd->y);
    } else add_monst_by(nmonst, spawn_x, spawn_y);
	if (jump_out > 0) {
		nmonst->jump_out_time = jump_out;
		add_jumpgate(nmonst->x, nmonst->y);
	}
	return 1;
}


monst_t *asteroid_t::spawn_monst(monst_base *mb, int n, int jump_out) {
	if (n <= 0) return NULL;
	monst_t *king_of_herd = NULL;
        int herd_size = MAX(1, mb->herd_size);
        if (herd_size > 1) for(list<monst_t *>::iterator mon = m.begin(); mon != m.end(); mon++) if ((*mon)->base == mb) {
		king_of_herd = *mon;
                break;
        }
        int hc = 0;
        monst_t *nmonst = NULL;
        for (int c = 0; c < n; c++ ) {
                nmonst = new monst_t(mb);
		int spawn_x = irand(pw);
		int spawn_y = irand(ph);
                if ((++hc) % herd_size == 0) {
       	        	king_of_herd = nmonst;
        		add_monst_by(nmonst, spawn_x, spawn_y);
                } else if (king_of_herd) {
                  	add_monst_by(nmonst, king_of_herd->x, king_of_herd->y);
        	} else add_monst_by(nmonst, spawn_x, spawn_y);
        	if (jump_out > 0) {
           		nmonst->jump_out_time = jump_out;
           		add_jumpgate(nmonst->x, nmonst->y);
            }
        }
        return nmonst;
}

monst_t *asteroid_t::spawn_monst_by(monst_base *mb, float x, float y, int n,  int jump_out, float rngfar, float rngnear) {
	if (n <= 0) return NULL;
	monst_t *nmonst = NULL;
	for (int c = 0; c < n; c++ ) {
		nmonst = new monst_t(mb);
		float d = (rngfar - rngnear) * frand() + rngnear;
		float a = 2. * M_PI * frand();
		int spawn_x = (int)(x + d * cos(a));
		int spawn_y = (int)(y + d * sin(a));
		add_monst_by(nmonst, spawn_x, spawn_y);
		if (jump_out > 0) {
			nmonst->jump_out_time = jump_out;
			add_jumpgate(nmonst->x, nmonst->y);
		}
	}
	return nmonst;
}


void asteroid_t::spatter_blood(monst_t *m, float ang, float vel, int amt, int alt_fluid) {
	blood.push_back(new blood_t(this, m, ang, vel, amt, alt_fluid));
}

void asteroid_t::add_jumpgate(float x, float y, int typ) {
	jg_particle.push_back(new jg_particle_t(this, x, y, typ));
}

monst_t *asteroid_t::jump_in(monst_t *monst) {
	add_monst(monst);
	monst->x = 60 + irand(pw-60);
    monst->y = 60 + irand(ph-60);
	add_jumpgate(monst->x, monst->y);
	return monst;
}

monst_t *asteroid_t::jump_away(monst_t *monst) {
	add_jumpgate(monst->x, monst->y);
	monst->forget_all();
    _forget_monst(monst);
    return monst;
}

int  asteroid_t::poll() {
	if (ship && this == ast) {
		int ctx = (int)(ship->x - (hud->ox / 2));
		int cty = (int)(ship->y - (SCREEN_H / 2));
		if (ctx != camera_x) camera_x += ((ctx - camera_x) / 4 );
		if (cty != camera_y) camera_y += ((cty - camera_y) / 4 );
		if (camera_x < 0) camera_x = 0; else if (camera_x + hud->ox  > pw) camera_x = pw - hud->ox;
		if (camera_y < 0) camera_y = 0; else if (camera_y + SCREEN_H > ph) camera_y = ph - SCREEN_H;
	}
	if (ms_count < start_time) return 0;
	int oh[org_last], pg[org_last];
	int sc[5];
	if (asteroid_idx > 0 && asteroid_idx < 1000) hud->marked[asteroid_idx] = 1;
	stabilizer_lvl = 0;
	memset(oh,0, sizeof oh);   memset(pg,0, sizeof pg);   memset(sc,0, sizeof sc);
	memset(orgs_poweru,0, sizeof orgs_poweru);   memset(orgs_powerr,0, sizeof orgs_powerr);
	if (this == home_ast) {
		hud->fame = 0;
		for (int c = 0; mb_list_all[c]; c++) mb_list_all[c]->stat_inpark = 0;
	}
	for(list<monst_t *>::iterator mon = m.begin(); mon != m.end();) {
		if ( ((*mon)->decay_time < ms_count) &&
		     (*mon)->dead() &&
		     (*mon) == thehuman 
		     // || (*mon)->base == &mb_netship
		) {
			if (mainbase->alive() ) {
				// (*mon)->decay_time = ms_count + 12 * 60000;
				ast = home_ast;
				thehuman->hp = thehuman->mhp;
				thehuman->energy = 30;
				thehuman->forget_all();
				thehuman->do_act(ACT_CHILL);
				unhook_monst(thehuman);
				ast->add_monst_by(thehuman, mainbase->x, mainbase->y);
				ast->add_jumpgate(thehuman->x, thehuman->y);
				thehuman->ai_func = ai_player;
				hud->money -= MIN(2000, hud->money); 
			}  else gamedone = 1;
            return 0;
        }   
		if ( ((*mon)->decay_time < ms_count) &&
		     (*mon)->dead() &&
		     (*mon)->base == &mb_netship && 
		     (*mon)->ai_func == ai_player
		) {
			if (mainbase->alive() ) {
				monst_t *save_ship = ship;
				monst_t *ns = ship = new monst_t(&mb_netship);
				(*mon)->ai_func = ai_item;
				hud->money -= MIN(hud->money, MID(2500,hud->money / 2, 12000 ));
				// upgrade funcs require ship == the netship !
				hud->upgrade(&IDF_ROPEUP0);
				hud->upgrade(&IDF_CLAWSPEEDUP0);
				hud->upgrade(&IDF_THRUST0);
				hud->upgrade(&IDF_CLAW0);
				hud->upgrade(&IDF_HULL0);
				ast->add_monst_by(ns, mainbase->x, mainbase->y);
				ast->add_jumpgate(ns->x, ns->y);
				ns->ai_func = ai_player;
				ship = save_ship;
				SHIP_NOISE(RESPAWN);
			} else gamedone = 1;
            return 0;
		}
		if ( ((*mon)->decay_time < ms_count) &&
		     (*mon)->dead() &&
		     ((*mon) != hud->hooked_monst) &&
                     (*mon)->base != &mb_astbase &&
                     (*mon) != jumpgate_s &&
                     (*mon) != ship &&
                     (*mon) != thehuman                     
		     ) {
                	mon = delete_monst(mon);
  		} else {
			monst_t *mq = (*mon);
			if (mq->base == &mb_hole && mq->hole_monst) mq = mq->hole_monst;
			monst_t *old_hole = mq->hole_monst;
			monst_iter_unsafe = 0;
			mq->poll();
			if (monst_iter_unsafe) {
				for (mon = m.begin(); mq != (*mon); mon++);
				monst_iter_unsafe = 0;
			}
			if (mq->alive() && asteroid_idx == 0) {
				mq->base->stat_inpark++;
				// freek of nature bonus for really massive animals within a group.
				if (mq->size > mq->size * 5) hud->fame += 500;
				hud->fame += mq->fame_bonus;
			}
			if (mq->alive()) {
				if (mq->base == &mb_radiomarker && asteroid_idx > 0 && asteroid_idx < 1000) hud->marked[asteroid_idx] = 2;
				if (mq->pow_on && mq->base->inv_link &&
						mq->base->inv_link->typ == inv_scanner
				) sc[mq->base->inv_link->sv]++;				
				if (mq->encl) mq->encl->poll();
				oh[mq->org_idx]++;
				mq->pow_on = 0;
				if (mq->pow_drain <= 0) {
					pg[mq->org_idx] -= mq->pow_drain;
					mq->pow_on = 1;
					if (mq->base == &mb_powercore) {
						int cpow = (int)(2000. * (1.0 - (float)(mq->hp) / mq->base->mhp) - mq->pow_drain);
						for (int c = 0; c < org_last; c++) pg[c] += cpow;
					} else pg[mq->org_idx] -= mq->pow_drain;
				} else if (mq->on) {
					orgs_powerr[mq->org_idx] += mq->pow_drain;
					if (mq->pow_drain <= orgs_powerg[mq->org_idx] - orgs_poweru[mq->org_idx]) {
						orgs_poweru[mq->org_idx] += mq->pow_drain;
						mq->pow_on = 1;
					}
				}
			}
			if (mq->hole_monst != old_hole) {
				if (mq->hole_monst) mon = m.erase(mon); // monster goes in hole
				else {
					add_monst(mq); // jumps out hole
						for (mon = m.begin(); old_hole != (*mon); mon++);
					mon++;
				}
			} else if ( mq->hp < mq->mhp * 3 / 4 && mq->building_squads) {
				int n[2] = {6, 0};
				monst_base *def[2] = {&mb_sentry, NULL};
				if (mq->base == &mb_raidercamp) { def[0] = &mb_raider; }
				if (mq->base == &mb_policestation) { def[0] = &mb_squadship; def[1] = &mb_enforcer; n[0] = 8;  n[1] = 2; }
				if (mq->base == &mb_orickcamp) { def[0] = &mb_orick; }
					if (mq->base == &mb_fisec) { def[0] = &mb_sentinel; def[1] = &mb_watcher; n[0] = 8;  n[1] = 2 + irand(4); }
				if ( mq->base == &mb_bfc ) {
					def[0] = &mb_sentry; n[0] = 10 + irand(8);
				};
				if (mq->base == &mb_jmart || mq->base == &mb_arena) {
					if (irand(3) == 0) def[0] = &mb_sentinel; n[0] = 6 + irand(3);
							def[1] = &mb_watcher; n[1] = irand(3);
				};
				for (int c = 0; c<2; c++) for (int c2 = 0; c2 < n[c]; c2++) {
					monst_t *fighter = new monst_t(def[c]);
					add_monst_by(fighter, mq->x, mq->y);
					fighter->who_hit_me_last = mq->who_hit_me_last;
   				}
   				mq->building_squads--;
                        	for (mon = m.begin(); mq != (*mon); mon++);
                        	mon++;
                        } else if (mq->jump_out_time && (ms_count > mq->jump_out_time) && mq->agression > 100 && mq->energy > 100 && mq->my_ast) {
                                // self jump check
                                mon = m.erase(mon);
				jump_away(mq);
				delete mq;
              		} else {
				// jumpgate check
				asteroid_t *to_ast = (mq->my_ast == home_ast ? remote_ast : home_ast );
                		if (
                			jumpgate && dist(mq->x, mq->y, jumpgate_x, jumpgate_y) < 50 &&
                			to_ast && mq->my_ast &&
                			!(mq->base->inv_link && mq->base->inv_link->typ == inv_stabilizer)

                		) {
         					printf((char *)"%s hit jumpgate to AST #%03i.\n",mq->name,to_ast->asteroid_idx);
                            mon = m.erase(mon); // actually removes from list
         					jump_away(mq); // only forgets monster
                            to_ast->jump_in(mq);
                		} else mon++;
					}
          	}
        }
		memcpy(orgs_here,oh, sizeof oh);
		memcpy(orgs_powerg,pg, sizeof pg);
		memcpy(scanner_count,sc, sizeof sc);
		if (this == home_ast) {
		/*
		if 1 firbit is +100 fame, a second firbit might only be +60 3rd = 35  fourth = 23
		so 4 firbits = 218 fame
		10 firbits = 300 fame
		it's like going to the zoo, yes 12 tigers is better than one, but not 12 times better
		
		todo:  bonus would be a constant for what level of compition it won at
		4 is ~ 2x  10 ~ 3x 100 ~ 12x
		*/
		for (int c = 0; mb_list_all[c]; c++) if (mb_list_all[c]->stat_inpark) {
			float nm1 = mb_list_all[c]->stat_inpark - 1.;
			hud->fame += (int) (mb_list_all[c] == &mb_hippie ? -100 * nm1 :
				mb_list_all[c]->fame * (2. - pow(0.5, nm1) + 0.10 * nm1)
			);
		}
	}

	for(list<blood_t *>::iterator bl = blood.begin(); bl != blood.end();) {
   		if (ms_count < (*bl)->dry_time){
			(*bl)->poll();
			bl++;
		} else {
			delete *bl;
				bl = blood.erase(bl);
		}
    }
    for(list<jg_particle_t *>::iterator jgp = jg_particle.begin(); jgp != jg_particle.end();) {
   		if ((*jgp)->size >= 0) {
			(*jgp)->poll();
			jgp++;
		} else {
			delete *jgp;
			jgp = jg_particle.erase(jgp);
		}
    }
	for(list<killer_particle_t *>::iterator bl = bullets.begin(); bl != bullets.end();) {
   		if (ms_count < (*bl)->dry_time ){
			(*bl)->poll();
			bl++;
		} else {
			(*bl)->spore_check();
			delete *bl;
			bl = bullets.erase(bl);
		}
    }
	for(list<killer_particle_t *>::iterator bl = temp_bullets.begin(); bl != temp_bullets.end();) {
				bullets.push_back(*bl);
				bl = temp_bullets.erase(bl);
	}
	if (weather) weather->poll();
	return 1;
}

void asteroid_t::main_jumpgate_draw() {
	if (jumpgate) {
		float a = ( (ms_count % 31415) * 0.002); // workarround for spinstop
		int xx = (int)(jumpgate_x - ast->camera_x);
		int yy = (int)(jumpgate_y - ast->camera_y);
        	sprite_rsw(active_page, (BITMAP *)sprites[SPR_JUMPGATE].dat, xx, yy, a, 120,
			200 + (abs(ms_count % 2600 - 1300) / 50)
		);
	}
}

extern char debug_sweetness_points_str[1024];


void asteroid_t::draw() {
	int ty = (camera_y / TILE_SIZE);
	int yy = -(camera_y % TILE_SIZE);
	int rs = (trader ? trader->ox + trader->kmenu->w - 220 : 0); if (rs < 0) rs = 0;
	int xs = (camera_x + rs);
	int left_col = (xs / TILE_SIZE);
	int xx_start = rs -(xs % TILE_SIZE);
	int xx_end = MID(0, hud->ox + 50, SCREEN_W);
	int tx, xx;
	for (;yy < SCREEN_H; ty++, yy += TILE_SIZE) for (xx = xx_start, tx = left_col; xx < xx_end; tx++, xx += TILE_SIZE) {
        	if (ty >= 0 && ty < this->th && tx >= 0 ) blit(
                	tile_map[grid[ty][tx % this->tw]], active_page, 0, 0, xx, yy, TILE_SIZE, TILE_SIZE
        	); else rectfill(active_page,xx,yy,xx+TILE_SIZE,yy+TILE_SIZE,Green);
 	}
	for(list<blood_t *>::iterator bl = blood.begin(); bl != blood.end(); bl++) (*bl)->draw();
	for(list<monst_t *>::iterator mon = m.begin(); mon != m.end(); mon++) (*mon)->draw();
    for(list<killer_particle_t *>::iterator kp = bullets.begin(); kp != bullets.end(); kp++) (*kp)->draw();
    main_jumpgate_draw();
    for(list<jg_particle_t *>::iterator jgp = jg_particle.begin(); jgp != jg_particle.end(); jgp++)	(*jgp)->draw();
	if (weather) weather->draw();
	if (arg_xinfo) {
	    int tyy = 4;
		int tystep = text_height(&font_tiny);
		int txx = 4;
		textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
			"ms_count: %i", ms_count
		);tyy += tystep;
		textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
			"game_turn: %i", game_turn
		);tyy += tystep;
		textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
			"%s", debug_sweetness_points_str
		);tyy += tystep;			    
	}
}

void asteroid_t::encl_draw(int ssl, int sst, int ssr, int ssb, BITMAP *floor_bmp, BITMAP *cage_bmp) {
	int ls = (trader ? trader->ox + trader->kmenu->w - 220 : 0); if (ls < 0) ls = 0;
	int rs = MID(0, hud->ox + 50, ssr);
	camera_x = 0;
	camera_y = 0;
	if (ssl < ls) {
		camera_x = -ssl - ls;
		ssl = ls;
	}
	if (sst < 0) {
		camera_y = -sst;
		sst = 0;
	}
	if (ssb >= SCREEN_H) ssb = SCREEN_H - 1;
	if (ssr > rs) ssr = rs;
	if (ssl > ssr || sst > ssb) return;
	int screen_w = ssr - ssl + 1;
	int screen_h = ssb - sst + 1;
	BITMAP *sav_ap = active_page;
	BITMAP *subb = create_sub_bitmap(active_page, ssl, sst, screen_w, screen_h);
	if (!subb) return;
	active_page = subb;
	if (floor_bmp) {
		int ty = (camera_y / floor_bmp->h);
		int yy = -(camera_y % floor_bmp->h);
		int xs = (camera_x);
		int left_col = (xs / floor_bmp->w);
		int xx_start = -(xs % floor_bmp->w);
		int xx_end = screen_w;
		int tx, xx;
		for (;yy < screen_h; ty++, yy += floor_bmp->h) for (xx = xx_start, tx = left_col; xx < xx_end; tx++, xx += floor_bmp->w) {
        		if (ty >= 0 && ty < this->th && tx >= 0 ) blit(
                		floor_bmp, active_page, 0, 0, xx, yy, floor_bmp->w, floor_bmp->w
        		);
 		}
	}
    for(list<blood_t *>::iterator bl = blood.begin(); bl != blood.end(); bl++) (*bl)->draw();
	for(list<monst_t *>::iterator mon = m.begin(); mon != m.end(); mon++) (*mon)->draw();
    for(list<killer_particle_t *>::iterator kp = bullets.begin(); kp != bullets.end(); kp++) (*kp)->draw();
    for(list<jg_particle_t *>::iterator jgp = jg_particle.begin(); jgp != jg_particle.end(); jgp++)	(*jgp)->draw();
	if (cage_bmp) {
		int ty = (camera_y / cage_bmp->h);
		int yy = -(camera_y % cage_bmp->h);
		int xs = (camera_x);
		int left_col = (xs / cage_bmp->w);
		int xx_start = -(xs % cage_bmp->w);
		int xx_end = screen_w;
		int tx, xx;
		for (;yy < screen_h; ty++, yy += cage_bmp->h) for (xx = xx_start, tx = left_col; xx < xx_end; tx++, xx += cage_bmp->w) {
			if (ty >= 0 && ty < this->th && tx >= 0 ) draw_sprite(
					active_page, cage_bmp, xx, yy
			);
		}
	}
	rect(active_page,0,0,screen_w - 1,screen_h -1,LightGreen);
	active_page = sav_ap;
}





