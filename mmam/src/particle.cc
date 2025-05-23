#include "mmam.h"
#include "bubbles.h"

weather_t::weather_t(asteroid_t *aa, int wx, int ws) {
    my_ast = aa;
	init_rain(); weather_idx = wx;  severity = 5; wind_x = wind_y = 0.;
    rain_color = LightBlue;
}


void weather_t::init_rain() {
	for (int i = 0; i < NUM_RAINDROPS; i++) {
		rain[i].x = rand() % (my_ast->pw * 2) - my_ast->pw / 2;
		rain[i].y = - rand() % (my_ast->ph / 2);
	}
}

void weather_t::step_rain() {
	int rain_vx = -2;
	for (int i = 0; i < NUM_RAINDROPS; i++) {
		rain[i].x += rain_vx;
		if ((rain[i].y += 5) > my_ast->ph -  i) {
			rain[i].x = rand() % (my_ast->pw * 2) - my_ast->pw / 2;
			rain[i].y = - rand() % (my_ast->ph / 4);
		}
	}
}

void weather_t::poll() {
	if (weather_idx == wth_rain) step_rain();
}


void weather_t::draw() {
	if (weather_idx == wth_rain) for (int i = 0; i < NUM_RAINDROPS; i++) {
   		int xx = (int)(rain[i].x - my_ast->camera_x);
		int yy = (int)(rain[i].y - my_ast->camera_y);
		putpixel(active_page, xx,   yy, rain_color);
		putpixel(active_page, xx+1, yy, rain_color);
		putpixel(active_page, xx,   yy+1, rain_color);
		putpixel(active_page, xx+1, yy+1, rain_color);
	}
}

//=============== jg particles ==================

jg_particle_t::jg_particle_t(asteroid_t *aa, float sx, float sy, int styp) {
    my_ast = aa;
  	x = sx; y = sy; s_time = ms_count;
  	a = 0.; size = 1; typ = styp;
}

void jg_particle_t::poll() {}

void jg_particle_t::draw() {
	size = (int)(1 + JG_MAX_SIZE - abs( ((s_time + JG_MID_TIME) - ms_count) * JG_MAX_SIZE / JG_MID_TIME ));
	a = (s_time - ms_count) * JG_RAD_PER_MS;
	int xx = (int)(x - my_ast->camera_x);
	int yy = (int)(y - my_ast->camera_y);
       	if (size > 0) sprite_rsw(active_page, (BITMAP *)sprites[typ].dat, xx, yy, a, (int)(size), 200);
}

//============== Killer particles =================

killer_particle_t::killer_particle_t(asteroid_t *smy_ast, monst_t *swho_fired, float sx, float sy, float sa, ivalu_t *w) {
	t = w; x = sx; y = sy; a = sa;
	if (t->wtyp & WP_SKATR) a += frand(M_PI/20.) - (M_PI / 40.);
    if (t->wtyp & WP_FLASH) hud->flash_time = ms_count + 1200;
    if (t->wtyp & WP_QUAKE) hud->shake_time = ms_count + 500;
	float sv = w->vel / TURNS_PER_SEC;
	vx = sv * cos(a);   vy = sv * sin(a);
	pic = (BITMAP *)sprites[w->projs].dat;
	size = pic->w * 2 / 3;
	my_ast = smy_ast;   who_fired = swho_fired;
	if (who_fired) {
          vx += who_fired->vx; vy += who_fired->vy;
    }
	dry_time = (ms_count + (int)(t->wtyp & WP_SPORE ? 18000. + irand(2200) : 1000. * w->rng / w->vel) );
	mask.mask = NULL;
   	mask.src = NULL;
	mark_pmask(&mask, pic, a - (M_PI / 2.), size);
}

void killer_particle_t::spore_check() {
	if (irand(30) == 0) { // 60 spores in an explosion
		monst_base *grow_monst = (
			t == &IDF_POISONSPORE ? &mb_poisonshroom :
			t == &IDF_PSYCHOSPORE ? &mb_psychoshroom :
			t == &IDF_PYROSPORE ? &mb_pyroshroom : NULL
		);
		if (grow_monst) {
			monst_t *nm = new monst_t(grow_monst, 0);
			my_ast->add_monst(nm, x, y);
		}
	}
}
void killer_particle_t::spore_freeze() {
       	dry_time = 6000;
}

