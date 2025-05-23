#ifndef _altheora_h
#define MOV_NOEXIT 1
typedef struct frame_time {int first, ms; };
int play_movie(char *fn, frame_time *tlist = NULL, int flags = 0);
#endif
