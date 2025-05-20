#ifndef _FORTIFY2_H_
#define _FORTIFY2_H_

#ifdef FORTIFY

#include "fortify.h"

void fortify_init(void);
void fortify_exit(void);

extern int always_close_fortify_file, fortify_write_traceback;
#ifndef ALLEGRO_H
#warning include allegro.h first
#endif
#undef ustrdup
//#define ustrdup(s)   (_ustrdup((s), malloc))
#define ustrdup(s)   (ustrcpy((char *)malloc(ustrsizez(s)), s))

#endif

#endif
