#include "mmam.h"
#include <math.h>
#include <allegro.h>
#include <jpgalleg.h>
#include "qfe.h"
#include "gfx.h"
#include "gamelog.h"
#include "graphs.h"

BITMAP *tile_map[16 * NUM_SURFACES];

int vram_full = 0;
int sprites_in_vram = 0;
BITMAP *sprite_list[SPR_COUNT];
int sprite_location[SPR_COUNT];

static void endprog(char *s) { set_gfx_mode(GFX_TEXT, 0,0,0,0);  puts(s);  exit(0);}

/* Although the numbers can really be anything, we've decided that the
 * range of the gamma_factor is 0.5 to 1.5.
 *
 *   0.5 = brightest
 *   1.0 = "normal"
 *   1.5 = darkest
 */
 
float gamma_factor = 1.0;

static float last_gamma = -1;
static int table[256];


static void build_table (float gamma) {
	int i, inf;

	/* Table already built. */
	if (last_gamma == gamma) return;

	/* Otherwise update the table. */
	for (i = 0; i < 256; i++) {
		/* formula from Quake */
		inf = (int)floor(255 * pow ((i + 0.5) / 255.5, gamma) + 0.5);
		table[i] = MID (0, inf, 255);
	}

	last_gamma = gamma;
}


static int gamma_func (float gamma, int i) {
    build_table (gamma);

    i = MID (0, i, 255);
    return table[i];
}


void apply_gamma(BITMAP *bmp, float gamma) {
    int depth;
    int x, y;

    /* fast path */
    if (gamma == 1.0)
	return;

    depth = bitmap_color_depth (bmp);
    int pink = bitmap_mask_color (bmp);
    for (y = 0; y < bmp->h; y++) {
	for (x = 0; x < bmp->w; x++) {
	    int c = getpixel (bmp, x, y);

	    if (c != pink) {
		int r = gamma_func (gamma, getr_depth (depth, c));
		int g = gamma_func (gamma, getg_depth (depth, c));
		int b = gamma_func (gamma, getb_depth (depth, c));

		putpixel (bmp, x, y, makecol_depth (depth, r, g, b));
	    }
	}
    }
}


void create_tile_map16(BITMAP *from, int start) {
	for (int c=0; c<16;c++) {tile_map[c + start] = create_bitmap(32,32);}
	blit(from, tile_map[0 + start],  0,  0, 0, 0, 32, 32);
	blit(from, tile_map[1 + start], 32,  0, 0, 0, 32, 32);
	blit(from, tile_map[2 + start],  0, 32, 0, 0, 32, 32);
	blit(from, tile_map[3 + start], 32, 32, 0, 0, 32, 32);
	draw_sprite_h_flip(  tile_map[4 + start],  tile_map[0 + start], 0, 0);
	draw_sprite_h_flip(  tile_map[5 + start],  tile_map[1 + start], 0, 0);
	draw_sprite_h_flip(  tile_map[6 + start],  tile_map[2 + start], 0, 0);
	draw_sprite_h_flip(  tile_map[7 + start],  tile_map[3 + start], 0, 0);

	draw_sprite_v_flip(  tile_map[8 + start],  tile_map[0 + start], 0, 0);
	draw_sprite_v_flip(  tile_map[9 + start],  tile_map[1 + start], 0, 0);
	draw_sprite_v_flip(  tile_map[10 + start], tile_map[2 + start], 0, 0);
	draw_sprite_v_flip(  tile_map[11 + start], tile_map[3 + start], 0, 0);

	draw_sprite_vh_flip( tile_map[12 + start], tile_map[0 + start], 0, 0);
	draw_sprite_vh_flip( tile_map[13 + start], tile_map[1 + start], 0, 0);
	draw_sprite_vh_flip( tile_map[14 + start], tile_map[2 + start], 0, 0);
	draw_sprite_vh_flip( tile_map[15 + start], tile_map[3 + start], 0, 0);
}


void clearscr() {
	forced_redraw_flag = MAKE_DIRTY;
}

static int load_sprites_dat_idx;

