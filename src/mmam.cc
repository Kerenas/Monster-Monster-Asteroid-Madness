#include "config.h"
#include "mmam.h"
#include "gamelog.h"
#include "qfe.h"
#include "rand.h"
#include "dsound.h"
#include "fonts.h"
#include "sprites.h"
#include "bubbles.h"
#include "gfx.h"
#include "graphs.h"
#include <jpgalleg.h>
#ifdef USE_PNG
#  include <loadpng.h>
#endif
/*
TODO:

(size - rsize)/ mature(in sec)= Growth_per_sec

rsize + mature(in secs) * growth_per_sec = size

 NosaJimiki:  and pit fights should be like tournements that take a certain amount of time
 NosaJimiki:  or you can only enter one moster into a devision per certain amount of time
 NosaJimiki:  and there would be three weightclasses for your three different general sizes of monster
 MissingDialTone:  I was definately thinking of some sort of weight class system.
 NosaJimiki:  so you can have your roachy goin up against a firbit
 NosaJimiki:  but not an uber worm
 MissingDialTone:  fame bonus for monsters who win pit fights.

also stalkers don't like to leave their dead lyeing arround so they grind them into "Vojo Dust".


NosaJimiki: Krakins are supposed to go hide in thier holes when not active

NosaJimiki: oh I was wondering if for the portriats we could do actual voiceovers and you could code a lip sinker for the portait
NosaJimiki: ok, We can do the Old Guy and the Orick, ther're suposed to have grabled voices I'd think
Rngulley: i think realistically we would have to puppet the voices after they get recorded.
NosaJimiki: maybe the cop and pyou mean manual sinking?
Rngulley: not really i'll make a keyboard interhace to open and close the mouths.
NosaJimiki: so I have some time to maybe try to find a voice team
NosaJimiki: or rather so I know how long to tell ppl I have before the tool is fuinctional
Rngulley: do you want to go ahead and try to tget together most of the stuff from the hints.cc into ogg files.


  NosaJimiki:  I should call it the Malibit
 NosaJimiki:  you can only get them by injecting a furbit with both growth hormones and project17
NosaJimiki:  they'll be really small like a fubit but though like a drexler, so they'll murder other smaller monsters in compition


ok new things:

>sentries: equiped like hunters but a lil more durrable.  They emidiately come to the deffence of
public buildings (and traveling merchants?).

>sentinals: supper sentries, they have heavy firepower and 4xHP of a raider.  Basic millitary
foot soildgers.  Deffend police stations like sentries, also ussed as backbone of police garrisons
and assalts. (squadships will be ussed purely for patrols [randomly come by park if you on on good
terms with cops and fly around in cities], enforcers will only be ussed in assalts.)


>Poisenshrooms: poisens any mosnter that passes by except for mubarks.  When a mubark eats a 
poisenshroom, the Mubark is infected and speads the shroom... the less hungry an infected mubark, 
the more it produces more shrooms.  Cant be poisened or burned by pyroshrooms.(They can live in simbiosis)

>Psycoshrooms: cause confusion to passing monsters.  They spawn on the remains of  monsters that 
have be augemented by or are high on drugs, only if your park has or has had a carrier 
monster(squeeker, meeker, or sneeker) in it.  They sell well.  Cheeches love them


 MissingDialTone:  you got any ideas for how the heigh level patr of pitfights should work is?
 MissingDialTone:  the low level part will involve me making a temporary asteroid and populating it
  with the combatants.
 MissingDialTone:  but the main question is what is allowed to be a combatant.  do we do wagering, 
 interance fees, prizes?
 MissingDialTone:  to the death, ko?
 MissingDialTone:  time limit?
 MissingDialTone:  what kinds of weight clesses?
 NosaJimiki:  I think we need to flag every monster in a wiegh class
 MissingDialTone:  some things shouldn't be allowed to fight. or no weapons 
 rules or somthing like that
 NosaJimiki:  and at the beginning of every tournement randomly generate 31 
 other monsters (stats only).
 NosaJimiki:  no weapons, some monsters(plants/quivers) inadmissable 
 NosaJimiki:  randomly selllect one to fight in 1st round, if win, awward 
 bronze medal and cash priize.  give option to advance or pull moster
 NosaJimiki:  when a monster is KOed the is a chance that it will die
 MissingDialTone:  i think ko should happen at 15% health
 NosaJimiki:  after first round autocalc remaining monster to dertermen "the best"
 NosaJimiki:  wait only 15 random monster generate
 MissingDialTone:  ok
 NosaJimiki:  remaining 8 go ont to silver match, 4 two gold, 2 to Mightiest in the Galixy
 MissingDialTone:  i think the mightiest in the galizy should require a qualifying tournament.
 NosaJimiki:  so you think it must 1st win an 8 monster 3 round tournement?
 MissingDialTone:  yes
 NosaJimiki:  when you win a gold you get ast cordinates to the Central Stadium
 NosaJimiki:  to compeat in TMITG tournement
 MissingDialTone:  Yeah that whould be sweet.
 NosaJimiki:  they will also host standard tournements
 NosaJimiki:  I can make a really cool supper huge stadium
 yeah i'd probbally multiply the stats by 1.25 or somthing
 
 MissingDialTone:  m->awards[0] would be how many TMIG awards it has
 :all
Bob
Joe
Max
Timmy
Eric
Jason
Brian
"Big" Nate
Boris
Sean
Rocket

:Furbit Rumble Light-weight
Fuzzy
Harry
 

*/

