/* internal (ugly) random number generator funcs */
#define UGLY_RAND_MAX 0xffff
extern int ugly_randseed;
void ugly_reseed(int i);
int ugly_rand();

/* system random number generator funcs */
int irand(int i);
float frand();
float frand(float mv);
int deviate(int i); /* picks a number usually close to i */

void do_randomize(); /* sets both internal and system randseeds using the current time. */

