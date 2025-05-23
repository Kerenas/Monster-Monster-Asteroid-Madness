#include "mmam.h"
#include <stdarg.h>

typedef struct mpos_t {
	float x, y, mnear, mfar;
	int b,t,l,r;
};

typedef class scriptstate_t {
	public:
	char *fn; int line;
	scriptstate_t(char *setfn) { fn = strdup(setfn); line = 0; }
	~scriptstate_t() { free(fn); }
};

static list <mpos_t *> mstack;
static list <scriptstate_t *> sstack;
static asteroid_t *a;
static monst_t *m;
static cage_t *cage;
int script_grid_line, script_level;
int script_fuss_loud = 1;

int script_fuss(char *format, ...) {
	int ret;
	char buff[800], buff2[800];
	va_list args;
	va_start(args, format);
	vsnprintf(buff, sizeof buff, format, args);
	va_end(args);
	printf(buff2,  "mmam-script: (%s %i) %s!\n", sstack.front()->fn, sstack.front()->line, buff);
	if (script_fuss_loud) {
		snprintf(buff2, sizeof buff2, "mmam-script: (%s %i) %s!   Quit?", sstack.front()->fn, sstack.front()->line, buff);
		want_to_quit = tiredofplaying = askyn(buff2);

	}
	return 1;
}

int dump_ast(char *fn, int flags) {
        if (!ast) return 0;
        if (lev_editor) ms_count = lev_edit_time0;
	PACKFILE *pf = (fn ? pack_fopen(fn, (flags & SAVE_COMPRESSED ? "pw" : "w")) : NULL);
	if (pf || fn == NULL) {
                //if (pf) pack_fputs("ast.new\n", pf); else printf("ast.new\n");
                ast->save(pf, flags);

        	if (pf) pack_fclose(pf);
 		printf("asteroid dumped to file\n");
 	} else {
                printf("asteroid dumped to failed!");
 		return 0;
	}
 	return 1;
}

int load_game() {
        char fn[200];
        snprintf(fn, sizeof fn, "%ssav/%s.sav", home_dir, player_name);
	script_load(fn);
}

int save_game() {
        char fn[200];
        int flags = SAVE_EXACT;
        snprintf(fn, sizeof fn, "%ssav/%s.sav", home_dir, player_name);
	PACKFILE *pf = pack_fopen(fn, (flags & SAVE_COMPRESSED ? "pw" : "w"));
	if (pf ) {
                hud->save(pf, flags);
        	if (pf) pack_fclose(pf);
                printf("game saved: %s\n",fn);
 		return 0;
	}
 	return 1;
}


static char *narg(char **str) {
        if (str == NULL || *str == NULL || **str == 0) return NULL;
	char *ret;
	for (;**str == ' ' || **str == '\t' ;(*str)++);
	ret = *str;
	while (**str && **str != ' ' && **str != '\t') {
		(*str)++;
	}
	if (**str) { **str = 0; (*str)++;}
	for (;**str == ' ' || **str == '\t' ;(*str)++);
	return (*ret == 0 ? NULL : ret);
}

static char *nstrarg(char **str) {
        if (str == NULL || *str == NULL || **str == 0) return NULL;
	for (;**str == ' ' || **str == '\t' ;*str++);
	return (**str == 0 ? NULL : *str);
}

void pfwrite(PACKFILE *pf, const char *buff) {
	if (pf) pack_fputs(buff, pf); else printf("%s", buff);
}