DATAFILE *sprites;
BITMAP *buffer;
BITMAP *page1, *page2;
BITMAP *active_page;

static int top = 20;

int  want_to_quit, tiredofplaying;

volatile int ms_count = 0, fps = 0, frame_count = 0, page_flip = 0, forced_redraw_flag = 0, game_logic = 0;
int game_turn;

int do_scr_shot;
int gamedone;

BITMAP *mmam_splash, *mmam_play_arrow;



char *top_string;

asteroid_t *ast, *home_ast, *remote_ast;
trader_t *trader, *free_trader;
hud_t *hud;
monst_t *ship, *jumpgate_s, *mainbase, *thehuman;


static void endprog(char *s) { set_gfx_mode(GFX_TEXT, 0,0,0,0);  puts(s);  exit(0);}


//============================================================================
void outstr(char *s) {
	free(top_string);
	top_string = strdup(s);
	forced_redraw_flag = MAKE_DIRTY;
}



#ifdef USE_PNG
#   define SS_EXT "png"
#   define SS_SAVE BITMAP *tbmp = create_bitmap_ex(screen->w, screen->h, 24); if (tbmp) { blit(screen,tbmp,0,0,0,0,screen->w,screen->h); save_png(gws, tbmp, NULL); destroy_bitmap(tbmp); }
#else
#   define SS_EXT "jpg"
#   define SS_SAVE save_jpg_ex(gws, screen, NULL, 90, JPG_SAMPLING_444, NULL)
#endif


void check_screen_shot() {
	if (key[KEY_INSERT]) do_scr_shot = 1;
	if (!key[KEY_INSERT] && do_scr_shot == 1) {
		char gws[256];int i = 1;
        do {
			snprintf(gws, sizeof gws, "%smmam%03i." SS_EXT, scr_shot_dir, i);
			if (i++ > 999) break;
		} while (exists(gws));
		SS_SAVE;
		if (graphs) graphs->notestr_add('S', gws);
		do_scr_shot = 0;
	}
}

void do_auto_screen_shot() {
	if (!graphs) return;
	char gws[256];int i = 1;
	do {
		snprintf(gws, sizeof gws, "%sautoshot%03i." SS_EXT, scr_shot_dir, i);
		if (i++ > 999) break;
	} while (exists(gws));
	// SS_SAVE;
	graphs->notestr_add('A', gws);
}

static void flash() {
	if (hud) if (hud->flash_time > ms_count) {
		int fi = MID(0, 200 * (hud->flash_time - ms_count ) / 1000, 200);
		drawing_mode(DRAW_MODE_TRANS, NULL, 0,0);
		set_add_blender(0,0,0,fi);
		rectfill(active_page,0,0,active_page->w - 1,active_page->h - 1, White);
		solid_mode();
	}
}