void killer_particle_t::poll() {
	if (!t) return;
	if (t->wtyp & WP_SPORE) {
		int tleft = dry_time - ms_count;
		if (tleft > 12000) {
				vx *= 0.94; vx += (frand()-0.5);
				vy *= 0.94; vy += (frand()-0.5);
		} else {
			vx = vy = 0.;
			if (tleft <= 6000) return;
		}
	}
	this->x += this->vx;
	this->y += this->vy;
	if (this->x < 0 || this->x > my_ast->pw || this->y < 0  || this->y > my_ast->pw) dry_time = 0;
	if (ms_count < dry_time) {
			monst_t *bump = (1 ? my_ast->monst_at(&mask, x, y, who_fired, 0) :
				my_ast->monst_at(x, y, who_fired, 0)
			);
			if ( bump && ((bump->flags & AF_NOSHOOT) == 0) ) {
				int pickup_only =  (
						( (t == &IDF_PSYCHOSPORE) || (t == &IDF_POISONSPORE) ) &&
									!bump->can_shroom_poison() &&  !bump->isa_shroom()
				);
				if ((t->wtyp & WP_SPORE) &&  bump->can_pickup_spores()) {
						int tleft = dry_time - ms_count;
						if (tleft <= 12000 || pickup_only) {
							if (t == &IDF_PYROSPORE)  bump->spore_pyro_count++;
							else if (t == &IDF_PSYCHOSPORE) bump->spore_psycho_count++;
							else if (t == &IDF_POISONSPORE) bump->spore_poison_count++;
									dry_time = 0;
							printf("spore attached to %s\n",bump->name);
							t = NULL; // prevent from growing
							return;
						}
				}
				if (bump->base == &mb_crysillian) {
						my_ast->add_bullet(bump, atan2(-vy,-vx), t);
				}                                                            // radius inten
				if (t->wtyp & WP_EXPL && t->drg && my_ast) my_ast->explode(x,y,t->drg, t->sv, who_fired, bump);
				int str = t->sv;
				if (bump->od_thresh && t->wtyp & WP_SLEEP) {
					if ( bump->can_poison()) bump->drugged += t->tox / (bump->base == &mb_hoveler ? 5 : 1);
				if ( bump->can_sleep()) bump->met_rate_bonus -= 1000. * t->drg / bump->od_thresh / (bump->base == &mb_hoveler ? 5 : 1);
				}
				if (t->wtyp & WP_PSY) {
					if (bump->base == &mb_yazzert || bump->base == &mb_vaerix) return;
					bump->energy -= MIN(t->drg, bump->energy);
				}
				if (t->wtyp & WP_CHEM ) {
					bump->flags |= AF_BESERK;
					bump->drugged += 100;
				}
				if ((t->wtyp & WP_FIRE) && bump->flammable()) {
					bump->on_fire = ms_count + ON_FIRE_TIME;
				}
				bump->hit(str, who_fired);
				dry_time = 0;
				printf((char *)"bullet hit %s\n",bump->name);
				if (t->wtyp & WP_SPORE) t = NULL; // prevent from growing
		}
	}
}

void killer_particle_t::draw() {
    if (!t) return;
	int xx = (int)(x - my_ast->camera_x);
	int yy = (int)(y - my_ast->camera_y);
    if (t->wtyp & WP_SPIN) a = (4. * M_PI) * (dry_time - ms_count) / 1000.;
	int alpha = (
		t->wtyp & WP_SOLID ? 255 :
		t->wtyp & WP_SPORE ? MID(60,(int)((dry_time - ms_count) / 9000.),255) : 220
	);
	if (size > 0) sprite_rsw(active_page, pic, xx, yy, a - M_PI / 2., (int)(size), alpha);
  	mark_pmask(&mask, pic, a - (M_PI / 2.), size);
}