void asteroid_t::save(PACKFILE *pf, int flags) {
	// this function is probbally unsafe the buffer _could_ overflow but its rather unlikely
	char buff[8000];
	char *b = buff;
	b += sprintf(b, "###### Asteroid %i (%s) %ix%i 0x%06x ######\n", ast->asteroid_idx, "DUMP", pw, ph, flags);
	if (flags & SAVE_GRID) {
		b += sprintf(b, "ast.asteroid_idx %i\n", asteroid_idx);
		b += sprintf(b, "ast.gridalloc %i %i\n", tw,th);
		pfwrite(pf,buff);
		int hx_len = 0;
		for (int yy = 0; yy < th; yy++) for (int xx = 0; xx < tw; xx++) {
			if (grid[yy][xx] >= ('~' - ' ' - 2) || grid[yy][xx] < 0) {
				int nhl = (grid[yy][xx] < 0x100 ? 2 : grid[yy][xx] < 0x10000 ? 4 : 8);
				if (nhl > hx_len) hx_len = nhl;
			}
		}
		const char *hfs = (hx_len == 2 ? "%02x " : hx_len == 4 ? "%04x " : "%08x ");
		if (hx_len == 0) for (int yy = 0; yy < th; yy++) {
			b = buff + sprintf(buff, "ast.grid ");
			for (int xx = 0; xx < tw; xx++) {
			char ch = ('A' + grid[yy][xx]);
			if (ch > '~') {
				ch = ch - '~' + ' ';
				if (ch >= '#') ch++;   if (ch >= ';') ch++;
			}
			*b = ch;  b++;
			}
			sprintf(b, "\n");
			pfwrite(pf,buff);
		} else for (int yy = 0; yy < th; yy++) {
			b = buff + sprintf(buff, "ast.hgrid ");
			for (int xx = 0; xx < tw; xx++) b += sprintf(b, hfs, grid[yy][xx]);
			b--; sprintf(b, "\n");
			pfwrite(pf,buff);
		}
		b = buff;
	} else b += sprintf(b, "ast.clear\n");
	b += sprintf(b, "   ast.jumpgate %i %i %i\n", jumpgate, jumpgate_x, jumpgate_y);
	b += sprintf(b, "   ast.skin_num %i\n", skin_num);
	if (flags & SAVE_EXACT_ASTEXIT) {
		b += sprintf(b, "   ast.camera %i %i\n", camera_x, camera_y);
		b += sprintf(b, "   ast.exit_time %i\n", exit_time - ms_count);
	}
	// char *style; int building_sign; int artifact_idx, artifact_x, artifact_y, mimmack_mood, orgs_here;
	if (pf) pack_fputs(buff, pf); else printf("%s", buff);
	for(list<blood_t *>::iterator bl = blood.begin(); bl != blood.end(); bl++) {
		blood_t *b = *bl;
		sprintf(buff,"    ast.addblood %i %i %i %i %i %i %i %s\n",
			b->x, b->y, b->vx, b->vy, b->dry_time - ms_count, b->shape, b->pool_size,
			(b->color == &Green ? "Green" : "Red"  )
		);
		pfwrite(pf,buff);
	}
	int aun = 1;
	for(list<monst_t *>::iterator mon = m.begin(); mon != m.end(); mon++) {
   		if (*mon == ship && (*mon)->alive() && ((flags & SAVE_EXACT_ASTEXIT) == 0 || lev_editor)  ) continue;
		(*mon)->aun = ((flags & SAVE_AI_STATE) ? aun++ : 0);
		(*mon)->save(pf, flags);
		pfwrite(pf, (flags & SAVE_SPAWN ? "ast.spawnmon\n" : "ast.addmon\n") );
	}
	if (flags & SAVE_AI_STATE) {
		for(list<monst_t *>::iterator mon = m.begin(); mon != m.end(); mon++) {
			monst_t *m = *mon;
			if (m->see_monst || m->who_hit_me_last) {
				sprintf(buff,
					"ast.linkmonai %i %i %i\n", m->aun,
					(m->see_monst ? m->see_monst->aun : 0),
					(m->who_hit_me_last ? m->who_hit_me_last->aun : 0)
				);
				pfwrite(pf,buff);
			}
		}
	}
	for(list<killer_particle_t *>::iterator kp = bullets.begin(); kp != bullets.end(); kp++) {
		killer_particle_t *k = *kp;
		sprintf(buff, "ast.addbullet %.6f %.6f %.6f %.6f %.6f %i %i %i %i %s\n",
			k->x, k->y, k->vx, k->vy, k->a,
			k->dry_time - ms_count,
			k->str, k->size,
			(k->who_fired ? k->who_fired->aun : 0), k->t->nam
		);
		pfwrite(pf,buff);
	}
	for(list<jg_particle_t *>::iterator jgp = jg_particle.begin(); jgp != jg_particle.end(); jgp++)	{
		sprintf(buff, "ast.addjgpart %.6f %.6f %i\n",
			(*jgp)->x, (*jgp)->y,
			ms_count - (*jgp)->s_time
		);
		pfwrite(pf,buff);
	}
	for(list<monst_t *>::iterator mon = m.begin(); mon != m.end(); mon++) (*mon)->aun = 0;

}