//============================================================================
static void render() {
	if (ai_test) return;
	check_sound();
        char info[200];
        snprintf(info, sizeof info, "%s (%s%s)   fps:%4i",
		gfx_driver->name,
		(page_flip ? "PFlip" : "DBuff"),
		(forced_redraw_flag ? "[F]" : ""),
		fps
	);
	int swid = text_length(&font_micro,info) + 10;
	frame_count++;
	if (page_flip) {
			return; // no pageflip!
		fix_fubar_vram();
		show_mouse(NULL);
		acquire_bitmap(active_page);
		if (forced_redraw_flag) {
		// blit(mmam_splash, active_page,0,0,0,0,mmam_splash->w,mmam_splash->h);
		rectfill(active_page, 0, 0, SCREEN_W, top - 1, Black);
		if (top_string) textout_centre_ex(active_page, &font_bold, top_string, SCREEN_W / 2, 0, White, -1);
		} else manual_mouse->hide();
		ast->draw();
		hud->draw();
		free_edit->draw();
		gamelog->draw();
		gameqbox->draw();
		blit(mmam_splash, active_page, SCREEN_W - swid - 10, SCREEN_H - 15,SCREEN_W - swid - 10, SCREEN_H - 15, swid + 10, 15);
		textout_ex(active_page, &font_micro, info, SCREEN_W - swid+5, SCREEN_H - 15, White, -1);
		manual_mouse->show();
		flash();
		if (PAUSED) textout_centre_ex(active_page, &font_big, (char *)(input_map.paused == 2 ? "SAFTY PAUSE!" : "PAUSED."), SCREEN_W / 2, SCREEN_H / 2, White, Black);
		if (forced_redraw_flag) forced_redraw_flag--;
		release_bitmap(active_page);
		if (page_flip) show_video_bitmap(active_page);
		active_page = (active_page == page1 ? page2 : page1);
	} else {
		active_page = buffer;
		if (forced_redraw_flag) {
			//blit(mmam_splash, active_page,0,0,0,0,mmam_splash->w,mmam_splash->h);
			rectfill(active_page, 0, 0, SCREEN_W, top - 1, Black);
			if (top_string) textout_centre_ex(active_page, &font_bold, top_string, SCREEN_W / 2, 0, White, -1);
		}
		ast->draw();
		hud->draw();
		free_edit->draw();
		gamelog->draw();
		gameqbox->draw();
		blit(mmam_splash, active_page, SCREEN_W - swid - 10, SCREEN_H - 15,SCREEN_W - swid - 10, SCREEN_H - 15, swid + 10, 15);
		textout_ex(active_page, &font_micro, info, SCREEN_W - swid+5, SCREEN_H - 15, White, -1);
		show_mouse(buffer);
		flash();
		if (PAUSED) textout_centre_ex(active_page, &font_big, (char *)(input_map.paused == 2 ? "SAFTY PAUSE!" : "PAUSED."), SCREEN_W / 2, SCREEN_H / 2, White, Black);
		acquire_bitmap(screen);
		blit(buffer,screen,0,  0,  0,   0, SCREEN_W, SCREEN_H);
		release_bitmap(screen);
		show_mouse(screen);
		if (forced_redraw_flag) forced_redraw_flag--;
	}
	check_screen_shot();
check_sound();
	// there isn't much point in drawing more frames than your monitor's refresh rate.
	// slow this ride down.
	// if (fps >= 60)
	rest(0); //yield_timeslice(); /* give the network code a chance */
}


void game_rest(int ms) {
	if (ai_test) return;
  	int pause_till = ms_count + ms;
  	while (ms_count < pause_till) render();
}

void game_rest_nodraw(int ms) {
	if (ai_test) return;
	int pause_till = ms_count + ms;
  	while (ms_count < pause_till) {
         	check_sound();
         	rest(10);
	}
}




//============================================================================
int askyn(char *s) {
	int res;
	gameqbox->ask(s);
	while ( (res = gameqbox->poll()) < 0 ) { forced_redraw_flag = MAKE_DIRTY; render();}
	forced_redraw_flag = MAKE_DIRTY;
	return res;
}



static void poll_game() {
	while (game_logic) {
		game_turn++;
		check_kb_input();
		if (tiredofplaying) return;
		home_ast->poll();
		if (ast != home_ast) ast->poll();
		hud->poll();
		event.pop();
		ast = ship->my_ast;
		game_logic--;
		if (game_logic > 20) {
			printf((char *)"Warn: Game logic Swamping processor!\n");
			render();
			game_logic = 1;
		}
	}
	render();
	if (PAUSED) check_kb_input();
};

void run_cmd_list(char *cmd);

