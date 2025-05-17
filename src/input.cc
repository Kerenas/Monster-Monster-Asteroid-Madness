#include "mmam.h"
#include "gamelog.h"


int strincmp(char *s1, char *s2, int n) {
	while (--n >= 0) {
		int d = (isupper(*s1) ? tolower(*s1) : *s1) - (isupper(*s2) ? tolower(*s2) : *s2);
		if (d != 0 || *s1 == '\0' || *s2 == '\0') return d;
		++s1; ++s2;
	}
	return 0;
}


input_map_t input_map;
int sound_options_dlg(DIALOG *d);
int askyn(char *s);


static int kb_input_lock = 0;

static struct {char *str; int e;} ecl[] = {
	{(char *)"pitfight", event_pit_fight}, {(char *)"merchant"  , event_merchant},
	{(char *)"tourists", event_tourists},  {(char *)"golgathan",  event_golgithan},
	{(char *)"raiders",  event_raiders},   {(char *)"stalkerclan",  event_stalker_clan},
	{(char *)"hunters", event_hunters},    {(char *)"meteorstorm",  event_meteor_storm},
	{(char *)"police",  event_police},
	{NULL,  0}
};

static void calcs() {
	PACKFILE *pf = pack_fopen("calcs.txt",  "w");
	if (pf ) {
               	char buff[8000];
                for (int c = 0; mb_list_all[c]; c++) {
	            monst_base *mb = mb_list_all[c];
                    char *b = buff;
	            // BFC_Price b += sprintf(b, "%s : bfc: %0.3f\n", mb->name, ((float)(mb->nutrish) / MAX(1, sqrt(mb->size)) * sqrt(mb->nutrish / 2000.)));
                    b += sprintf(b, "%s : bfc: %0.3f\n", mb->name, ((float)(mb->mhp / 2. + 
                    mb->str * 10000. / MAX(1, mb->attack_rate) + mb->fame / 10. + sqrt(mb->max_energy) +
                    (mb->speed) + (mb->turn_rate / 10.) + (mb->lat_speed / 3.))));
                    pack_fputs(buff, pf);
                }
        	pack_fclose(pf);
	}
}

static  int test_script(const char *filename, int attrib, void *param) {
	char *f = (char *)filename;
	for (char *ff = f; *ff; ff++) {
		if (*ff == '/') f = ff + 1;
	}
	if (f[0] == '_') return 0;	
	printf((char *)"try to load: %s\n", filename);
	asteroid_t *ta = new asteroid_t(-1);
	if (script_load(f, ta)) {
		printf((char *)"script load: %s (%i mon)",f,ta->m.size());
	} else {
		printf((char *)"could not load: %s",f);
	}
	delete ta;
	return 0;
}

