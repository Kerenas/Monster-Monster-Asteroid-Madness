#include "config.h"
#include "mmam.h"
#ifndef _WIN32
#  include <pwd.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#endif
#include "qfe.h"
#include "dsound.h"
#include "mmam_ai.h"
#include "fonts.h"
#ifdef USE_DUMB
#  include <dumb.h>
#  define MIDISUBST(mod,midi) mod
#  define IFDUMB(args...) args
#else
#  define MIDISUBST(mod,midi) midi
#  define IFDUMB(args...)
#endif
#ifdef USE_NET
#  define IFNET(args...) args
#else
#  define IFNET(args...)
#endif
#define NO_MIDI
#define SETUP_EMBEDDED
#define SETUP_TITLE        "MMAM Allegro Setup"
#ifdef _WIN32
#  define SETUP_CFG_FILE     "mmam.cfg"
#else
static char cfgname[200];
#  define SETUP_CFG_FILE     cfgname
#endif
static int in_setup_c = 0;
#include "setup_c.h"
#ifndef DATA_DIR
#   define DATA_DIR "./"
#endif
#include "graphs.h"

extern BITMAP *buffer;
int qfe_state;

int ai_test, ai_wait_timing,  enable_sound, enable_music, fast_spinner, chat_linger,
    sound_vol, music_vol, music_buffer_size,  music_freq,
    default_screen_w, default_screen_h, prefer_windowed,
    enable_page_flip[2], skip_intro, sackless_mode, lev_editor, lev_edit_time0, make_nobones,  autoshot_freq_sec,
    show_fps;

float gamma_lvl = 1.0;

char *arg_cfg_file, *arg_command_list, *command_list;

int arg_net_mode, arg_ai_test, arg_def_port,
	arg_skip_intro, arg_quick, arg_cheater, arg_nosound,
	arg_nobones, arg_mute_add, arg_fps, arg_buyany, arg_money, arg_xinfo;

char player_name[30], music_dir[200], data_dir[200], scr_shot_dir[200], home_dir[200] = "./";

int dump_ast_flags = SAVE_EXACT;

int instructions(DIALOG *d);

int ok_adime(DIALOG *d, int n) {
	return -1;
}

static void my_adime_callback(DIALOG *d) {
	check_sound();
	check_screen_shot();
}

void set_badmode() {
	char bmf[200];
	snprintf(bmf, sizeof bmf, (char *)"%sbadmode.txt",home_dir);
	PACKFILE *pf = pack_fopen(bmf, (char *)"w");
	if (pf) {
		pack_fputs((char *) "Error in last attempt to set graphic mode!\n",pf);
		pack_fclose(pf);
	}

}

void unset_badmode() {
	char bmf[200];
	snprintf(bmf, sizeof bmf, (char *)"%sbadmode.txt",home_dir);
	delete_file(bmf);
}

static int check_badmode() {
	char bmf[200];
	snprintf(bmf, sizeof bmf, (char *)"%sbadmode.txt",home_dir);
	return exists(bmf);
}

#ifdef _WIN32
#    define mkdir(path, junk) CreateDirectory(path, NULL)
#endif