int asteroid_t::parse(char *cmd, char *next, int line) {
	if (0 == strcmp(cmd, "asteroid_idx")) {
		asteroid_idx = atoi(next);
	} else if (0 == strcmp(cmd, "gridalloc") ) {
		for (int c = 0; c < th; c++) free(grid[c]);
		free(grid);
		sscanf(next, "%i %i", &tw, &th);
		grid = (int **) calloc(sizeof (int *), th);
		for (int c = 0; c < th; c++) {
			grid[c] = (int *)calloc(sizeof(int), tw);
		}
		script_grid_line = 0;
	} else if (0 == strcmp(cmd, "hgrid")) {
		script_fuss("hex grid reader not yet implemented"); return 0;
	} else if (0 == strcmp(cmd, "grid")) {
		if (script_grid_line == th) {
			script_fuss("too many asteroid rows"); return 0;
		}
		int c;
		for (c = 0; c < tw; c++) {
			char ch = next[c];
			if (ch == 0) {
				script_fuss("too few asteroid cols"); return 0;
			}
			int zs = '~' - 'A' + 1;
			int decoded = (
					ch >= 'A' ? ch - 'A' :
					ch < '#' ? zs + ch - '!' :
					ch < ';' ? zs + ch - '!' - 1 :
						zs + ch - '!' - 2
			);
			grid[script_grid_line][c] = decoded;
			script_grid_line++;
		}
		if (next[c] != 0) {
			script_fuss("too many asteroid cols"); return 0;
		}
	} else if (0 == strcmp(cmd, "clear")) {
		clear();
	} else if (0 == strcmp(cmd, "populate")) {
                /* ejk depriciated
		int pix = atoi(next); if (!pix) pix = asteroid_idx;
		populate(pix);
		*/
	} else if (0 == strcmp(cmd, "jumpgate")) { sscanf(next, "%i %i %i", &jumpgate, &jumpgate_x, &jumpgate_y);
	} else if (0 == strcmp(cmd, "skin_num")) { sscanf(next, "%i", &skin_num);
	} else if (0 == strcmp(cmd, "camera")) { sscanf(next, "%i %i", &camera_x, &camera_y);
	} else if (0 == strcmp(cmd, "exit_time")) { exit_time = ms_count + atoi(next);
	} else if (0 == strcmp(cmd, "addblood")) {
		/*
		sscanf(next, "%i %i %i %i %i %i %i %s\n",
		b->x, b->y, b->vx, b->vy, b->dry_time - ms_count, b->shape, b->pool_size,
		(b->color == &Green ? "Green": "Red" )
		);
		*/
	} else if (0 == strcmp(cmd, "linkmonai")) {
		monst_t *mm = NULL, *sm = NULL, *hm = NULL;
		int mmaun, smaun, hmaun;
		sscanf(next, "%i %i %i",  &mmaun, &smaun, &hmaun);
		for(list<monst_t *>::iterator mon = m.begin(); mon != m.end(); mon++) {
			if (mmaun && (*mon)->aun == mmaun) mm = (*mon);
			if (smaun && (*mon)->aun == smaun) sm = (*mon);
			if (hmaun && (*mon)->aun == hmaun) hm = (*mon);
		}
		if (mm) {
			mm->see_monst = sm;
			mm->who_hit_me_last = hm;
		}

	} else if (0 == strcmp(cmd, "addbullet")) {
		// "%.6f %.6f %.6f %.6f %.6f %i %i %i %i %s\n",
	} else if (0 == strcmp(cmd, "addjgpart")) {
		// "%.6f %.6f %i\n", (*jgp)->x, (*jgp)->y, ms_count - (*jgp)->s_time
	} else {
		script_fuss("ast command \"%s\" unknown", cmd, line); return 0;
	}
	return 1;
}

