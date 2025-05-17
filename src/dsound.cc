#define NO_MIDI
#define USE_ALOGG
#include "config.h"
#include "mmam.h"
#ifdef USE_DUMB
	#include <aldumb.h>
#endif
#ifdef USE_DFIM_OGG
	//#include <dfim.h>
	#define USE_OGG
#endif
#ifdef USE_ALOGG
	#include <alogg/aloggint.h>
	#include <alogg/alogg.h>
	#define USE_OGG
#endif
/*
#include <allegro.h>
#ifdef _WIN32
	#include <winalleg.h>
	#include <windows.h>
#else
	#include <unistd.h>
        #include <fcntl.h>
#endif
#include <errno.h>
*/
#include "dsound.h"

DATAFILE *sounds;
int sound_driver;
#ifndef NO_MIDI
	static struct MIDI *the_song = NULL;
	static int midi_player;
	#define CHECK_MIDI MIDI_AUTODETECT
	#define MIDI_IS_OK 1
#else
	#define CHECK_MIDI MIDI_NONE
	#define MIDI_IS_OK 0
#endif
#ifdef USE_DUMB
	static DUH *the_song_duh = NULL;
	static AL_DUH_PLAYER *dumb_player = NULL;
	#define MOD_IS_OK 1
#else
	#define MOD_IS_OK 0
#endif
char songtab[GM_COUNT][45];
int num_song, num_alloc_songtab;
int cur_mood, next_mood, was_playing_song;
#define sound_on enable_sound
#define music_allowed enable_music
#ifdef USE_ALOGG
struct alogg_stream *the_alogg_stream;
#endif

void init_sound() {
	set_volume_per_voice(0);
	if ( install_sound(DIGI_AUTODETECT, CHECK_MIDI, "") == 0 ) {
		printf((char *)"Sound Detected...\n");
	#ifdef USE_DUMB
		atexit(&dumb_exit);
		dumb_register_stdfiles();
		// dumb_register_packfiles();
		// dumb_resampling_quality = 4;
		dumb_it_max_to_mix =  128;
		// set_volume_per_voice(0); ??? look later.
	#endif
	#ifdef USE_DFIM_OGG
	    	dfim_install_soundplayers(32, 0); // allocates up to 32 soundplayers in a polling context.	
	#endif
	#ifdef USE_ALOGG
		alogg_init();
	#endif
	} else {
		enable_sound = enable_music = 0;
		allegro_message("Error initialising sound system\n%s\n", allegro_error);
		return;
	}
	char fn[200];
	snprintf(fn, sizeof(fn), "%s/sounds.dat", data_dir);
	sounds = load_datafile(fn);
	if (!sounds) {
		printf((char *)"Unable to open \"%s\" continuing without sounds.\n",fn);
		enable_sound = 0;
		return;
	} else {
		for (int c = 0; ; c++) if (sounds[c].type == DAT_END) {
				c--;
				if (c == SND_COUNT) printf((char *)"Sounds loaded from \"%s\".\n",fn);
				else {
					allegro_message("Sound file \"%s\" apears to belong to be the wrong version.\n"
						"Count = %i (should be %i) Continuing without sounds.\n",fn,c,SND_COUNT
					);
					unload_datafile(sounds); sounds=NULL;
					enable_sound = 0;
					return;
				}
				break;
		}
	}
	if (music_dir[0] == 0) {
		printf((char *)"No music dir. Continuing without custom music.\n");
		return;
	}
	sound_on = 1;
}

#ifdef USE_DUMB
static int my_dumb_song_over = 0;

static int my_endsong_callback(void *data) {
	my_dumb_song_over = 1;
	printf((char *)"song died!\n");
	return 0;
}
#endif