void getstartupvals(char **argv) {
	#ifdef USE_ADIME
    adime_callback = my_adime_callback;
    #endif
    int wants_setup = 0;
#ifndef _WIN32
    passwd *pwp; 
    if ( ( pwp = getpwuid(getuid()) ) ) {
       snprintf(home_dir, sizeof home_dir, (char *)"%s/.mmam/", pwp->pw_dir);
    }
    snprintf(cfgname, sizeof cfgname, (char *)"%smmam.cfg", home_dir);
#endif
    char xd[300];
    mkdir(home_dir, 0755);
    snprintf(xd, sizeof xd, (char *)"%sbones/", home_dir); mkdir(xd, 0755);
    snprintf(xd, sizeof xd, (char *)"%scustom/", home_dir); mkdir(xd, 0755);
    snprintf(xd, sizeof xd, (char *)"%ssav/", home_dir); mkdir(xd, 0755);
    snprintf(xd, sizeof xd, (char *)"%sstat/", home_dir); mkdir(xd, 0755);
    snprintf(xd, sizeof xd, (char *)"%sstat/auto/", home_dir); mkdir(xd, 0755);
    snprintf(xd, sizeof xd, (char *)"%sinfo/", home_dir); mkdir(xd, 0755);
    snprintf(xd, sizeof xd, (char *)"%sinfo/img/", home_dir); mkdir(xd, 0755);
    if (argv) {
		char **arg = argv;
		while (*arg) {
			if (strcmp(*arg,"--cfg-file") == 0) {
				arg++;
				arg_cfg_file = strdup(*arg);
			} else if (strcmp(*arg,"--cmd") == 0) {
				arg++;
				arg_command_list = strdup(*arg);
				arg_cheater = 1;
			} else if (strcmp(*arg,"--setup") == 0) {
				wants_setup = 1;
			} else if (strcmp(*arg,"--skip-intro") == 0) {
				arg_skip_intro = 1;
			} else if (strcmp(*arg,"--quick") == 0) {
				arg_skip_intro = arg_quick = 1;
			} else if (strcmp(*arg,"--cheater") == 0) {
				arg_cheater = 1;
			} else if (strcmp(*arg,"--lev-editor") == 0) {
				lev_editor = arg_cheater = 1;
			} else if (strcmp(*arg,"--no-sound") == 0) {
				arg_nosound = 1;
			} else if (strcmp(*arg,"--no-bones") == 0) {
				arg_nobones = 1;
			} else if (strcmp(*arg,"--buy-any") == 0) {
				arg_buyany = 1;
			} else if (strcmp(*arg,"--money") == 0) {
				arg_money = 1;
			} else if (strcmp(*arg,"--xinfo") == 0) {
				arg_xinfo = 1;
			} else if (strcmp(*arg,"--mute-add") == 0) {
				arg_nobones = 1;
			} else if (strcmp(*arg,"--fps") == 0) {
				arg_fps = 1;
			} else {
				if (strcmp(*arg,"--help") != 0) printf((char *) "unreconised arg: %s\n\n",*arg);
				allegro_message(
					"mmam [options]\n"
					"   --cfg-file fn          sets alternate config file.\n"
					"   --help                 show this screen.\n"
					"   --skip-intro           skip the intro.\n"
					"   --quick                skip the intro and start a new game.\n"
					"   --no-sound             avoid sound init.\n"
					"   --no-bones             don't use bones files.\n"
					"   --mute-add             don't tell about monsters added to asteroids.\n"
					"   --lev-editor           level editor mode.\n"
					"   --fps                  show frames per second.\n"
					"   --cheater              allow cheat mode.\n"
					"   --buy-any              lets you buy anything from your base.\n"
					"   --cmd 'commandlist'    runs commandlist after game starts.\n"
					"   --xinfo                all sorts of debug info.\n"
					"   --setup                runs the advanced configure mode.\n\n"
				);
				exit(0);
			}
			arg++;
		}
    }
    if (check_badmode()) wants_setup = 1;
    set_config_file((arg_cfg_file ? arg_cfg_file : SETUP_CFG_FILE));
    strncpy(player_name, get_config_string((char *) "main", (char *)"player_name",
    #ifdef _WIN32
		"Asteroid Mike"
	#else
		pwp->pw_name
	#endif
	), sizeof player_name);
    ai_test  = get_config_int((char *) "main", (char *)"ai_test", 0);
    make_nobones = get_config_int((char *) "main", (char *)"make_nobones", 0);
    chat_linger =  get_config_int((char *) "main", (char *)"chat_linger", 18000);
    strncpy(data_dir, get_config_string((char *) "main", (char *)"data_dir", DATA_DIR), sizeof data_dir);
    strncpy(scr_shot_dir, get_config_string((char *) "main", (char *)"scr_shot_dir", home_dir), sizeof scr_shot_dir);
    strncpy(music_dir, get_config_string((char *) "main", (char *)"music_dir", home_dir), sizeof music_dir);
    strncpy(songtab[GM_DEFAULT], get_config_string((char *) "main","default_song", MIDISUBST((char *) "main.it","wasted.mid")), sizeof songtab[0]);
    strncpy(songtab[GM_TITLE], get_config_string((char *) "main","title_song", MIDISUBST((char *) "main.it","wasted.mid")), sizeof songtab[0]);
    strncpy(songtab[GM_CREDITS], get_config_string((char *) "main","credits_song", MIDISUBST((char *) "wasted.it","wasted.mid")), sizeof songtab[0]);
    strncpy(songtab[GM_WIN], get_config_string((char *) "main","win_song", MIDISUBST((char *) "win.it","wasted.mid")), sizeof songtab[0]);
    strncpy(songtab[GM_LOSE], get_config_string((char *) "main","lose_song", MIDISUBST((char *) "lose.it","wasted.mid")), sizeof songtab[0]);
#ifdef USE_DUMB
    dumb_resampling_quality = get_config_int((char *) "main", (char *)"music_resampling_quality", DUMB_RQ_CUBIC );
    music_freq = get_config_int((char *) "main", (char *)"music_freq",  44100);
    music_buffer_size = get_config_int((char *) "main", (char *)"music_buffer_size",  1024 * 6);
#endif
    enable_sound  = get_config_int((char *) "main", (char *)"enable_sound", 1);
    enable_music  = get_config_int((char *) "main", (char *)"enable_music", 1);
    sound_vol  = MID(0, get_config_int((char *) "main", (char *)"sound_vol", 127), 255);
    music_vol  = MID(0, get_config_int((char *) "main", (char *)"music_vol", 60), 255);
    default_screen_w = get_config_int((char *) "main", (char *)"screen_w", 800);
    default_screen_h = get_config_int((char *) "main", (char *)"screen_h", 600);
    prefer_windowed = get_config_int((char *) "main", (char *)"windowed", 0);
    gamma_lvl = get_config_float((char *) "main", (char *)"gamma", 1.0);
    show_fps = get_config_int((char *) "main", (char *)"show_fps", 0);
    enable_page_flip[0] = get_config_int((char *) "main", (char *)"enable_page_flip", 0);
    enable_page_flip[1] = get_config_int((char *) "main", (char *)"enable_page_flip_windowed", 0);
    skip_intro = get_config_int((char *) "main", (char *)"skip_intro", 0);
    if (wants_setup || key[KEY_F2]) {
			in_setup_c = 1;
			set_gfx_mode(GFX_SAFE, 640, 480, 0, 0);
			setup_main();
			in_setup_c = 0;
    }
    autoshot_freq_sec = get_config_int((char *) "main", (char *)"autoshot_freq_sec", 60);
    command_list = strdup( get_config_string((char *) "main", (char *)"cmd", ""));
    printf((char *) "got startup values\n");
}