void monst_t::save(PACKFILE *pf, int flags) {
	// this function is probbally unsafe the buffer _could_ overflow but its rather unlikely
	char buff[8000];
	char *b = buff;
	b += sprintf(b, "mon.new %s\n", base->name);
	if (aun) b += sprintf(b, "    mon.aun %i\n", aun);
	if (strcmp(base->name, name) != 0) b += sprintf(b, "    mon.name %s\n", name);
	if ((flags & (SAVE_SPAWN_BY | SAVE_SPAWN)) == 0) {
		b += sprintf(b, "    mon.pos %.3f %.3f %.6f %.6f\n", x,y,ang,wang);
		if (fabs(vx) > 0.0001 || fabs(vy) > 0.0001) b += sprintf(b, "    mon.vel %.3f %.3f\n", vx,vy);
		if (fabs(ax) > 0.0001 || fabs(ay) > 0.0001) b += sprintf(b, "    mon.acc %.3f %.3f\n", ax,ay);
	}
	if (flags & SAVE_EXACT_WEAP) {
	       b += sprintf(b, "    mon.clearweaps\n");
	       weap.reverse();
	       for(list<weap_t *>::iterator wp = weap.begin(); wp != weap.end(); wp++) {
	               weap_t *w = *wp;
	               if (w->rl_time > ms_count) {
		              b += sprintf(b, "    mon.addweap_rl %i %i %s\n", w->count, w->rl_time - ms_count, w->t->nam);
	               } else b += sprintf(b, "    mon.addweap %i %s\n", w->count, w->t->nam);
	       }
	       weap.reverse();
	}
	if (flags & SAVE_EXACT_TRAIT) {
		b += sprintf(b, "    mon.hp %i %i\n", hp, mhp);
		b += sprintf(b, "    mon.size %i %.3f\n", size, radius);
		if (flags  != base->flags) b += sprintf(b, "    mon.flags %i\n", flags);
		if (ai_func != base->ai_func) b += sprintf(b, "    mon.ai_func %i\n", ai_func);
		if (sex) b += sprintf(b, "    mon.sex %i\n", sex);
		if (action_idx) b += sprintf(b, "    mon.action_idx %i\n", action_idx);
		// if (vis_count) b += sprintf(b, "    mon.vis_count %i\n", vis_count - ms_count);
		// traveled = 0.; frame_ms
		if (krulasite_inf) b += sprintf(b, "    mon.krulasite_inf %i\n", krulasite_inf - ms_count);
		if (attack_time > ms_count) b += sprintf(b, "    mon.attack_time %i\n", attack_time - ms_count);
		b += sprintf(b, "    mon.speed  %.6f %.6f %.6f\n",
			speed * (float)(TURNS_PER_SEC),
			lat_speed * (float)(TURNS_PER_SEC),
			turn_rate  * (180./M_PI) * (float)(TURNS_PER_SEC)
		);
		if (!lev_editor) b += sprintf(b, "    mon.times %i %i %i\n",
			time_born - ms_count,
			time_mature - ms_count,
			time_die - ms_count
		);
		if (jump_out_time) b += sprintf(b, "    mon.jump_out_time %i\n", jump_out_time - ms_count);
		if (time_captured) b += sprintf(b, "    mon.time_captured %i\n", time_captured - ms_count);
		if (time_thaw > ms_count) b += sprintf(b, "    mon.time_thaw %i\n", time_thaw - ms_count);
		if (dead())  b += sprintf(b, "    mon.decay_time %i\n", decay_time - ms_count);
		if (drugged)  b += sprintf(b, "    mon.drugged %.6f\n", drugged);
		if (speed_bonus) b += sprintf(b, "    mon.speed_bonus %.6f\n", speed_bonus);
		if (energy_bonus) b += sprintf(b, "    mon.energy_bonus %.6f\n", energy_bonus);
		if (str_bonus) b += sprintf(b, "    mon.str_bonus %.6f\n", str_bonus);
		if (met_rate_bonus) b += sprintf(b, "    mon.met_rate_bonus %.6f\n", met_rate_bonus);
		if (healing_bonus) b += sprintf(b, "    mon.healing_bonus %.6f\n", healing_bonus);
		if (agression_bonus) b += sprintf(b, "    mon.agression_bonus %.6f\n", agression_bonus);
		if (invisability_bonus) b += sprintf(b, "    mon.invisability_bonus %.6f\n", invisability_bonus);
		if (growth_bonus) b += sprintf(b, "    mon.growth_bonus %.6f\n", growth_bonus);
		if (restlessness) b += sprintf(b, "    mon.restlessness %.6f\n", restlessness);
		if (flags & SAVE_AI_STATE) b += sprintf(b, "    mon.aistate1 %.0f %.0f %i %i %i\n", run_x, run_y, aiu1, aiu2, aiu3);
		b += sprintf(b, "    mon.intblock1 %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i\n",
			str, attack_rate, stomach, met_rate,
			max_nutrish, max_energy, od_thresh, gestation, agression,
			fear_dist, nutrish, ast_from, action_frame, emote_icon, emote_count, weap_rounds
		);
		b += sprintf(b, "    mon.floatblock1 %.6f %.6f\n",in_stomach, energy);
		if (this == hud->hooked_monst) b += sprintf(b, "    mon.hooked\n");
		if (this == hud->watch_monst) b += sprintf(b, "    mon.watched\n");
                if (this == ship && alive()) b += sprintf(b, "    mon.theship\n");
	}
	// unhandled :
	//    vis_count monst_base **extra_hates monst_base **extra_scared_of;
	//    monst_t *who_hit_me_last, *see_monst;  smacked last_smack
	//
	if (pf) pack_fputs(buff, pf); else printf("%s", buff);
	if (trader_ui) trader_ui->save(pf, flags);
}