static int load_song(char *full_fn) {
	printf((char *)"load_song: %s\n",full_fn);
	int load_ok = 0;
	#ifdef USE_DUMB
		DUH *duh = NULL;
		AL_DUH_PLAYER *dp = NULL;
		al_stop_duh(dumb_player);
		unload_duh(the_song_duh);
		dumb_player  = NULL;
		the_song_duh = NULL;
	#endif
	#ifndef NO_MIDI
		if (the_song) {
			destroy_midi(the_song); the_song = NULL;
		}
	#endif
	#ifndef NO_MIDI
		MIDI *l_song = NULL;
		if ( ( l_song = load_midi(full_fn) ) ) {
			printf((char *)"load midi(%s).\n",full_fn);
			play_midi(l_song, 0);
			load_ok = 1;
		}
		#ifdef USE_DUMB
		if (load_ok == 0)
		#endif
	#endif
	#ifdef USE_DUMB
		{
			load_ok = (
				(duh = dumb_load_xm(full_fn))  ? 1 :
				(duh = dumb_load_it(full_fn))  ? 1 :
				(duh = dumb_load_s3m(full_fn)) ? 1 :
				(duh = dumb_load_mod(full_fn)) ? 1 : 0
			);
			if (load_ok) {
     				printf((char *)"load Xm(%s).\n",full_fn);
				if ( (dp = al_start_duh(duh, 1, 0, 1.0f, music_buffer_size,  music_freq )) ) {
					DUH_SIGRENDERER *sr = al_duh_get_sigrenderer(dp);
					DUMB_IT_SIGRENDERER *itsr = duh_get_it_sigrenderer(sr);
					dumb_it_set_loop_callback(itsr, &my_endsong_callback, NULL);
      					dumb_it_set_xm_speed_zero_callback(itsr, &my_endsong_callback, NULL);
					my_dumb_song_over = 0;
                                }
			}
		}
	#endif
	if (!load_ok) printf((char *)"unable to load music file(%s).\n",full_fn);
	else {
		#ifdef USE_DUMB
			dumb_player = dp;
			the_song_duh = duh;
		#endif
		#ifndef NO_MIDI
			midi_player = ((the_song = l_song) != NULL);
		#endif
	}
        return ( (load_ok));
}

void set_mood(int mood) {
	if (!sound_on) return;
	if ((mood < 0) || (mood >= GM_COUNT)) return;
	
	if (music_vol == 0 || music_allowed == 0) return;
	if (cur_mood == mood && was_playing_song ) return;
	printf((char *)"set_mood %i\n",mood);
	char full_fn[200];
	if (mood == GM_FIGHT) {
		// make scan music_dir later
		snprintf(full_fn, sizeof full_fn,"%s/%s", data_dir, songtab[0]);
	} else {
		snprintf(full_fn, sizeof full_fn,"%s/%s", data_dir, songtab[mood]);
	}
	if (!load_song(full_fn)) {
		snprintf(full_fn, sizeof full_fn,"%s/%s", data_dir, songtab[GM_DEFAULT]);
		load_song(full_fn);
	}
	next_mood = mood;
	cur_mood = mood;
}

void do_sound(int idx, int x, int y, int amp) {
	if (idx <0 || idx >= SND_COUNT || !sound_on || !sounds || ai_test) return;
	int dx = (ast ? (int)floor(127. + ((x - (ast->pw / 2))/ast->pw) * 180.) : 127);//[37-217]
        if (ship) {
        	float d = dist(x,y, ship->x, ship->y);
        	if (d > 100) amp -= (int)(255. * ((d-100.) / 600.));
        }
        if (amp <= 10) return;
        if (amp > 255) amp = 255;
        if (sounds[idx].type == DAT_ID('O','G','G',' ')) {
			#ifdef USE_DFIM_OGG
				if (sounds[idx].size < 80000) {
					dfim_convert_ogg_to_sample(&sounds[idx].dat, sounds[idx].size);
					sounds[idx].type = DAT_SAMPLE;
					play_sample((SAMPLE *) sounds[idx].dat, amp, dx, 1000, FALSE);
				} else dfim_create_mem_ogg_soundplayer(sounds[idx].dat, sounds[idx].size, 4096, 1000, amp, dx);
				printf((char *)"play ogg\n");
			#else
				#ifdef USE_ALOGG
		 		if (sounds[idx].size < 80000) {
					sounds[idx].type = DAT_OGG_VORBIS;  // workaround for alogg 
					SAMPLE *sample = alogg_create_sample(&sounds[idx]);
   					free(sounds[idx].dat);
					sounds[idx].dat = sample;
					sounds[idx].type = DAT_SAMPLE;
					play_sample((SAMPLE *) sounds[idx].dat, amp, dx, 1000, FALSE);
				} else if (!the_alogg_stream) the_alogg_stream = alogg_start_streaming_datafile(&sounds[idx], 16000);
      
				#else
					printf((char *)"cannot play ogg!\n");        	
				#endif			
			#endif
        } else play_sample((SAMPLE *) sounds[idx].dat, amp, dx, 1000, FALSE);
}

