#ifndef _GWSOUND_H_
#define _GWSOUND_H_  1
#include "sounds.h"

#define GM_DEFAULT 0
#define GM_TITLE 1
#define GM_CREDITS 2
#define GM_WIN 3
#define GM_LOSE 4
#define GM_FIGHT 5
#define GM_COUNT 6

extern char songtab[GM_COUNT][45];
extern DATAFILE *sounds;

void init_sound();
void set_mood(int mood);
void do_sound(int idx, int x, int y, int amp);
void reset_music();
void check_sound();

void start_gibberish(int idx, int nc);
void end_gibberish();
int gibberish_done();

#endif