int monst_t::parse(char *cmd, char *next, int line) {
	if        (0 == strcmp(cmd, "aun")) { aun = atoi(next);
	} else if (0 == strcmp(cmd, "name")) { rename(next);
	} else if (0 == strcmp(cmd, "pos")) {
		sscanf(next, "%f %f %f %f", &x, &y, &ang, &wang);
	} else if (0 == strcmp(cmd, "hungry")) {
   		in_stomach = (1. - atof(next)/ 100.) * stomach;
	} else if (0 == strcmp(cmd, "hooked")) { hud->hooked_monst = this;
	} else if (0 == strcmp(cmd, "watched")) { hud->watch_monst = this;
	} else if (0 == strcmp(cmd, "theship")) { ship = this; ai_func = ai_player;
	} else if (0 == strcmp(cmd, "vel")) {
		sscanf(next, "%f %f", &vx, &vy);
	} else if (0 == strcmp(cmd, "acc")) {
		sscanf(next, "%f %f", &ax, &ay);
	} else if (0 == strcmp(cmd, "clearweaps")) {
			for (list<weap_t *>::iterator wp = weap.begin(); wp != weap.end(); wp++) delete *wp;
			weap.clear();
	} else if (0 == strcmp(cmd, "addweap_rl")) {
		int count, rl;
		char *cmd;
		if ( (cmd = narg(&next)) ) {
                        count = atoi(cmd);
		        if ( (cmd = narg(&next)) ) rl = atoi(cmd);
                        for (int c = 0; c < NUM_IVALU_LIST; c++) if (strcmp(next,ivalu_list[c].nam) == 0) {
                               give_weap(&ivalu_list[c], count);
                	       return 1;
                        }
                        script_fuss("unreconized item \"%s\"",next);
                }
		return 0;
	} else if (0 == strcmp(cmd, "addweap")) {
		char *cmd;
		if ( (cmd = narg(&next)) ) {
                        int count = atoi(cmd);
                        for (int c = 0; c < NUM_IVALU_LIST; c++) if (strcmp(next,ivalu_list[c].nam) == 0) {
                               give_weap(&ivalu_list[c], count);
                	       return 1;
                        }
                        script_fuss("unreconized item \"%s\"",next);
                }
		return 0;
	} else if (0 == strcmp(cmd, "giveweap")) {
                for (int c = 0; c < NUM_IVALU_LIST; c++) if (strcmp(next,ivalu_list[c].nam) == 0) {
                        give_weap(&ivalu_list[c]);
                	return 1;
                }
		script_fuss("unreconized item \"%s\"",next);
		return 0;
	} else if (0 == strcmp(cmd, "hp")) {
		sscanf(next, "%i %i", &hp, &mhp);
	} else if (0 == strcmp(cmd, "size")) {
		sscanf(next, "%i %f", &size, &radius);
	} else if (0 == strcmp(cmd, "flags")) {
		flags = atoi(next);
	} else if (0 == strcmp(cmd, "sex")) {
		sex = atoi(next);
	} else if (0 == strcmp(cmd, "action_idx")) { action_idx = atoi(next);
	} else if (0 == strcmp(cmd, "krulasite_inf")) { krulasite_inf = atoi(next) + ms_count;
	} else if (0 == strcmp(cmd, "attack_time")) { attack_time = atoi(next) + ms_count;
	} else if (0 == strcmp(cmd, "speed")) {
	/*
	" %f %f %f",
		speed * (float)(TURNS_PER_SEC),
		lat_speed * (float)(TURNS_PER_SEC),
		turn_rate  * (180./M_PI) * (float)(TURNS_PER_SEC)
	);
	*/
	} else if (0 == strcmp(cmd, "times")) {
		sscanf(next, "%i %i %i",  &time_born, &time_mature, &time_die);
		time_born += ms_count;
		time_mature += ms_count;
		time_die += ms_count;
	} else if (0 == strcmp(cmd, "jump_out_time")) { jump_out_time = atoi(next) + ms_count;
	} else if (0 == strcmp(cmd, "time_captured")) { time_captured = atoi(next) + ms_count;
	} else if (0 == strcmp(cmd, "time_thaw")) { time_thaw = atoi(next) + ms_count;
	} else if (0 == strcmp(cmd, "decay_time")) { decay_time = atoi(next) + ms_count;
	} else if (0 == strcmp(cmd, "drugged")) { drugged = atof(next);
	} else if (0 == strcmp(cmd, "speed_bonus")) { speed_bonus = atof(next);
	} else if (0 == strcmp(cmd, "energy_bonus")) { energy_bonus = atof(next);
	} else if (0 == strcmp(cmd, "str_bonus")) { str_bonus = atof(next);
	} else if (0 == strcmp(cmd, "met_rate_bonus")) { met_rate_bonus = atof(next);
	} else if (0 == strcmp(cmd, "healing_bonus")) { healing_bonus = atof(next);
	} else if (0 == strcmp(cmd, "agression_bonus")) { agression_bonus = atof(next);
	} else if (0 == strcmp(cmd, "invisability_bonus")) { invisability_bonus = atof(next);
	} else if (0 == strcmp(cmd, "growth_bonus")) { growth_bonus = atof(next);
	} else if (0 == strcmp(cmd, "restlessness")) { restlessness = atof(next);
	} else if (0 == strcmp(cmd, "ai_func")) { ai_func = (ai_type)atoi(next);
	} else if (0 == strcmp(cmd, "aistate1")) {sscanf(next, "%f %f %i %i %i", &run_x, &run_y, &aiu1, &aiu2, &aiu3);
        } else if (0 == strcmp(cmd, "ast_from")) {
		sscanf(next, "%i", &ast_from);
	} else if (0 == strcmp(cmd, "intblock1")) {
		sscanf(next, "%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i",
			&str, &attack_rate, &stomach, &met_rate,
			&max_nutrish, &max_energy, &od_thresh, &gestation, &agression,
			&fear_dist, &nutrish, &ast_from, &action_frame, &emote_icon, &emote_count, &weap_rounds
		);
	} else if (0 == strcmp(cmd, "floatblock1")) {sscanf(next, "%f %f", &in_stomach, &energy);
	} else {
		script_fuss("unreconized mon command \"%s\"",cmd);
		return 0;
	}
	return 1;
}

