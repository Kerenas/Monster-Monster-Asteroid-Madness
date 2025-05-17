#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#ifdef _WIN32
#  include <windows.h>
#endif
#include "rand.h"

int ugly_randseed;

void ugly_reseed(int i) {
	ugly_randseed = 0x2f431287+i;
}

int ugly_rand() {
	ugly_randseed *= 0x73; ugly_randseed += 0x534e3; ugly_randseed += ugly_randseed >> 14;
	return ((ugly_randseed >> 8) & 0xffff);
}

float frand() {
	return ((rand())/(RAND_MAX+1.0));
}


float frand(float mv) {
	return ((mv * rand())/(RAND_MAX+1.0));
}

int irand(int i) {
	return (int)(i <= 0 ? i : rand() % i);
}


void do_randomize() {
  #ifdef _WIN32
	SYSTEMTIME st; GetSystemTime(&st);   srand((unsigned int)( st.wSecond << 16 + st.wMilliseconds));
  #else
        timeval rseed; gettimeofday(&rseed, NULL); srandom(rseed.tv_usec);
  #endif
  ugly_randseed = rand();
}

int deviate(int i) {
	float f = frand() * frand() * (i / 2.);
        return (int)(irand(1) ? i + f : i - f);
}