static void load_sprites_dat_callback(DATAFILE *d) {
	load_sprites_dat_idx++;
	textprintf_ex(screen, &font_bold, 10 , 4,   LightGreen, Black ,
  		"LOADING SPRITES.DAT (%i of %i).    ", load_sprites_dat_idx, SPR_COUNT
	);	
}

int getimages() {
	int mm = ms_count;
	graph_set *last_graphs = new graph_set;
	last_graphs->load_last();
	last_graphs->set_draw_zone(20, 42, SCREEN_W - 40, SCREEN_H - 72);
	if (last_graphs->num_graphs) {
	    textprintf_ex(buffer, &font_bold, 30 , 20,   White, Black ,
  		    "Last Game:"
	    );
	    last_graphs->disp_seconds = last_graphs->graph[0].valid_secs;
	    last_graphs->display_only_user();
	    last_graphs->draw(buffer);
	} else {
	    textprintf_ex(buffer, &font_bold, 30 , 20,   White, Black ,
  		    "Please Wait ..."
	    );	
	}
	blit(buffer, screen, 0,0, 0,0, buffer->w, buffer->h);
    delete last_graphs;
    
	char fn[200]; snprintf(fn, sizeof fn, "%s/sprites.dat", data_dir);
        load_sprites_dat_idx = 0;
	if ( !(sprites = load_datafile_callback(fn, load_sprites_dat_callback)) ) {
		allegro_message("Unable to load required Datafile: %s", fn);
        return 0;
	}
	for (int c = 0; ; c++) {
		if (sprites[c].type == DAT_BITMAP || sprites[c].type == DAT_JPEG) {
    			apply_gamma((BITMAP *)sprites[c].dat, gamma_lvl);
		} else if (sprites[c].type == DAT_END) {
			if (c == SPR_COUNT + 1) break;
			allegro_message("Datafile has wrong version: %s (%i=bad %i=good)", fn, c, SPR_COUNT + 1);
           	return 0;
    	}
	}
	printf("sprites.dat loaded in %0.3f seconds.\n",(ms_count - mm) / 1000.);

	for (int c = 0; c < NUM_SURFACES; c++ ) create_tile_map16( (BITMAP *) sprites[SPR_SURFACE0 + c].dat, c * 16);
	// create_player_chat_bubbles();
	mmam_splash = (BITMAP *) sprites[SPR_SPLASH].dat;
	mmam_play_arrow = (BITMAP *) sprites[SPR_ARROW_BMP].dat;
	if (mmam_splash->w != SCREEN_W || mmam_splash->h != SCREEN_H ) {
		BITMAP *tb = create_bitmap(SCREEN_W,SCREEN_H);
		if ( tb ) {
			stretch_blit(mmam_splash, tb, 0,0, mmam_splash->w, mmam_splash->h, 0,0, tb->w, tb->h);
			destroy_bitmap(mmam_splash);
			sprites[SPR_SPLASH].dat = (DATAFILE *)tb; 
			mmam_splash = tb;
		}
	}
    
	return 1;
}

void load_all_to_vram();

static int good_scale = 0;

void check_scale() {
  	if (good_scale) return;
        load_all_to_vram();
	good_scale = 1;
}



//============================================================================

int setup_graphics(int w, int h, int want_windowed, int want_pageflip, int accel);

void reinit_graphics() {
	setup_graphics(default_screen_w, default_screen_h, prefer_windowed, enable_page_flip[prefer_windowed], 2);
	unload_datafile(sprites);
        getimages();
        good_scale = 0;
	check_scale();
	clearscr();
	set_mouse_sprite((BITMAP *)(sprites[SPR_POINTER].dat));
        scare_mouse();
	blit(mmam_splash, screen, 0,0, 0,0, mmam_splash->w, mmam_splash->h);
	show_mouse(screen);
}

int init_graphics() {
	set_badmode();
	if (setup_graphics(default_screen_w, default_screen_h, prefer_windowed, enable_page_flip[prefer_windowed], 2) != 0) return 0;
	unset_badmode();
	printf("Basic Allegro Init OK.\n");
	if (!getimages()) return 0;
    good_scale = 0;
	check_scale();
	printf("Load Images OK.\n");
	set_mouse_sprite((BITMAP *)(sprites[SPR_POINTER].dat));
        return 1;
}