int sound_options_dlg(DIALOG *d) {
   int graphics_mode_index = (default_screen_w == 800 ?
     ( default_screen_h == 600 ? 1 : 3 ) : 
     default_screen_w == 1024 ? 2 : 0
   );
   int old_gmx = graphics_mode_index;
   int old_windowed = prefer_windowed;
#ifdef USE_DUMB
   int mfreq_list[] = {8000,11025,22050,44100};
   int freq_index = (
       music_freq >= mfreq_list[3] ? 3  :
       music_freq >= mfreq_list[2] ? 2  :
       music_freq >= mfreq_list[1] ? 1  : 0
   );
   int old_fi  = freq_index;
   int old_mbs = music_buffer_size;
#endif
#ifndef USE_ADIME
	allegro_message("Edit mmam.cfg to change options\n");
#else
   adime_dialogf(
                    "Video/Sound Options",
                    ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 160,
                    "Graphics mode:%list[,640x480;800x600;1024x768;800x480]"
                    "Windowed:%bool[]"
                    "Autoshot Sec (0 = disable):%pint[0,999999]"
                    "%line[]"
                    "SFX Volume [0-255]:%pint[0,255]"
                    "Allow SFX:%bool[]"
                    "Music Volume [0-255]:%pint[0,255]"
                    "Allow Music:%bool[]"
            IFDUMB(
                    "%line[]"
                    "Advanced Sound:%nothing[]"
                    "Resample Quality: %list[,Poor;Normal;High]"
                    "Music Freq: %list[,8000;11025;22050;44100]"
                    "Music Buffer (bytes): %pint[1000,1000000]"
            )
                    ,
                    &graphics_mode_index,
                    &prefer_windowed,
                    &autoshot_freq_sec,
                    &sound_vol,
                    &enable_sound,
                    &music_vol,
                    &enable_music
            IFDUMB(
                    ,
                    &dumb_resampling_quality,
                    &freq_index,
                    &music_buffer_size
            )
   );
#endif

   if (old_gmx != graphics_mode_index ||  old_windowed != prefer_windowed) {
   	switch (graphics_mode_index) {
        case 0: default_screen_w =  640; default_screen_h = 480; break;
        case 1: default_screen_w =  800; default_screen_h = 600; break;
        case 2: default_screen_w = 1024; default_screen_h = 768; break;
        case 3: default_screen_w =  800; default_screen_h = 480; break;
        }

	if ( !in_setup_c ) reinit_graphics();
	set_config_int((char *) "main", (char *)"screen_w", default_screen_w );
	set_config_int((char *) "main", (char *)"screen_h", default_screen_h );
	set_config_int((char *) "main", (char *)"windowed", ( prefer_windowed ? 1 : 0));
   }
#ifdef USE_DUMB
   set_config_int((char *) "main", (char *)"music_resampling_quality", dumb_resampling_quality);
   if (freq_index != old_fi || music_buffer_size != old_mbs) {
        music_freq = mfreq_list[freq_index];
        set_config_int((char *) "main", (char *)"music_freq", music_freq );
        set_config_int((char *) "main", (char *)"music_buffer_size", music_buffer_size );
        reset_music();
   }
#endif
   set_config_int((char *) "main", (char *)"enable_sound", ( enable_sound ? 1 : 0) );
   set_config_int((char *) "main", (char *)"enable_music", ( enable_music ? 1 : 0) );
   set_config_int((char *) "main", (char *)"sound_vol", sound_vol );
   set_config_int((char *) "main", (char *)"music_vol", music_vol );
   set_config_int((char *) "main", (char *)"autoshot_freq_sec", MAX(0, autoshot_freq_sec)  );
   
   flush_config_file();
   #ifdef USE_ADIME
   return (in_setup_c ? D_REDRAW : ADIME_REDRAW);
   #else
   return D_REDRAW;
   #endif
}