static int parse_main(char *cmd, char *next, int line) {
	if (strcmp("include", cmd) == 0) {
		script_level++;
		int ok = script_load(next, a);
		script_level--;
		if (!ok) { script_fuss("failed to load include \"%s\"",next); return 0; }
	} else if (strcmp("poppos", cmd) == 0) {
		if (! (mstack.empty())) { delete mstack.front(); mstack.pop_front();
		} else { script_fuss("can't pop from empty stack"); return 0; }
        } else if (strcmp("duppos", cmd) == 0) {
		if (! (mstack.empty())) {
			mpos_t *pp = new mpos_t;
                	pp->x = mstack.front()->x;
			pp->y = mstack.front()->y;
			mstack.push_front(pp);
		} else { script_fuss("can't dup empty stack"); return 0; }
	} else if (strcmp("movpos", cmd) == 0) {
                if (! (mstack.empty())) {
			float ppx, ppy;
			sscanf(next, "%f %f", &ppx, &ppy);
                	mstack.front()->x += ppx;
			mstack.front()->y += ppy;
		} else { script_fuss("can't movpos on an empty stack"); return 0; }
	} else if (strcmp("pushpos", cmd) == 0) {
		mpos_t *pp = new mpos_t;
		sscanf(next, "%f %f", &pp->x, &pp->y);
		mstack.push_front(pp);
	} else if (strcmp("mon.new", cmd) == 0) {
		int ok = 0;
		for (int c = 0; mb_list_all[c]; c++) if (strcmp(mb_list_all[c]->name, next) == 0) {
			m = new monst_t(mb_list_all[c]);  ok = 1;  break;
		}
		if (!ok) { script_fuss("monster type \"%s\" undefined",next); return 0; }
	} else if (strstr(cmd, "event.") == cmd) {
                cmd += 6;
                if (!event.parse(cmd, next, line)) return 0;
	} else if (strstr(cmd, "mon.") == cmd) {
		cmd += 4;
		if (m) {
			if (strcmp("pushpos", cmd) == 0) {
				mpos_t *pp = new mpos_t;
				pp->x = m->x;
				pp->y = m->y;
				mstack.push_front(pp);
			} else if (!m->parse(cmd, next, line)) return 0;
		} else { script_fuss("mon undefined"); return 0; }
	} else if (strcmp("ast.new", cmd) == 0) {
		a = new asteroid_t(atoi(next));
	} else if (strcmp(cmd, "ast.setast") == 0) {
                ast = a;
	} else if (strcmp(cmd, "ast.hook") == 0) {
		if (strcmp(next, "home") == 0) {
			a = home_ast;
		} else if (strcmp(next, "remote") == 0) {
			a = remote_ast;
		}  else { script_fuss("invalid ast.hook"); return 0; }
        } else if (strstr(cmd, "hud.") == cmd) {
		cmd += 4;
                if (!hud->parse(cmd, next, line)) return 0;
        } else if (strstr(cmd, "cage.setcage") == cmd) {
                cage = hud->jail[MID(0, atoi(next), 11)];
        } else if (strstr(cmd, "cage.") == cmd) {
		cmd += 5;
		if (cage) {
                	if (strstr(cmd, "addmon") == cmd) {
        			cage->m = m;
			} else if (!cage->parse(cmd, next, line)) return 0;
		} else { script_fuss("cage undefined"); return 0; }
	} else if (strstr(cmd, "ast.") == cmd) {
		cmd += 4;
		if (a) {
			if (strcmp(cmd,"spawnmon") == 0) {
				a->spawn_monst(m);
			} else if (strcmp(cmd,"addmon") == 0) {
				a->add_monst(m);
			} else if (strcmp(cmd,"addmonr") == 0) {
				if (!mstack.empty()) {
					mpos_t *pp = mstack.front();
					m->x += pp->x;
					m->y += pp->y;
					a->add_monst(m);
				} else { script_fuss("can't addmonr with empty stack"); return 0; }
			} else if (strcmp(cmd,"addmonby") == 0) {
				if (!mstack.empty()) {
					mpos_t *pp = mstack.front();
					a->add_monst_by(m, pp->x, pp->y);
				} else { script_fuss("can't addmonby with empty stack"); return 0; }
			} else	if (!a->parse(cmd, next, line)) return 0;
		} else { script_fuss("ast undefined"); return 0; }
	} else if (cmd[0]) {
		script_fuss("command \"%s\" unreconized",cmd);
		return 0;
	}
	return 1;
}

