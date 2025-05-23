#include "mmam.h"
#include "albgi.h"
#include "qfe.h"
#include "bubbles.h"
#ifdef STANDALONE
#include <jpgalleg.h>
#ifdef USE_PNG
#  include <loadpng.h>
#endif
input_map_t input_map;

DATAFILE *sprites;
BITMAP *buffer;
BITMAP *page1, *page2;
BITMAP *active_page;

int  want_to_quit, tiredofplaying;

volatile int ms_count = 0, fps = 0, frame_count = 0, page_flip = 0, forced_redraw_flag = 0, game_logic = 0;
int game_turn;

int do_scr_shot;
int gamedone;

BITMAP *mmam_splash, *mmam_play_arrow;

//void my_close_button_callback() {
//  want_to_quit = 1;
//}

void ten_ms_counter(void) {
	if ( (( ms_count += 10) % 1000) == 0) {
	}
}
END_OF_FUNCTION(ten_ms_counter);


#endif

static int instr_top = 30, img_boundry;

static void instructions_blit_and_wait(int &yy) {
	scare_mouse();
	blit(active_page,screen,0,0, 0,0, screen->w, screen->h);
	unscare_mouse();
	readkey(); while (keypressed()) readkey();
	clear_bitmap(active_page);
	yy = instr_top;
	img_boundry = instr_top;
}

int instructions(DIALOG *d) {
	char fn[512];
	char scrap[8000], *s = scrap;
	while(keypressed()) readkey();
	#ifdef STANDALONE
	sprintf(fn,"../data/custom/instructs.conf");	
	#else
	sprintf(fn,"%s/baseconf.dat#INSTRUCTS_CONF", data_dir);
	#endif
	//BITMAP *h = (BITMAP *)sprites[SPR_HELP0].dat;
	scare_mouse();
	BITMAP *t = active_page = create_bitmap( screen->w, screen->h);
    clear_bitmap(t);
    
	int xx = 30;
	 img_boundry = _textout_y = instr_top;		
	int bottom = active_page->h - 80;
	PACKFILE *pf = pack_fopen(fn, "r");
    FONT *df = &font_bold;
	if (pf) {
		while (!pack_feof(pf)) {
			pack_fgets(s, sizeof scrap, pf);
			if (s[0] == '@') {
				if (strncmp(s, "@header ",8) == 0) {
					s += 8; textout_box(t, &font_bold_plus, s,
        				xx, _textout_y, xx + 320, bottom, LightGreen, -1
					);
					_textout_y += 4;
				} else if (strncmp(s, "@img ",5) == 0) {
					s += 5;
					BITMAP *img = NULL; 
					#if STANDALONE
					char imgfn[512];
					sprintf(imgfn, "../data/sprites.dat#%s", s);
					//img = load_bitmap(imgfn, NULL);
					if (!img) img = load_jpg(imgfn, NULL);
					#else
					for (int c = 0; c < SPR_COUNT; c++) {
						if (strcmp(s,  get_datafile_property(&sprites[c], DAT_ID('N','A','M','E'))) == 0) {
							img = (BITMAP *)sprites[c].dat;
						}
					}
					#endif
					if (img) {
						if (_textout_y < img_boundry) _textout_y = img_boundry;
						if (img->h + _textout_y > bottom) {
								instructions_blit_and_wait(_textout_y); 
						}
						blit(img, active_page,0,0, xx + 355, _textout_y, img->w, img->h);			
						img_boundry = _textout_y + img->h - 12;
												
					}
					#ifdef STANDALONE 
					else textprintf_ex(active_page, &font_tiny, 320 + xx, _textout_y, LightGreen, -1 , (char *)"load failed :%s",imgfn);
					#endif
					continue;
				} else if (strncmp(s, "@gobutton ",10) == 0) {
					//later
				} else if (strncmp(s, "@golabel ",9) == 0) {
					//later
				} else if (strncmp(s, "@note",5) == 0) {
				} else if (strncmp(s, "@page",5) == 0) {
					if (_textout_y > (bottom - instr_top) / 2 + instr_top) {
						instructions_blit_and_wait(_textout_y);
					}  
				} else if (strncmp(s, "@eof",4) == 0) {
					s = scrap; break;
				} 
				s = scrap;

			} else if (strlen(s) == 0) { 
				s = scrap; 
				textout_box(t, df, scrap,
        			xx, _textout_y, xx + 320, bottom, LightGreen, -1
				);				
			} else { s += strlen(s); *s = ' '; ++s; *s = 0; continue; }
			while (_textout_text) {
					textout_box(t, df, _textout_text,
        				xx, _textout_y, xx + 320, bottom, LightGreen, -1
					);				
			}
			_textout_y += 16;
			if (_textout_y > bottom) {
				instructions_blit_and_wait(_textout_y); 
			}			
		}
		pack_fclose(pf);
	} else 	textout_box(active_page, &font_bold, "<Missing Instructions File>",
        xx, _textout_y, xx + 320, active_page->h - 30, LightGreen, -1
	);    
    //blit(screen,t,0,0, 0,0, screen->w, screen->h);
	//stretch_blit(h, screen, 0,0, h->w, h->h, 0,0, screen->w, screen->h);
	instructions_blit_and_wait(_textout_y); 
			
    
    destroy_bitmap(t);
    #ifdef USE_ADIME
	return ADIME_REDRAW;
	#else
	return 0; // This number is unimportant.
	#endif
}

#ifdef STANDALONE

int setup_graphics(int w, int h, int want_windowed, int want_pageflip, int accel);

int init_graphics();

int main(int argc, char *argv[] ) {
	allegro_init();
	jpgalleg_init();
#ifdef USE_PNG
        loadpng_init();
#endif
	install_timer();
	install_keyboard();
	install_mouse();
	input_map.last_menu = -1;
	fixup_datafile(font_data);
	//getstartupvals(argv + 1);
	set_window_title("MMaM Instructions");
	//set_close_button_callback(my_close_button_callback);
	set_color_conversion(COLORCONV_TOTAL);
	// if (!arg_nosound) init_sound();
 	//install_int(ten_ms_counter, 10);
 	//if (!init_graphics()) return 1;
	int cd = desktop_color_depth();
   	set_color_depth( (cd < 15 ? 16 : cd) );
   	int adm = ( GFX_AUTODETECT_WINDOWED );
	set_gfx_mode(adm, 630,464, 0,  0); 
	albgi_init();   
	instructions(NULL);

	//if (setup_graphics(default_screen_w, default_screen_h, prefer_windowed, enable_page_flip[prefer_windowed], 2) != 0) return 0;
	
	
}
END_OF_MAIN();

#endif