//============================================================================



static int save_page_flip, vram_is_fubar;

static void remove_page_flip() {
	save_page_flip = page_flip;
	page_flip = 0;
	forced_redraw_flag = MAKE_DIRTY;
	if (save_page_flip) printf("page flip removed!\n");
}


static void set_forced_redraw_flag() {
	if (save_page_flip != page_flip) printf("page flip restored!\n");
	page_flip = save_page_flip;
	vram_is_fubar = 1;
	forced_redraw_flag = MAKE_DIRTY;
}

static void init_switch_background() {
        if (set_display_switch_mode(SWITCH_BACKAMNESIA) == 0) {
                // windows has to run like this.
		int ok1 = (set_display_switch_callback(SWITCH_OUT, remove_page_flip) == 0);
        	int ok2 = (set_display_switch_callback(SWITCH_IN, set_forced_redraw_flag) == 0);
                if (!ok1 || !ok2) {
                	save_page_flip = page_flip = 0;
                	if (!ok2) {
                		if (set_display_switch_mode(SWITCH_BACKGROUND) == 0) {
                			printf("Running SWITCH_BACKAMNESIA (no pageflip)");
                			return;
                		}
                	}
        	}
		printf("Running SWITCH_BACKAMNESIA [%s,%s] (%s)\n",
			(ok1 ? "good" : "none"),
                	(ok2 ? "good" : "none"),
                	(ok1 && ok2 ? "ok" : !ok1 ? "page flip unsafe?" : "page flip unsafe?, crud on task switch")
                );
        } else if (set_display_switch_mode(SWITCH_BACKGROUND) == 0) {
		int ok1 = (set_display_switch_callback(SWITCH_OUT, remove_page_flip) == 0);
        	int ok2 = (set_display_switch_callback(SWITCH_IN, set_forced_redraw_flag) == 0);
                if (!ok1 || !ok2) {
                	save_page_flip = page_flip = 0;
                }
		printf("Running SWITCH_BACKGROUND [%s,%s] (%s)\n",
			(ok1 ? "good" : "none"),
                	(ok2 ? "good" : "none"),
                	(ok1 && ok2 ? "ok" : "page flip unsafe?")
                );
	} else {
        	printf("cannot set SWITCH_BACKGROUND (no pageflip, network code may screw up on task switch)\n");
		save_page_flip = page_flip = 0;
	}
}



static void peek_at_abilitys() {
        char id_str[5] = "????";
        memcpy((void *)id_str, (void *)&gfx_driver->id, 4);//there's a quicker way i'm sure.
	char buff[500] = "Capabilities:";
	if (gfx_capabilities & GFX_CAN_SCROLL) strcpy(buff + strlen(buff), "GFX_CAN_SCROLL");
	if (gfx_capabilities & GFX_CAN_TRIPLE_BUFFER) strcpy(buff + strlen(buff), "GFX_CAN_TRIPLE_BUFFER");
#define CHECK_HW(x) if (gfx_capabilities & GFX_HW_ ## x) strcpy(buff + strlen(buff), " " #x);
           CHECK_HW(CURSOR)
           CHECK_HW(HLINE)
           CHECK_HW(HLINE_XOR)
           CHECK_HW(HLINE_SOLID_PATTERN)
           CHECK_HW(HLINE_COPY_PATTERN)
           CHECK_HW(FILL)
           CHECK_HW(FILL_XOR)
           CHECK_HW(FILL_SOLID_PATTERN)
           CHECK_HW(FILL_COPY_PATTERN)
           CHECK_HW(LINE)
           CHECK_HW(LINE_XOR)
           CHECK_HW(TRIANGLE)
           CHECK_HW(TRIANGLE_XOR)
           CHECK_HW(GLYPH)
           CHECK_HW(VRAM_BLIT)
           CHECK_HW(VRAM_BLIT_MASKED)
           CHECK_HW(MEM_BLIT)
           CHECK_HW(MEM_BLIT_MASKED)
           CHECK_HW(SYS_TO_VRAM_BLIT)
           CHECK_HW(SYS_TO_VRAM_BLIT_MASKED)
#undef CHECK_HW
	printf("%s [%s] %s\n", gfx_driver->name, id_str, buff);
}