void event_t::save(PACKFILE *pf, int flags) {
	// this function is probbally unsafe the buffer _could_ overflow but its rather unlikely
	char buff[8000];
	char *b = buff;
	b += sprintf(b, "    event.clear\n");
	for (int i = 0; i < event_last; i++) b += sprintf(b, "    event.set %i %i %i %i\n", i,
		event_list[i].enabled, event_list[i].time,
		event_list[i].severity
	);
	if (pf) pack_fputs(buff, pf); else printf("%s", buff);
}

int  event_t::parse(char *cmd, char *next, int line) {
	if (0 == strcmp(cmd, "clear")) { clear();
	} else if (0 == strcmp(cmd, "set")) {
        	int idx = MID(0, atoi(next), event_last - 1);
        	int junk;
                sscanf(next, "%i %i %i %i",
			&junk, &event_list[idx].enabled, &event_list[idx].time,
			&event_list[idx].severity
		);
        } else {
        	script_fuss("event command \"%s\" unreconized",cmd);
		return 0;
        }
	return 1;
}


int script_load(char *fn, asteroid_t *script_load_ast) {
	if (!fn || !fn[0]) return 0;
	int good_script = 0;
	char buff[4000], buff2[4000];
	char *b;
	char sfn[300];
	PACKFILE *pf;
	a = script_load_ast;
	for (int c = 0; c < 5; c++) {
		switch (c) {
		case 0: snprintf(sfn, sizeof sfn, "%ssav/%s",home_dir,fn);   break;
		case 1:
		     if (arg_nobones || make_nobones) continue;
		     snprintf(sfn, sizeof sfn, "%sbones/%s",home_dir,fn);   break;
		case 2: snprintf(sfn, sizeof sfn, "%scustom/%s",home_dir,fn);   break;
		case 3: snprintf(sfn, sizeof sfn, "%scustom/%s",data_dir,fn);   break;
		case 4:
		   {
       			char *ch = sfn;
      			ch += snprintf(ch, sizeof sfn, "%slevels.dat#",data_dir);
      			for (char *t = fn; *t; t++, ch++) {
           			*ch = (
           				*t >= 'a' && *t <= 'z' ? *t - 'a' + 'A' :
           				*t == '.' ? '_' : *t
   				);
                        }
      			*ch = 0;
          	   }
		   break;
		}
		if ( (pf = pack_fopen(sfn,"pr")) ) break;
		if ( (pf = pack_fopen(sfn,"r")) ) break;
	}
	if (pf) {
  		sstack.push_front(new scriptstate_t(sfn));
		while ((b = pack_fgets(buff2, sizeof(buff2), pf))) {
			(sstack.front())->line++;
			while (*b == ' ' || *b == '\t') b++;
			int for_count = 1;
			char *rem = strstr(b,"#"); if (rem) { *rem = 0;}
			if (!b[0]) continue;
			char *eol = b + strlen(b);
			if (eol) { *eol = ';'; eol++; *eol = 0; }
			if (b[0] == '*') {
				char *next = b;
				char *cmd;
				if ( (cmd = narg(&next)) ) {
      					cmd++;	for_count = atoi(cmd);
                        	}
                        	strncpy(buff, next, sizeof buff);
     			} else strncpy(buff, buff2, sizeof buff);
     			for (int c = for_count; c; c--) {
          			b = buff2;
          			strncpy(buff2, buff, sizeof buff);
				char *eoi;
				while ( (eoi = strstr(b,";")) ) {
					*eoi = 0;
					char *next = b;
					char *cmd;
					if ( (cmd = narg(&next)) ) {
						if  (!parse_main(cmd, next, (sstack.front())->line)) goto f_this;
					}
					b = eoi + 1;
				}
                     	}
		}
		good_script = 1;
f_this:
		delete sstack.front(); sstack.pop_front();
		pack_fclose(pf);
	}
	return good_script;
}
