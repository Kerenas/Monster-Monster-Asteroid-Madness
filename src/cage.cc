#include "mmam.h"
#include "rand.h"
#include "bubbles.h"

cage_t::cage_t(int x, int y, int idx, ivalu_t *t) {
	this->x = x;
	this->y = y;
	cage_num = idx;
	upgrade(t);
	unrattle_time = rattle_x = rattle_y = 0; rattle_tilt = 0.;
  	m = NULL;
}

void cage_t::upgrade( ivalu_t *t) {
 	if (t) {
		lvl = t->lvl;
		hp = mhp = t->sv;
		spr_ok = t->ws1;
		spr_broken = t->ws2;
		hp_rf = (t->cost / 2.) / hp;
		has_cryo = (lvl == 5);
		has_regen = (lvl == 4);
 	} else lvl = hp = mhp = -1;
}

void cage_t::pack(monst_t *monst) {
	m = monst;
	if (m) {
                if (m->my_ast) m->my_ast->unhook_monst(m);
                m->forget_all();
   		m->my_ast = NULL;                           
                m->time_captured = ms_count;
   		if (has_cryo) cryo_time = ms_count + 15000;
	}
}

monst_t *cage_t::dump(int onto_ast) {
	if (!m) return NULL;
	monst_t *ret = m;
	if (m->dead()) m->decay_time = ms_count + 20000;
	if (has_cryo) m->cryo_advance(cryo_time);
       	if (onto_ast) {
		monst_t *into = ast->monst_at(ship->x, ship->y, ship, 0);
          	if (into && into->encl) {
             		into->encl->add_monst_by( m,
             			ship->x - into->x + (into->encl->pw / 2),
             			ship->y - into->y + (into->encl->ph / 2)
             			// (into->encl->pw / 2), (into->encl->ph / 2)

     			);
            	} else ast->add_monst_by(m, ship->x, ship->y);
        }
       	m = NULL;
        return ret;
}

int  cage_t::hit(int h) {
	hp -= h;
	unrattle_time = ms_count + 250;
	rattle_x = irand(7) - 3;
	rattle_y = irand(4);
	rattle_tilt = (frand() * (M_PI / 2.) - (M_PI / 4.));
	if (hp <= 0) {
        	hp = 0;
        	if (m) ast->add_monst_by(m, ship->x, ship->y);
        	m = NULL;
        	SHIP_NOISE(SMASHCAGE);
        	return 1;
   	} else {
        	SHIP_NOISE(RATTLECAGE);
      	}
   	return 0;
}

int  cage_t::poll() {
  	if (m && !frozen()) m->caged_poll(this);
	if (unrattle_time < ms_count) {  rattle_x = rattle_y = 0;  rattle_tilt = 0.;  }
	return 1;
}


void cage_t::draw() {
	BITMAP *cage_spr = (hp >= 0 ? (BITMAP *)(sprites[(hp == 0 ? spr_broken : spr_ok)].dat) : NULL);
	if (cage_spr) {
  		if (
  			mouse_x >= hud->ox + x &&
	        	mouse_x <= cage_spr->w + hud->ox + x -1 &&
  			mouse_y >= y &&
  			mouse_y <= cage_spr->h + y -1
	  	) {
        		input_map.monster_hover = cage_num;
		}
   		rectfill(active_page, hud->ox + x, y +6 , cage_spr->w + hud->ox + x -1, cage_spr->h + y +6 -1, Black);
   		BITMAP *monst_spr = (m == NULL ? NULL : ( m->base->spr_big[0] < 0 ? NULL : (BITMAP *)(sprites[m->base->spr_big[0]].dat)));
   		int dx = hud->ox + x + rattle_x + cage_spr->w / 2;
   		int dy = y +6 + rattle_y + cage_spr->w / 2;
   		float rt = (hp < 200 ? rattle_tilt : rattle_tilt / (hp / 200));
                if (monst_spr) sprite_rsw(
   			active_page, monst_spr, dx, dy,
                	rt, cage_spr->w
        	);
                if (has_cryo) {
                   int clr = (!m ? Green :  ms_count > cryo_time ? LightGreen : (ms_count / 800) & 1 ? LightRed : Red);
                   BITMAP *csc = create_bitmap(cage_spr->w, cage_spr->h);
                   blit(cage_spr, csc, 0, 0, 0, 0, csc->w, csc->h);
                   rectfill(csc, csc->w - 10, 2,  csc->w - 8, 4, clr);
                   rect(csc, csc->w - 11, 1,  csc->w - 7, 5, Black);
                   sprite_rsw(
                	active_page, csc, dx, dy,
                	rt, cage_spr->w
        	   );
        	   destroy_bitmap(csc);
                } else {
                   sprite_rsw(
                	active_page, cage_spr, dx, dy,
                	rt, cage_spr->w
        	   );
        	}
        	BITMAP *e_bmp = (m ? m->emote_bitmap() : NULL);
                if (e_bmp) sprite_rsw(
   			active_page, e_bmp, hud->ox + x + cage_spr->w-2, y - 4,
                	0., 3 * e_bmp->w / 4
        	);
        	if (hud->cage_info_upgrade) bar_draw_h(active_page, (BITMAP *)sprites[SPR_BAR_CAGE].dat,
                	hud->ox + x +1,
                	y+38, 100 * hp / (mhp ? mhp : 1)
        	);


	}
}


void cage_t::save(PACKFILE *pf, int flags) {
	char buff[8000];
	sprintf(buff, "cage.statblock1 %i %.3f %i %i %i %i %i\n",
		lvl, rattle_tilt, rattle_x, rattle_y, unrattle_time,
		hp,  cryo_time
	);
        if (m) {
        	m->save(pf, flags);
        	pfwrite(pf, "cage.addmon\n");
        }
}

int  cage_t::parse(char *cmd, char *next, int line) {
	if (0 == strcmp(cmd, "statblock1")) {
		upgrade(&ivalu_list[FIRST_INV_CAGE_IDX + atoi(next)]);
		sscanf(next, "%i %f %i %i %i %i %i %i",
			&lvl, &rattle_tilt, &rattle_x, &rattle_y, &unrattle_time,
			&hp,  &cryo_time
		);
		return 1;
	}
	return 0;
}