static int do_cmd(char *cmd, int valid_ui) {
    // may mess with cmd in memory (but sets back to orig state)
	char *cp = strstr(cmd," ");
	if (!valid_ui) {
		input_map.target_x = irand(400);
		input_map.target_y = irand(400);
	}
	if (cp) {
		*cp = 0; cp++;
		int arg = atoi(cp);
		if (stricmp(cmd, "upg") ==0 ) {
			if (arg >0 && arg < NUM_IVALU_LIST) hud->upgrade(&ivalu_list[arg]);
			else for (int c = 0; c < NUM_IVALU_LIST; c++) {
				if (strincmp(ivalu_list[c].nam, cp, strlen(cp)) == 0) {
					hud->upgrade(&ivalu_list[c]);
					break;
				}
			}
		} else if (stricmp(cmd, "event") ==0 ) {
			event.set(arg,2);
			for (int c = 0; ecl[c].str; c++) if (strincmp(ecl[c].str, cp, strlen(cp)) == 0) {
				event.set(ecl[c].e,2);
				break;
			}
		} else if (stricmp(cmd, "karma") ==0 ) {
			hud->karma = arg;
		} else if (stricmp(cmd, "ast") ==0 ) {
			delete remote_ast;
			remote_ast = new asteroid_t(arg);
		} else if (stricmp(cmd, "dump") ==0 ) {
			dump_ast(cp,dump_ast_flags);
		} else if (stricmp(cmd, "spawn") ==0 ) {
			if (arg > 0) {
				ast->spawn_monst(mb_list_all[arg]);
			} else for (int c = 0; mb_list_all[c]; c++) {
				if (strncmp(mb_list_all[c]->name, cp, strlen(cp)) == 0) {
					ast->add_monst_by(new monst_t(mb_list_all[c]), input_map.target_x, input_map.target_y);
					break;
				}
			}
		} else if (stricmp(cmd, "pickleslap") ==0 ) {
			for (int c = 0; c < org_last; c++) {
					if (strncmp(org_list[c].nam, cp, strlen(cp)) == 0) {
					alignment.offend(c,50000);
					break;
				}
			}
		} else if (strcmp(cmd, "flock") ==0 ) {
			if (arg > 0) {
				for (int i = 0; i < 5; i++) ast->spawn_monst(mb_list_all[arg]);
			} else for (int c = 0; mb_list_all[c]; c++) {
				if (strincmp(mb_list_all[c]->name, cp, strlen(cp)) == 0) {
					for (int i = 0; i < 5; i++) ast->add_monst_by(new monst_t(mb_list_all[c]), input_map.target_x, input_map.target_y);
					break;
				}
			}
		}
	} else if (stricmp(cmd, "wtf") ==0 ) {
		printf((char*)"ast calcs dump event flock (no-)fps karma lakeside (no-)levedit");
		printf((char*)"money pickleslap (no-)sackless spawn strapped testscr");
		printf((char*)"upg wimpy wtf (no-)xinfo zoo"); 
    } else if (stricmp(cmd, "dump") ==0 ) {
		char fn[200];
		snprintf(fn, sizeof fn, "ast-dump/ast%03i.mmam", ast->asteroid_idx);
		dump_ast(fn, dump_ast_flags);
		snprintf(fn, sizeof fn, "ast-dump/ast%03i.cmm", ast->asteroid_idx);
		dump_ast(fn, dump_ast_flags | SAVE_COMPRESSED);
	} else if (stricmp(cmd, "testscr") ==0 ) {
		char sfn[300];   
		snprintf(sfn, sizeof sfn, "%scustom/*.mmam",home_dir);
		for_each_file_ex(sfn,0,FA_DIREC, test_script, NULL);
		snprintf(sfn, sizeof sfn, "%scustom/*.mmam",data_dir);
		for_each_file_ex(sfn,0,FA_DIREC, test_script, NULL);
	} else if (stricmp(cmd, "money") == 0 ) {
		hud->money = 9999999;
	} else if (stricmp(cmd, "strapped") == 0 ) {
		for (int c = 0; c < NUM_IVALU_LIST; c++) {
			if (ivalu_list[c].typ == inv_proj) hud->upgrade(&ivalu_list[c]);
		}
	} else if (stricmp(cmd, "wimpy") == 0 ) {
		for (int c = 0; c < NUM_IVALU_LIST; c++) {
			hud->upgrade(&ivalu_list[c]);
		}
	} else if (stricmp(cmd, "zoo") == 0 ) {
		int drp = 0;
		for (int c = 0; mb_list_all[c]; c++) {
			if (drp) ast->spawn_monst(mb_list_all[c]);
			if (mb_list_all[c] == &mb_netship) drp = 1;
		}
 	} else if (stricmp(cmd, "lakeside") ==0 ) {
		monst_base *strip_mall[] = {
			&mb_jmart, &mb_bfc, &mb_wisehut, &mb_arena,
			&mb_trainer, &mb_policestation,
   			&mb_fisec, &mb_orickcamp, &mb_merchant, NULL
   		};
   		for (int c = 0; strip_mall[c]; c++) {
			ast->add_monst_by(new monst_t(strip_mall[c]), input_map.target_x, input_map.target_y);
		}
	} else if (stricmp(cmd, "calcs") == 0 ) {
		calcs();
	} else if (stricmp(cmd, "sackless") == 0 ) {
		sackless_mode = 1;
	} else if (stricmp(cmd, "levedit") == 0 ) {
		lev_editor = 1; lev_edit_time0 = ms_count;
	} else if (stricmp(cmd,"xinfo") == 0) {
		arg_xinfo =  1;
	} else if (stricmp(cmd, "fps") == 0) {
		arg_fps =  1;
	} else if (stricmp(cmd, "no-sackless") == 0 ) {
		sackless_mode = 0;
	} else if (stricmp(cmd, "no-levedit") == 0 ) {
		lev_editor = 0;
	} else if (stricmp(cmd,"no-xinfo") == 0) {
		arg_xinfo = 0;
	} else if (stricmp(cmd, "no-fps") == 0) {
		arg_fps = 0;
	}
	if (cp) { cp--; *cp = ' '; }
	return (cmd[0] == 0 ? 0 : 1);
}

