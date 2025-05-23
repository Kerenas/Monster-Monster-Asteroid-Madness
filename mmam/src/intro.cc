#include "config.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <allegro.h>
#include <jpgalleg.h>
#include "mmam.h"
#include "albgi.h"
#include "dsound.h"
#include "fonts.h"
#include "graphs.h"
#ifdef USE_PNG
#  include <loadpng.h>
#endif
#include "altheora.h"

/*
 {1, 400},
 {2, 180},
 {7, 200},
 {17, 180},
 {36, 200},
 {52, 450},
 {53, 190},
 {57, 170},
 {64, 450},
 {65, 500},
 {66, 150},
 {72, 220},
 {73, 180},
 {122, 200},
 {129, 180},
 {135, 500},
 {136, 180},
 {140, 500},
 {141, 160},
 {145, 300},
 {146, 150},
 {151, 180},
 {193, 360},
 {194, 180},
 {205, 200},
 {209, 800},
 {210, 200},
 {211, 240},
 {212, 260},
 {213, 150},  //222 = last frame.
*/

static char *credits[] = {
        	(char *)"Code: Eric Killeen",
        	(char *)"Graphics: Jason Killeen",
        	(char *)"Sound FX: Eric Killeen",
        	(char *)"Music: Jason Killeen",
        	(char *)"Special Guest Apperance: Nathan \"Big Nate\" Killeen",
        	(char *)"Graphics Lib: Allegro (http://alleg.sf.net/)",
            (char *)"Jpeg Decoder Lib: JpegAlleg (http:///)",
        	(char *)"Mod playing Lib: DUMB (http://dumb.sf.net/)",
        	(char *)"HAVE FUN!",
        	0
};

static frame_time intro_timing[] = {
 {1, 430},
 {2, 160},
 {7, 190},
 {17, 170},
 {36, 190},
 {52, 280},
 {53, 170},
 {57, 150},
 {66, 180},
 {76, 160}, // its labeled 100? furbit strapped in.
 {85, 200},
 {88, 170},

 {92, 190},
 {99, 160},
 {102, 180},
 {105, 160},
 {110, 170},

 {115, 180},
 {122, 150},
 {126, 210},
 {128, 150},
 {132, 190},
 {148, 160}, // labled 200? in cage
 {161, 170},
 {162, 190},
 {163, 170},
 {164, 190},
 {164, 180},
 {179, 220},
 {186, 230},
 {188, 180},
 {211, 160},
 {-1,-1}, // end marker
};


#define NUM_BAR_MUL 12
#define NUM_BAR_X 64
#define FLAIR_MS 8000.
#define FADE_MS 3200.
#define SCROLLON_MS 800.

extern BITMAP *buffer, *mmam_splash;



static void spin_asteroid() {
	BITMAP *ast_spr = (
		(BITMAP *)sprites[SPR_AA0001 + (ms_count / 400 % 14)].dat
	);
	rectfill(buffer,0,0,ast_spr->w,ast_spr->h,(arg_cheater ? Red : Blue));
	draw_sprite(buffer,ast_spr,0,0);
}

static int check_quit_intro() {
        if (key[KEY_E] && key[KEY_J] && key[KEY_K] && !arg_cheater) arg_cheater = 1;
      	return (key[KEY_SPACE] || key[KEY_ESC] || key[KEY_ENTER]);
}