static void playgame() {
	check_scale();
	tiredofplaying = gamedone = 0;
	outstr((char *)"Monster Moster Asteroid Maddness V" VERSION);
	printf((char *)"playgame\n");
	set_mood(GM_FIGHT);
	fix_ivalu_desc();
	event.clear();
	alignment.clear();
	graphs = new graph_set();
	graphs->build_new_set();
	ship = new monst_t(&mb_netship);
	thehuman = new monst_t(&mb_worker);
	thehuman->give_weap(&IDF_LANCER);
	thehuman->worker_salary = 0;
	thehuman->worker_skill = 2;
	thehuman->aiu1 = ('R' << 24);
    thehuman->gestation = 0;
    thehuman->sex = 1;
    free(thehuman->name); 
    thehuman->name = strdup(player_name);
    thehuman->mhp = thehuman->hp = 140;
    thehuman->ai_func = ai_player;
        	
	hud = new hud_t();
	ast = home_ast = new asteroid_t(0);
	/*
	home_ast->spawn_monst_by(&mb_molber, 300,300, 7);
	home_ast->spawn_monst_by(&mb_furbit, 400,300, 4);
	home_ast->spawn_monst_by(&mb_radrunner, 500,300, 7);
	//home_ast->spawn_monst_by(&mb_drexler, 300,400, 3);
	home_ast->spawn_monst_by(&mb_mamothquito, 300,600, 3);
	*/
	remote_ast = NULL;
	mainbase = new monst_t(&mb_astbase);
	jumpgate_s = new monst_t(&mb_jumpgatenode);
	ast->add_monst_by(mainbase, 460., 220.);
	ast->add_monst(jumpgate_s);
	ast->jumpgate_x = ast->jumpgate_y = 90;
	jumpgate_s->x = jumpgate_s->y = 90;
	ast->add_monst_by(ship, 80., 240.);
	run_cmd_list(command_list);
	run_cmd_list(arg_command_list);	
	while ((!tiredofplaying) && !(gamedone))  poll_game();
	if (gamedone) {
		tiredofplaying = (ai_test ? 0 : !askyn((char *)"do you want to play again?"));
	}
	delete remote_ast;   remote_ast = NULL;
	delete home_ast;     home_ast = NULL;
	delete hud;          hud = NULL;
	delete graphs;       graphs = NULL;
}

void my_close_button_callback() {
  want_to_quit = 1;
}

void ten_ms_counter(void) {
	if (!PAUSED) if ( (( ms_count += 10) % 1000) == 0) {
		fps = frame_count;
		frame_count = 0;
	}
}
END_OF_FUNCTION(ten_ms_counter);

void game_logic_counter(void) {
        if (!PAUSED) game_logic++;
}

END_OF_FUNCTION(ten_ms_counter);
int init_graphics();


void mmam_intro();

int main(int argc, char *argv[]) {
	gameqbox = new QBOX();
	free_edit = new FREE_EDIT(input_map.chat_buffer, sizeof input_map.chat_buffer);
  	gamelog = new GAMELOG();
	printf((char *)"Welcome to Monster Monster Asteroid Madness.\n");
	allegro_init();
	jpgalleg_init();
#ifdef USE_PNG
        loadpng_init();
#endif
	install_timer();
	install_keyboard();
	install_mouse();
	do_randomize();
	input_map.last_menu = -1;
#ifdef USE_ADIME	
	if (adime_init() != 0) {  allegro_message("Error initializing Adime.\n"); return 1; }
#endif
	fixup_datafile(font_data);
	getstartupvals(argv + 1);
	printf((char *)"Startupvals OK.\n");
    set_window_title("Monster Monster Asteroid Maddness");
	set_close_button_callback(my_close_button_callback);
	set_color_conversion(COLORCONV_TOTAL);
	if (!arg_nosound) init_sound();
 	install_int(ten_ms_counter, 10);
 	if (!init_graphics()) return 1;
	if (!(skip_intro || arg_skip_intro)) mmam_intro();
	if (qfe_gamesetup() == QFE_NEW_GAME) {
		for (tiredofplaying = 0; !tiredofplaying;) {
			install_int(game_logic_counter, 1000 / TURNS_PER_SEC);
			playgame();
			remove_int(game_logic_counter);
		} 
	}
	delete gamelog;
	delete gameqbox;
	delete free_edit;
	return 0;
}

END_OF_MAIN();


// End.

