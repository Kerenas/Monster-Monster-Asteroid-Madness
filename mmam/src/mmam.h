#ifndef _mmam_h
#define _mmam_h 1
#include <allegro.h>
#ifdef _WIN32
#  include <winalleg.h>
#endif
#include <list>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#ifndef STEAL_PRINTF
#   undef printf
#   define printf gamelog->add
#endif
#define TURNS_PER_SEC 20
using namespace std;
extern volatile int ms_count, fps, frame_count, page_flip, forced_redraw_flag;
extern int game_turn;
extern int gamedone;
#include "pmask.h"
#include "sprites.h"
#include "sounds.h"
#include "sprite_patch.h"
#include "fonts.h"
#include "albgi.h"
#include "hud.h"
#include "trader.h"
#include "cage.h"
#include "event.h"
#include "scripts.h"
#include "xtrig.h"



#define WP_STD      0x00000
#define WP_SLEEP    0x00001
#define WP_SKATR    0x00002
#define WP_FLASH    0x00004
#define WP_QUAKE    0x00008
#define WP_INTERNAL 0x00010
#define WP_SPIN     0x00020
#define WP_PSY      0x00040
#define WP_EXPL     0x00080
#define WP_FIRE     0x00100
#define WP_CHEM     0x00200
#define WP_SPORE    0x00400
#define WP_GUIDED   0x00800
#define WP_SOLID    0x01000


extern volatile int ms_count;
void check_screen_shot();
void game_rest(int ms);
void reinit_graphics();
int askyn(char *s);

typedef struct orick_clan_t {int spr_big; char *name; int *color; };

typedef struct aspath {
  int idx, ms, wx, wy;
} aspath;

typedef struct weap_t {
  struct ivalu_t *t;
  int count;
  int rl_time, a_time;
} weap_t;

typedef struct builder_table {struct monst_base *fr, *to; int time;};

#include "qfe.h"
#include "alignment.h"
#include "monster.h"
#include "asteroid.h"
#include "particle.h"
#include "blood.h"
#include "input.h"
#include "dsound.h"

#include "monstbase.h"

extern asteroid_t *ast, *home_ast, *remote_ast;
extern trader_t *trader;
extern hud_t *hud;
extern monst_t *ship, *jumpgate_s, *mainbase, *thehuman;
extern DATAFILE *sprites;
extern BITMAP *buffer, *page1, *page2, *active_page;
extern BITMAP *mmam_splash, *mmam_play_arrow;
extern BITMAP *tile_map[16 * NUM_SURFACES];
extern int netship_spr_big;
extern aspath  netship_spr_chill[];
extern orick_clan_t orick_clan[];

#include "gamelog.h"

// sticky tape
#define SPR_ARROW_BMP 0
#define MAKE_DIRTY (page_flip ? 3 : 2)
#define SHIP_NOISE(noise) do_sound(SND_ ## noise, (int)(ship ? ship->x : 0), (int)(ship ? ship->y : 0), 255);
#define TILE_SIZE 32
#define vram_mmam_splash mmam_splash
#define REBUILD_COST 30000
#define ON_FIRE_TIME 7000
#define TURRET_TR 180
#endif