#ifdef USE_OGG
static int gibberish_sp = -1;
static int gibberish_idx = -1;
static int gibberish_nc = 1;

static void check_gibberish() {
	#ifdef USE_DFIM_OGG
	if (!dfim_alive_soundplayer(gibberish_sp) && gibberish_idx >= 0) {
        	start_gibberish(gibberish_idx, gibberish_nc);
   	}
   	#else
   	#endif
}
void start_gibberish(int idx, int nc) {
    if (idx < 0 || !gibberish_done() || !sounds) return;
  	gibberish_idx = idx; gibberish_nc = nc;
  	idx += irand(nc);
  	#ifdef USE_DFIM_OGG
  	gibberish_sp = dfim_create_mem_ogg_soundplayer(sounds[idx].dat, sounds[idx].size, 4096, 950 + irand(100), 40 + irand(10), 128);
  	#else
  	#endif
  	printf("Gib %i\n",idx);
}

void end_gibberish() {
	gibberish_idx = -1;
}

int gibberish_done() {
	#ifdef USE_DFIM_OGG
	return !dfim_alive_soundplayer(gibberish_sp);
	#else
	return 1;
	#endif
}

#else
static void check_gibberish() {}
void start_gibberish(int idx, int nc) {}
void end_gibberish() {}
int gibberish_done() { return 1;}
#endif

static int old_music_volume;

void reset_music() {
	// forces music to reset.
        int mood = cur_mood; cur_mood = -1; set_mood( mood );
}

void check_sound() {
	if (!sound_on) return;
        int eff_v = (music_allowed ? music_vol : 0 );
        #ifdef USE_OGG
        check_gibberish();
			#ifdef USE_DFIM_OGG
			dfim_poll_soundplayers();
			#else
			if (!the_alogg_stream ? 0 : alogg_update_streaming(the_alogg_stream) == 0) {
			    alogg_stop_streaming(the_alogg_stream);
			    the_alogg_stream = NULL;	
			}      
			#endif        
        #endif
	if (old_music_volume != eff_v) {
		#ifdef USE_DUMB
			if (!old_music_volume) set_mood(next_mood);
			if (eff_v) {
				al_duh_set_volume(dumb_player, eff_v / 255.);
			} else {
				al_stop_duh(dumb_player);
				unload_duh(the_song_duh);
				dumb_player  = NULL;
				the_song_duh = NULL;
			}
		#endif
		set_volume(255, eff_v); // sound_vol is controlled by rend funcs
		printf("volume change %i => %i.\n", old_music_volume, eff_v);
		old_music_volume = eff_v;
        }
	if (0
	#ifndef NO_MIDI
		|| (midi_player && midi_pos < 0 )
	#endif
	#ifdef USE_DUMB
		|| ((dumb_player && eff_v ) && (al_poll_duh(dumb_player) != 0 || my_dumb_song_over) )
	#endif
	) {

		printf("song ended!\n");
		if (was_playing_song) {
			was_playing_song = 0;
			set_mood(next_mood);
		}
	} else was_playing_song = 1;
}