int copy_sprite_to_vram(int idx) {
        BITMAP *vimage, *image = (BITMAP *)sprites[idx].dat;
        if (!page_flip) {
		sprite_list[idx] = image;
	} else if (sprite_location[idx] &~ 2) {
   		if ( (vimage = create_video_bitmap(image->w, image->h)) ) {
       			acquire_bitmap(vimage); // too paranoid?
   			blit(image,vimage,0,0,0,0,image->w, image->h);
			release_bitmap(vimage);
			sprite_location[idx] |= 2;
			sprites_in_vram++;
			return 1;
                } else sprite_list[idx] = image;
	} else return 1;
	return 0;
}

/*
int move_sprite_to_vram(int idx) {
        BITMAP *vimage, *image = (BITMAP *)sprites[idx].dat;
        if (!page_flip) {
		sprite_list[idx] = image;
	} else if (sprite_location[idx] &~ 2) {
   		if ( (vimage = create_video_bitmap(image->w, image->h)) ) {
       			acquire_bitmap(vimage); // too paranoid?
   			blit(image,vimage,0,0,0,0,image->w, image->h);
			release_bitmap(vimage);
			sprite_location[idx] = 2;
			sprites_in_vram++;
			free(sprites[idx].dat);
			sprites[idx].dat = NULL;
			return 1;
                } else sprite_list[idx] = image;
	} else return 1;
	return 0;
}
*/

int unload_sprite_from_vram(int idx) {
  	int ret = 0;
	if (sprite_location[idx] & 2) {
                if (sprite_list[idx]) {
                	destroy_bitmap(sprite_list[idx]) ;
                	printf("freed spritlist[%i]\n",idx);
       		} else endprog("inconsistant sprite_location!\n");
                sprites_in_vram--;
                ret = 1;
 	}
	sprite_location[idx] = ((sprite_list[idx] = (BITMAP *)sprites[idx].dat) ? 1 : 0);
	return ret;
}

void load_all_to_vram() {
	for (int c = 0; c < SPR_COUNT; c++) unload_sprite_from_vram(c);
	if (page_flip && (gfx_capabilities & GFX_HW_VRAM_BLIT)) {
		for (int c = 0; c < SPR_COUNT; c++) if (c != SPR_SPLASH && c != SPR_ARROW_BMP) {
			copy_sprite_to_vram(c);
		}
        	copy_sprite_to_vram(SPR_SPLASH);
        	printf("copyed %i of %i sprites into vram.\n", sprites_in_vram, SPR_COUNT-1);
	} else printf("vram copy not needed.\n");
}

void fix_fubar_vram() {
	if (!vram_is_fubar || !page_flip) return;
	for (int c = 0; c < SPR_COUNT; c++) if (sprite_location[c] & 2) {
		BITMAP *image = (BITMAP *)sprites[c].dat;
		acquire_bitmap(sprite_list[c]); // too paranoid?
		blit(image, sprite_list[c],0,0,0,0,image->w, image->h);
		release_bitmap(sprite_list[c]);
	}
	vram_is_fubar = 0;
	forced_redraw_flag = MAKE_DIRTY;
	printf("fubar vram fixed.\n");
}