static void mmam_fallback_intro() {
	set_mood(GM_TITLE);
        float dir_flipflop = 1.0;
	BITMAP *cutter = create_bitmap(NUM_BAR_X, NUM_BAR_MUL);
	static float bar_mul[NUM_BAR_MUL] = {
		-1.208,1.410,
		-1.307,1.277,
		-1.250,1.331,
		-1.100,1.675,
		-1.470,1.200,
		-1.500,1.090
	};
	int start_ms = ms_count;
	float t;
	clear_to_color(cutter,1);
	textout_centre_ex(cutter, font, (char *)"MMaM", NUM_BAR_X / 2, 0, 0, -1);
	int psiz = SCREEN_W / NUM_BAR_X;
        do {
               	clear(buffer);
                spin_asteroid();
               	// set_trans_blender(0,0,0,0x10);
                //draw_trans_sprite(buffer,mmam_splash,0,0);
               	//for (int x = 2; x < SCREEN_W; x += 4) vline(buffer,x,0,SCREEN_H,GhostGreen);
                t = (FLAIR_MS - (ms_count - start_ms)) / FLAIR_MS;
                if (t < 0.) t = 0.;
        	for (int y = 0; y < NUM_BAR_MUL; y++) for (int x = 0; x < NUM_BAR_X; x++) {
			if (getpixel(cutter,x,y) == 0) {
				int ox = (psiz * x) + (int)(floor(bar_mul[y] * SCREEN_W * t));
				int oy = (psiz * y) + SCREEN_H / 3;
				blit(mmam_splash,buffer,ox,oy,ox,oy,psiz,psiz);
			}
        	}
		check_sound();
                if (check_quit_intro()) goto exit_mmam_intro;
                blit(buffer, screen, 0,0,0,0, SCREEN_W, SCREEN_H);
        } while (t > 0.);
        for (char **str=credits; *str; str++) {
        	start_ms = ms_count;
        	do {
                	check_sound();
                        spin_asteroid();               	
                	if (check_quit_intro()) goto exit_mmam_intro;
        	} while (ms_count - start_ms < 400);
        	start_ms = ms_count;
        	do {
        		t = (SCROLLON_MS - (ms_count - start_ms)) / SCROLLON_MS;
                	if (t < 0.) t = 0.;
                	int oy = 2 * SCREEN_H / 3;
                        int ox = (int)(floor((SCREEN_W / 2.) + ( SCREEN_W  * t * dir_flipflop)));
                        rectfill(buffer,0,oy, SCREEN_W, oy + 20, Black);
                        textout_centre_ex(buffer, &font_bold, *str, ox, oy, White, -1);
                        blit(buffer, screen, 0,0,0,0, SCREEN_W, SCREEN_H);
			check_sound();
			spin_asteroid();               	
                	if (check_quit_intro()) goto exit_mmam_intro;

        	} while (t > 0.);
        	start_ms = ms_count;
        	do {
                	check_sound();
                	spin_asteroid();
                	if (check_quit_intro()) goto exit_mmam_intro;
        	} while (ms_count - start_ms < 1200);
        	start_ms = ms_count;
        	do {
        		t = 1. - ((SCROLLON_MS - (ms_count - start_ms)) / SCROLLON_MS);
                	int oy = 2 * SCREEN_H / 3;
                        int ox = (int)(floor((SCREEN_W / 2.) + ( SCREEN_W * t * dir_flipflop)));
                        rectfill(buffer,0,oy, SCREEN_W, oy + 20, Black);
                        textout_centre_ex(buffer, &font_bold, *str, ox, oy, White, -1);
                        blit(buffer, screen, 0,0,0,0, SCREEN_W, SCREEN_H);
			check_sound();
			spin_asteroid();
      	                if (check_quit_intro()) goto exit_mmam_intro;
        	} while (t < 1.);
                dir_flipflop *= -1.0;
        }
        start_ms = ms_count;
        do {
                t = (FADE_MS - (ms_count - start_ms)) / FADE_MS;
                if (t < 0.) t = 0.;
		clear(buffer);
               	set_trans_blender(0,0,0,255 - (int)floor(254. * t));
                draw_trans_sprite(buffer,mmam_splash,0,0);
               	if (t < 0.) t = 0.;
        	for (int y = 0; y < NUM_BAR_MUL; y++) for (int x = 0; x < NUM_BAR_X; x++) {
			if (getpixel(cutter,x,y) == 0) {
				int ox = (psiz * x);
				int oy = (psiz * y) + SCREEN_H / 3;
				blit(mmam_splash,buffer,ox,oy,ox,oy,psiz,psiz);
			}
        	}
		check_sound();
                if (check_quit_intro()) goto exit_mmam_intro;
                blit(buffer, screen, 0,0,0,0, SCREEN_W, SCREEN_H);
        } while (t > 0.);
exit_mmam_intro:
        blit(mmam_splash,buffer, 0,0,0,0, SCREEN_W, SCREEN_H);
	blit(mmam_splash,screen, 0,0,0,0, SCREEN_W, SCREEN_H);
	destroy_bitmap(cutter);
}

PACKFILE *load_datafile_start(char *filename, int &count ) {
    
    if (!filename) return NULL;
	int type;
   	PACKFILE *f = pack_fopen(filename, F_READ_PACKED);
   	if (!f) return NULL;
	type = pack_mgetl(f);
   	if (type == DAT_MAGIC) {
      		count = pack_mgetl(f);
      		return f;
   	}
   	count = 0;
   	pack_fclose(f);
   	return NULL;
}

BITMAP *load_next_datafile_jpg(PACKFILE *f, int &count ) {
// section uses old style datafile update later.
#if 0 
	if (!f) return NULL;
	BITMAP *bmp = NULL;
	int type;
	while (!pack_feof(f)) {
      		type = pack_mgetl(f);
      		if (type == DAT_PROPERTY) {
        		pack_mgetl(f);
   			int size = pack_mgetl(f);
      			pack_fseek(f, size);
      		} else {
   			PACKFILE *ff = pack_fopen_chunk(f, FALSE);
   			if (ff) {
      				int d = ff->todo;
				void *rawdata = malloc(d);
				pack_fread(rawdata, d, ff);
      				pack_fclose_chunk(ff);
      				bmp = (
      				#ifdef USE_PNG
      					type == DAT_ID('P','N','G',' ') ? load_memory_png(rawdata, d, NULL) :
				#endif
      					type == DAT_JPEG ? load_memory_jpg(rawdata, d, NULL) : NULL
				);
      				free(rawdata);
   			}
   			count--;
			return bmp;
      		}
	}
#endif	
	return NULL;
}