static int new_initgamedlg(DIALOG *d) {
   /*
   qdlg((char *) "Game Setup",
        ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 160,
        "Video/Sound:%button[Configure...]"
        ,
        &sound_options_dlg
   );
   */
   set_config_string((char *) "main", (char *)"data_dir",data_dir);
   set_config_string((char *) "main", (char *)"music_dir", music_dir);
   set_config_int((char *) "main", (char *)"enable_page_flip", enable_page_flip[0]);
   set_config_int((char *) "main", (char *)"enable_page_flip_windowed", enable_page_flip[1]);
   set_config_float((char *) "main", (char *)"gamma", gamma_lvl);
   set_config_int((char *) "main", (char *)"show_fps", show_fps);
   qfe_state = QFE_NEW_GAME;
   return -1;
}

static int load_initgamedlg(DIALOG *d) {
   /*
   qdlg((char *) "Game Setup",
        ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 160,
        "Video/Sound:%button[Configure...]"
        ,
        &sound_options_dlg
   );
   */
   set_config_string((char *) "main", (char *)"data_dir",data_dir);
   set_config_string((char *) "main", (char *)"music_dir", music_dir);
   set_config_int((char *) "main", (char *)"enable_page_flip", enable_page_flip[0]);
   set_config_int((char *) "main", (char *)"enable_page_flip_windowed", enable_page_flip[1]);
   qfe_state = QFE_LOAD_GAME;
   return -1;
}

static int quit_dlg(DIALOG *d) {
   want_to_quit = 1;
   qfe_state = QFE_QUIT;
   return -1;
}

void mmam_intro();

static int replay_intro(DIALOG *d) {
   scare_mouse();
   BITMAP *ss = create_bitmap(SCREEN_W, SCREEN_H);
   blit(screen,ss,0,0,0,0,SCREEN_W, SCREEN_H);
   mmam_intro();
   blit(ss,screen,0,0,0,0,SCREEN_W, SCREEN_H);
   destroy_bitmap(ss);
   unscare_mouse();
   return -1;
}

static int stats_dlg(DIALOG *d) {
   scare_mouse();
   BITMAP *ss = create_bitmap(SCREEN_W, SCREEN_H);
   blit(screen,ss,0,0,0,0,SCREEN_W, SCREEN_H);
   graph_set *gg = new graph_set();
   gg->load_last();
   gg->set_draw_zone(40,40,SCREEN_W - 80, SCREEN_H - 80);
   active_page = buffer;
   blit(mmam_splash, active_page,0,0,0,0,mmam_splash->w,mmam_splash->h);    		
   while (gg->handle_input(2)) {
        // run graph in double buffered mode  
		if (forced_redraw_flag) {
		textout_centre_ex(active_page, &font_bold, (char *)"Graphs from last game.", SCREEN_W / 2, 0, White, -1);
		}
		gg->draw(active_page);
		show_mouse(buffer);
	    acquire_bitmap(screen);
		blit(buffer,screen,0,  0,  0,   0, SCREEN_W, SCREEN_H);
		release_bitmap(screen);
		show_mouse(screen);
		if (forced_redraw_flag) forced_redraw_flag--;
        check_screen_shot();
 	    check_sound();
  	    rest(0);        
   }
   delete gg;
   scare_mouse();
   blit(ss,screen,0,0,0,0,SCREEN_W, SCREEN_H);
   destroy_bitmap(ss);
   unscare_mouse();
   return -1;
}

#ifndef USE_ADIME	