int setup_graphics(int w, int h, int want_windowed, int want_pageflip, int accel) {
// trys to grab good hicolor mode in fewest trys (in the want_windowed
// mode at up to accel) at the desktop's color depth.
// it is possable to get somthing completely different than what you ask for
// aka GFX_SAFE

   /* advice from exaccel and windows spacific section of the allegro docs on how to get page flipping up*/

   int cd = desktop_color_depth();
   page_flip = ( enable_page_flip[want_windowed] ? want_pageflip : 0);
   set_color_depth( (cd < 15 ? 16 : cd) );
   int ok = 0, pass = 0;
   if ( !ok && accel < 0) { ok = (set_gfx_mode(GFX_SAFE, 320, 200, 0, 0) == 0); pass++; }
   if (accel >=0) {
   #ifdef _WIN32
            if (want_windowed) {
                 switch (desktop_color_depth()) {
                 case 8: ok = (set_gfx_mode(GFX_DIRECTX_WIN, w, h, 0, 0) == 0); pass++;
                 	if (ok) allegro_message("Running in 16bbp -> 8bpp mode (Yuck)");
                 	break;
                 case 16:
            		if ( !ok && accel >= 3) {
            			ok = (set_gfx_mode(GFX_DIRECTX_OVL, w, h, 0, 0) == 0); pass++;
            			if ( !ok ) {
                 			set_color_depth(15);
                                	ok = (set_gfx_mode(GFX_DIRECTX_OVL, w, h, 0, 0) == 0); pass++;
            				if ( !ok ) set_color_depth(16);
                               }
                        }
            		if ( !ok ) {
            			ok = (set_gfx_mode(GFX_DIRECTX_WIN, w, h, 0, 0) == 0); pass++;
            			if ( !ok ) {
                 			set_color_depth(15);
                                	ok = (set_gfx_mode(GFX_DIRECTX_WIN, w, h, 0, 0) == 0); pass++;
            				if ( !ok ) set_color_depth(16);
                               }
                        }
                        break;
                default:
                        if ( !ok && accel >= 3) { ok = (set_gfx_mode(GFX_DIRECTX_OVL, w, h, 0, 0) == 0); pass++; }
            		if ( !ok ) { ok = (set_gfx_mode(GFX_DIRECTX_WIN, w, h, 0, 0) == 0); pass++; }
                        break;
                }
            }
            if ( !ok ) while (accel >= 0) {
	            int adm = ( accel == 0 ? GFX_DIRECTX_SAFE : accel == 1 ? GFX_DIRECTX_SOFT : GFX_DIRECTX_ACCEL);
                    pass++; if ( (ok = (set_gfx_mode(adm, w, h, 0, 0) == 0) ) ) break; accel--;
            }
   #else
   // cross platform stuff
   	set_gfx_mode(GFX_TEXT, w, h, 0,  0);//workarround for xwindows driver
   	int adm = ( want_windowed ? GFX_AUTODETECT_WINDOWED : GFX_AUTODETECT);
	if ( !ok && page_flip) { ok = ( set_gfx_mode(adm, w, h, 0,  w * 2) == 0 ); pass++; }
        if ( !ok ) { ok = ( set_gfx_mode(adm, w, h, 0,  0) == 0); pass++; }
        if ( want_windowed) {
                // drop the windowed requirement if we don't have a mode yet!
		if ( !ok && want_pageflip) { ok = ( set_gfx_mode(GFX_AUTODETECT, w, h, 0,  w * 2) == 0 ); pass++; }
        	if ( !ok ) { ok = ( set_gfx_mode(GFX_AUTODETECT, w, h, 0,  0) == 0); pass++; }
	}
   #endif
        if ( !ok ) {
          ok = (set_gfx_mode(GFX_SAFE, 640, 480, 0, 0) == 0);  pass++;
          if (ok) {
             // allegro_message( "Using GFX_SAFE (that sucks).\n");
          }
        }
        printf("ok mode on pass%i.\n", pass);
   }
   if (!ok) {
        allegro_message( "Could not set video mode. Reason: %s\n", allegro_error );
   }

   if (buffer) {destroy_bitmap(buffer); buffer = NULL; }
   if (page1) {destroy_bitmap(page1);  page1 = NULL; }
   if (page2) {destroy_bitmap(page2);  page2 = NULL; }
   // if (manual_mouse) { delete manual_mouse; manual_mouse = NULL; }
   if (page_flip) {
   	page1 = create_video_bitmap(SCREEN_W, SCREEN_H);
   	page2 = create_video_bitmap(SCREEN_W, SCREEN_H);
        manual_mouse = new MANUAL_MOUSE();
   	if ((!page1) || (!page2) || (!manual_mouse->ok) ) {
        	delete manual_mouse; manual_mouse = NULL;
      		page_flip = 0;
   	}
   }
   // I create the double buffer even if page flipping is available.
   buffer = create_bitmap(SCREEN_W, SCREEN_H);

   peek_at_abilitys();

   init_switch_background();//keep running when switched away

   active_page = ( page_flip ? page2 : buffer);
   save_page_flip = page_flip;
   albgi_init();
   return 0;
}