void run_cmd_list(char *cmd) {
	if (!cmd) return;
	char *p = strdup(cmd);
	char *pb = p, *pe = NULL;
	int done = 0;
	while (!done) {
		while (*pb == ' ' || *pb == ';' || *pb == '\t' || *pb == '\n' || *pb == '\r') pb++;
		for (pe = pb; 1; pe++ ) {
			if (*pe == 0 || *pe == ';' || *pe == '\n' || *pe == '\r') {
				done = (*pe == 0);    *pe = 0;   do_cmd(pb, 0);   pb = pe + 1;  break;
			}					
		}
	}
	free(p);
}

static BITMAP *b_cross = NULL;

void safty_pause() {
	if (ms_count > input_map.last_input_time + 12000) {
		input_map.paused = 2; // safty pause 
	}
}	

// code cannot make assume that any pointers to game objects are valid, because it is
// now also used for pregame gui stuff.
void check_kb_input() {
	//if (kb_input_lock) printf("kb_input_lock %i\n", kb_input_lock);
	if (((kb_input_lock & 1) == 0) && (key[KEY_ESC] || want_to_quit)) {
		kb_input_lock |= 1;
		tiredofplaying = !ast ? 1 : askyn((char*)"are you shure you want to quit?");
		kb_input_lock &=~ 1;
		if (tiredofplaying) return;
		want_to_quit = 0;
	}
	if (((kb_input_lock & 4) == 0) && key[KEY_MINUS_PAD]) {
		sound_options_dlg(NULL);
	}
	input_map.monster_but = input_map.menu_but = -1;
	int lvy = input_map.vy;
	if (!free_edit->visible) {
   		if (key[KEY_TILDE] && arg_cheater) { free_edit->visible = 1; clear_keybuf(); }
		if (key[KEY_0]) input_map.menu_but = 0;       if (key[KEY_1]) input_map.menu_but = 1;
		if (key[KEY_2]) input_map.menu_but = 2;       if (key[KEY_3]) input_map.menu_but = 3;
		if (key[KEY_4]) input_map.menu_but = 4;       if (key[KEY_5]) input_map.menu_but = 5;
		if (key[KEY_6]) input_map.menu_but = 6;       if (key[KEY_7]) input_map.menu_but = 7;
		if (key[KEY_8]) input_map.menu_but = 8;       if (key[KEY_9]) input_map.menu_but = 9;
		if (input_map.last_menu >= 0 && input_map.menu_but >= 0) {
			input_map.menu_but = -1;
		} else {
			input_map.last_menu = input_map.menu_but;
		}
		input_map.quick_weap_btn = input_map.menu_but;
		if (key[KEY_F1])  input_map.monster_but = 0;  if (key[KEY_F2])  input_map.monster_but = 1;
		if (key[KEY_F3])  input_map.monster_but = 2;  if (key[KEY_F4])  input_map.monster_but = 3;
		if (key[KEY_F5])  input_map.monster_but = 4;  if (key[KEY_F6])  input_map.monster_but = 5;
		if (key[KEY_F7])  input_map.monster_but = 6;  if (key[KEY_F8])  input_map.monster_but = 7;
		if (key[KEY_F9])  input_map.monster_but = 8;  if (key[KEY_F10]) input_map.monster_but = 9;
		if (key[KEY_F11]) input_map.monster_but = 10; if (key[KEY_F12]) input_map.monster_but = 11;
		if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) {
			if (input_map.monster_but > -1 && hud->jail[input_map.monster_but]->vacent()) {
				hud->next_cage = input_map.monster_but;
			}
			input_map.monster_but = -1;
		}
       	input_map.menu_exit_but = (key[KEY_TAB]);
		input_map.vt = (key[KEY_A] || key[KEY_LEFT] ? -100 : 0) + (key[KEY_D] || key[KEY_RIGHT] ? 100 : 0);
		input_map.vy = (key[KEY_W] || key[KEY_UP] ? 100 : 0)    + (key[KEY_S] || key[KEY_DOWN] ? -100 : 0);
		input_map.vx = (key[KEY_Q] ? 100 : 0)    + (key[KEY_E] ? -100 : 0);
		input_map.fire[3] = (key[KEY_SPACE]);
		static int my_p_is_sticky = 0;
		static int my_v_is_sticky = 0;
		static int my_r_is_sticky = 0;
		if (key[KEY_P] && !my_p_is_sticky) { 
			input_map.paused = input_map.paused ? 0 : 1;
			my_p_is_sticky = 1;
		} else if (!key[KEY_P]) my_p_is_sticky = 0;
		input_map.vehicle_btn = 0;			
		if (key[KEY_V] && !my_v_is_sticky) { 
			input_map.vehicle_btn = 1;
			my_v_is_sticky = 1;
		} else if (!key[KEY_V]) my_v_is_sticky = 0;
		if (key[KEY_R] && !my_r_is_sticky) { 
			input_map.repair_btn = input_map.repair_btn ? 0 : 1;
			my_r_is_sticky = 1;
		} else if (!key[KEY_R]) my_r_is_sticky = 0;
		
 	}
	int sf1 = input_map.fire[0];
	int sf2 = input_map.fire[1];
	input_map.fire[0] = mouse_b & 0x1;
	input_map.fire[1] = mouse_b & 0x2;
	input_map.fire[2] = mouse_b & 0x4;
	if (input_map.click_ok > ms_count) {
		input_map.fire[0] = 0;
		input_map.fire[1] = 0;
		input_map.fire[2] = 0;
   	}
	//input_map.clicked = (sf1 && !input_map.fire[0]);
	if (input_map.menu_hover >= 0 && (sf1 && !input_map.fire[0])) {
		if (input_map.menu_hover == HUD_MENU_EXIT)  input_map.menu_exit_but = 1;
		else input_map.menu_but = input_map.menu_hover;
		// printf("input_map.menu_hover = %i(0x%x)\n",input_map.menu_hover,input_map.menu_hover);
  	}
	if (input_map.menu_hover >= 0 && (sf2 && !input_map.fire[1])) {
		if (input_map.menu_hover == HUD_MENU_EXIT)  input_map.menu_exit_but = 1;
		else input_map.menu_but = input_map.menu_hover;
  	}
	if (input_map.monster_hover >= 0 && (sf1 && !input_map.fire[0])) {
          	input_map.monster_but = input_map.monster_hover;
  	}
	if (input_map.monster_hover >= 0 && (sf2 && !input_map.fire[1]) && hud) {
   		if (hud->jail[input_map.monster_hover]->vacent()) {
			hud->next_cage = input_map.monster_hover;
		}
  	}
	int mz = mouse_z;
	input_map.sel = (
		trader && lvy != 0 && input_map.vy == 0 ? (lvy > 0 ? -1 : 1) :
		key[KEY_PGUP] ? -1 : key[KEY_PGDN] ? 1 :
		input_map.old_mouse_z - mz
	);
	input_map.old_mouse_z = mz;
	input_map.target_valid = 0;
	int e = 0, cpyidx = 0, build_w = 0;
	if (ast) {
		weap_t * w = ship->armed();
		e = (w && w->t->typ != inv_proj ? SPR_CROSSHAIRS_BUILD : SPR_CROSSHAIRS);
		input_map.target_x = mouse_x + ast->camera_x;
		input_map.target_y = mouse_y + ast->camera_y;
		monst_t *look_monst = ast->monst_at(input_map.target_x, input_map.target_y);
		if (look_monst) {
			if (
				look_monst->trader_ui &&
					(look_monst->alive() || look_monst == mainbase) &&
					(alignment.align(look_monst) >= -1 || look_monst->base == &mb_policestation)
			) e = SPR_CROSSHAIRS_INTERFACE;
		}
		if (e ==  SPR_CROSSHAIRS_BUILD) {
			float r = sqrt(w->t->mb_link->size);
			cpyidx = w->t->mb_link->spr_chill[0].idx;
			build_w = (int)(sqrt(w->t->mb_link->size));
			input_map.target_valid = ( ast->monst_near(input_map.target_x, input_map.target_y, r) ? 0 : cpyidx);
			if ( trader || mouse_x > hud->ox ) { input_map.target_valid = e = 0; }
		} else {
			input_map.target_valid = ( mouse_x < hud->ox && !trader ? e : 0);
			e = 0;
		}
 	}
 	static int ltv = 0;
	if (e ==  SPR_CROSSHAIRS_BUILD) {
   		if (input_map.target_valid != ltv) {
			if (b_cross) destroy_bitmap(b_cross);
			b_cross = create_bitmap(build_w,build_w);
			BITMAP *cpy = (BITMAP *)sprites[cpyidx].dat;
			stretch_blit(cpy,b_cross,0,0,cpy->w,cpy->h,0,0,build_w,build_w);
			int c0 = (input_map.target_valid ? Green : Red );
			int c1 = (input_map.target_valid ? LightGreen : LightRed );
			int pink = bitmap_mask_color(b_cross);
			for (int y = 0; y < build_w; y++) for (int x = 0; x < build_w; x++) {
				putpixel(b_cross,x,y,
					(getpixel(b_cross,x,y) != pink ? c1 :
							(((y & 1) + x) & 1) ? c0 : pink
							)
				);
			}
			rect(b_cross,0,0,build_w-1,build_w-1,c1);
			set_mouse_sprite(b_cross);
			set_mouse_sprite_focus(build_w/2,build_w/2);
			ltv =  input_map.target_valid;
    	}
	} else if (input_map.target_valid && (input_map.target_valid != ltv)) {
		BITMAP *ptr = (BITMAP *)sprites[input_map.target_valid].dat;
		set_mouse_sprite(ptr);
		set_mouse_sprite_focus(ptr->w/2,ptr->h/2);
		ltv = input_map.target_valid;
	} else if (!input_map.target_valid && ltv) {
		set_mouse_sprite((BITMAP *)sprites[SPR_POINTER].dat);
		set_mouse_sprite_focus(0,0);
		ltv = input_map.target_valid;
	}
	if (free_edit && !free_edit->visible && input_map.target_valid) {
		if (key[KEY_T] && hud)	hud->track_monst = ast->monst_at(input_map.target_x, input_map.target_y);
	}
	
	if (input_map.vx ||input_map.vy || input_map.vt || 
		input_map.vx ||input_map.vy || input_map.vt || 
		
		input_map.old_mouse_x != mouse_x ||
		input_map.old_mouse_y != mouse_y ||
		input_map.fire[0] || input_map.fire[1] || input_map.fire[2] || 
		(hud && hud->claw_line_out > 0.)
   		
	) {
		input_map.last_input_time = ms_count;
	}
	
	input_map.old_mouse_x = mouse_x;
	input_map.old_mouse_y = mouse_y;
	if (free_edit && free_edit->poll()) { 
		int stay_open = do_cmd(input_map.chat_buffer, 1);   
		free_edit->clear(); 
		free_edit->visible = stay_open;
	}
	if (gamelog && free_edit) gamelog->visible = free_edit->visible;
}