static int draw_menu_but( int x, int y, char *s, int ret, int sel, int stack_dir) {
	int xx = x;
	int yy = y;
	int ex = xx + 180; // MID(20,text_length(&font_bold_plus, s)+20, 180);
	
	int ey = yy + 20;
	if (
		mouse_x >= xx && mouse_x <= ex &&
		mouse_y >= yy && mouse_y <= ey
	) {
		rectfill(active_page,xx,yy,ex,ey,LightGreen);
		textout_ex( active_page,&font_bold, s,	xx+10, yy, Black, -1 );
		input_map.menu_hover = ret;
	} else {
		rect(active_page,xx,yy,ex,ey,LightGreen);
		rectfill(active_page,xx+1,yy+1,ex-1,ey-1,sel ? Green : Black);
		textout_ex( active_page,&font_bold, s,	xx+10, yy, LightGreen, -1 );
	}
	return (stack_dir ? ey + 6 : ex + 6 );
}

int initgamedlg() {
    for (qfe_state = 0; qfe_state == 0;) {
		active_page = buffer; // run the menu in double buffered mode.
		blit(mmam_splash, active_page, 0,0,0,0, mmam_splash->w, mmam_splash->h);
		int xx = (active_page->w - 180) / 2;
		int yy = (active_page->h - (26 * 5)) / 2;
		// yy = draw_menu_but(xx, yy, (char *)"Continue Game", 1, 0, 1);
		yy = draw_menu_but(xx, yy, (char *)"Start New Game", 2, 0, 1);
		yy = draw_menu_but(xx, yy, (char *)"Instructions", 3, 0, 1);
		yy = draw_menu_but(xx, yy, (char *)"Replay Intro", 4, 0, 1);
		// yy = draw_menu_but(xx, yy, (char *)"Options", 5, 0, 1);
		yy = draw_menu_but(xx, yy, (char *)"Stats", 6, 0, 1);
		yy = draw_menu_but(xx, yy, (char *)"QUIT!", 7, 0, 1);
		show_mouse(buffer);
		acquire_bitmap(screen);
		blit(buffer,screen,0,  0,  0,   0, SCREEN_W, SCREEN_H);
		release_bitmap(screen);
		show_mouse(screen);
		check_kb_input();
		switch (input_map.menu_but) {
		case 1: load_initgamedlg(NULL); break;
		case 2: new_initgamedlg(NULL); break;
		case 3:	instructions(NULL); break;
		case 4:	replay_intro(NULL); break;
		case 5:	sound_options_dlg(NULL); break;
		case 6:	stats_dlg(NULL); break;
		case 7:	quit_dlg(NULL); break;
		}
		if (tiredofplaying) qfe_state = QFE_QUIT;
	}
	return qfe_state;
}


#else
int initgamedlg() {

    for (qfe_state = 0; qfe_state == 0;) {
		while (keypressed()) readkey();
		int junk = adime_lowlevel_dialogf(
			"Main",
			ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 200,
			"%button[New Game]"
			"%button[Load Game]"
			"%button[Instructions]"
			"%button[Replay Intro]"
			"%button[Options]"
			"%button[Stats]"
			"%button[QUIT!]"
			,
			&new_initgamedlg,
			&load_initgamedlg,
			&instructions,
			&replay_intro,
			&sound_options_dlg,
			&stats_dlg,
			&quit_dlg
		);
		// allegro_message("ret %i %i", junk, qfe_state);
		if (junk == 0) qfe_state = QFE_QUIT;
	}
	return qfe_state;
}
#endif

#undef MIDISUBST

/*
static void initplayerdlg(int pidx) {
   int ps = player[pidx].ai_var; // values should be set from the cfg file.
   char ttl[50];
   snprintf(ttl,sizeof(ttl),"Player %i Setup", pidx);
   char playername[20], playerai[20];
   snprintf(playername, sizeof playername, (char *)"player%i_name", pidx);
   snprintf(playerai, sizeof playerai, (char *)"player%i_ai", pidx);
   qdlg(ttl,
                 ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 200,
                 "Name:%string[26]"
                 "Player Style:%vlist[]"
                 ,
                 player[pidx].name,
                 &ps,
                 ai_name_list,
                 NUM_AI_NAMES
   );
   set_config_string((char *) "main", playername, player[pidx].name);
   set_config_int((char *) "main", playerai, ps);
   player[pidx].ai_func = ai_list[ps].ai_func;
   player[pidx].ai_var  = ai_list[ps].ai_var;
}
*/

int qfe_gamesetup() {
   int ret = QFE_NEW_GAME;
   set_mood(GM_TITLE);
   if (!arg_quick) ret = initgamedlg();
   flush_config_file();
   return ret;
}