void mmam_intro() {
        float dir_flipflop = 1.0;
	set_mood(GM_TITLE);
        int start_ms = ms_count;
	float t;
	int psiz = SCREEN_W / NUM_BAR_X;
	int count;
	BITMAP *bmp = NULL;
	if ( play_movie((char *)"intro.ogg", intro_timing) || play_movie((char *)"video.dat#INTRO_OGG", intro_timing) ) {
		bmp = create_bitmap(SCREEN_W,SCREEN_H);
		blit(screen,bmp,0,0,0,0,SCREEN_W,SCREEN_H);
	} else {
		mmam_fallback_intro(); return;
	#if 0
		PACKFILE *pf = load_datafile_start("introp.dat", count );
		if (!pf) { mmam_fallback_intro(); return; }
		bmp = load_next_datafile_jpg(pf, count);
		if (!bmp) { pack_fclose(pf); mmam_fallback_intro(); return; }
        	int tt = 0;
		int f = 1;
		int ttotal = intro_timing[tt].ms;
		if (bmp) while (1) {
			if (ms_count - start_ms > ttotal) {
				f++;
				BITMAP *nbmp = load_next_datafile_jpg(pf, count);
				if (!nbmp) break;
				destroy_bitmap(bmp);bmp = nbmp;
				if (intro_timing[tt + 1].first == f) tt++;
				ttotal += intro_timing[tt].ms;
			}
			check_sound();
			if (check_quit_intro()) goto exit_mmam_intro;
			stretch_blit(bmp, screen, 0,0, bmp->w, bmp->h, 0,0, SCREEN_W, SCREEN_H);
		}
		pack_fclose(pf);
	#endif
	}
        for (char **str=credits; *str; str++) {
        	start_ms = ms_count;
        	do {
                	check_sound();
                        spin_asteroid();
                	if (check_quit_intro()) goto exit_mmam_intro;
        	} while (ms_count - start_ms < 400);
        	start_ms = ms_count;
        	int oy = 7 * SCREEN_H / 8;
        	do {
        		t = (SCROLLON_MS - (ms_count - start_ms)) / SCROLLON_MS;
                	if (t < 0.) t = 0.;
                	int ox = (int)(floor((SCREEN_W / 2.) + ( SCREEN_W  * t * dir_flipflop)));
                        stretch_blit(bmp, buffer, 0,0, bmp->w, bmp->h, 0,0, SCREEN_W, SCREEN_H);
                        textout_centre_ex(buffer, &font_bold_plus, *str, ox, oy, White, -1);
                        blit(buffer, screen, 0,0,0,0, SCREEN_W, SCREEN_H);
			check_sound();
			spin_asteroid();
                	if (check_quit_intro()) goto exit_mmam_intro;

        	} while (t > 0.);
        	start_ms = ms_count;
        	do {
                	check_sound();
                	spin_asteroid();
                	if (check_quit_intro()) goto exit_mmam_intro;
        	} while (ms_count - start_ms < 1200);
        	start_ms = ms_count;
        	do {
        		t = 1. - ((SCROLLON_MS - (ms_count - start_ms)) / SCROLLON_MS);
                        int ox = (int)(floor((SCREEN_W / 2.) + ( SCREEN_W * t * dir_flipflop)));
                        stretch_blit(bmp, buffer, 0,0, bmp->w, bmp->h, 0,0, SCREEN_W, SCREEN_H);
                        textout_centre_ex(buffer, &font_bold_plus, *str, ox, oy, White, -1);
                        blit(buffer, screen, 0,0,0,0, SCREEN_W, SCREEN_H);
			check_sound();
			spin_asteroid();
      	                if (check_quit_intro()) goto exit_mmam_intro;
        	} while (t < 1.);
                dir_flipflop *= -1.0;
        }
        start_ms = ms_count;
        do {
                t = (FADE_MS - (ms_count - start_ms)) / FADE_MS;
                if (t < 0.) t = 0.;
                stretch_blit(bmp, buffer, 0,0, bmp->w, bmp->h, 0,0, SCREEN_W, SCREEN_H);
               	set_trans_blender(0,0,0,255 - (int)floor(254. * t));
                draw_trans_sprite(buffer,mmam_splash,0,0);
               	if (t < 0.) t = 0.;
		check_sound();
                if (check_quit_intro()) goto exit_mmam_intro;
                blit(buffer, screen, 0,0,0,0, SCREEN_W, SCREEN_H);
        } while (t > 0.);
exit_mmam_intro:
        blit(mmam_splash,buffer, 0,0,0,0, SCREEN_W, SCREEN_H);
	blit(mmam_splash,screen, 0,0,0,0, SCREEN_W, SCREEN_H);
	if (bmp) destroy_bitmap(bmp);
}


