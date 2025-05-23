#ifndef pmask_H
#define pmask_H

/*
This is the Pixel MASK library, which does pixel-perfect collisions using
bit masks.

There are several configuration options here, hope they aren't too
confusing.
*/

#define PMASK_VERSION          "5-ejkmod"
#define USE_ALLEGRO

struct BITMAP;
#define pmask_USE_INLINE

//MASK_WORD_TYPE and MASK_WORD_BITBITS can be changed for your platform

//MASK_WORD_TYPE should be the largest fast integer type available
#define MASK_WORD_TYPE unsigned long int

//MASK_WORD_BITBITS should be the log base 2
//of the number of bits in MASK_WORD_TYPE
//e.g. 4 for 16-bit ints, 5 for 32-bit ints, 6 for 64-bit ints
//don't worry about setting it incorrectly
//you'll get a compile error if you do, not a run-time error
#define MASK_WORD_BITBITS 5


//if SINGLE_MEMORY_BLOCK is defined
//then each mask will be allocated as
//only a single memory block.
//this means that masks will (in theory)
//be ever-so-slightly faster and more memory efficient
//however, if in single memory block mode
//then the masks are variable-sized
//so you can not use an array of them
//#define MASK_SINGLE_MEMORY_BLOCK

//trying to make portable const-ness easier:
#ifndef CONST
#	define CONST const

#endif

//This is the bounding box collision detection macro.
//It is a general purpose macro. Pass it the coordinates of one rectangle and the width and
//height of it, then pass the coordinates of a second rectangle and the width and height of
//it. It will return 0 if theres not collision or 1 if there is a collision between the
//rectangles (the ectangles overlap).
//This macro works looking for out of range values, if some value is out of
//range it returns 0, if all the values are in range it returns true.
#define check_bb_collision_general(x1,y1,w1,h1,x2,y2,w2,h2) (!( ((x1)>=(x2)+(w2)) || ((x2)>=(x1)+(w1)) || ((y1)>=(y2)+(h2)) || ((y2)>=(y1)+(h1)) ))
#define check_bb_collision(mask1,mask2,x1,y1,x2,y2) check_bb_collision_general(x1,y1,mask1->w,mask1->h,x2,y2,mask2->w,mask2->h)


typedef struct PMASK {
	BITMAP *src; float a, dw;
	BITMAP *osrc; float oa, odw;
	short int w;//width
	short int h;//height
#	ifdef MASK_SINGLE_MEMORY_BLOCK
		MASK_WORD_TYPE mask[1];//mask data (single memory block)
#	else
		MASK_WORD_TYPE *mask;//mask data (pointer at second memory block)
#	endif
} PMASK;

void install_pmask(); //sets up library

int _check_pmask_collision(struct PMASK *mask1, struct PMASK *mask2, int dx, int dy); //checks for collision (0 = no collision, 1 = collision)
int check_pmask_collision(struct PMASK *mask1, struct PMASK *mask2, int x1, int y1, int x2, int y2); //checks for collision (0 = no collision, 1 = collision)

//note: result[0] is set to the X coordinate of the collision, result[1] to the Y coordinate
int _check_pmask_collision_position(struct PMASK *mask1, struct PMASK *mask2, int dx, int dy, int *result);
int check_pmask_collision_position(struct PMASK *mask1, struct PMASK *mask2, int x1, int y1, int x2, int y2, int *result);

int get_pmask_pixel(CONST struct PMASK *mask, int x, int y) ; //returns 0 if mask is clear at those coordinates, 1 if not clear
void set_pmask_pixel(struct PMASK *mask, int x, int y, int value) ;//makes mask clear at those coordinates if value is zero, others makes mask not-clear at those coordinates

void init_pmask        (struct PMASK *mask, int w, int h); //initializes a PMASK
void pmask_load_pixels (struct PMASK *mask, void *pixels, int pitch, int bytes_per_pixel, int trans_color);//loads a pmask with pixel data from memory
void pmask_load_func   (struct PMASK *mask, int x, int y, void *surface, int trans_color, int (*func)(void*,int,int));//loads a pmask with pixel data from a function
void deinit_pmask(PMASK *mask);//de-initializes a pmask

PMASK *create_pmask  (int w, int h);//creates a new pmask and initializes it
void destroy_pmask(struct PMASK *mask);//destroys a pmask created by create_pmask

#if defined USE_ALLEGRO
	void init_allegro_pmask(struct PMASK *mask, struct BITMAP *sprite);
	PMASK *create_allegro_pmask(struct BITMAP *sprite);
	void draw_allegro_pmask(CONST struct PMASK *mask, struct BITMAP *destination, int x, int y, int color) ;
	void draw_allegro_pmask_stretch(CONST struct PMASK *mask, struct BITMAP *destination, int x, int y, int w, int h, int color) ;
	void reinit_pmask(struct PMASK *pm);
	void mark_pmask(struct PMASK *pm, struct BITMAP *bm, float a, float dw);
#endif


#define MASK_WORD_SIZE sizeof(MASK_WORD_TYPE)
#define MASK_WORD_BITS (MASK_WORD_SIZE*8)

#if defined pmask_USE_INLINED
inline int _get_pmask_pixel(struct PMASK *mask, int x, int y) {
	//no bounds checking
	return 1 & (mask->mask[(mask->h * (x >> MASK_WORD_BITBITS)) + y] >> (x & (MASK_WORD_BITS-1)));
}
inline void _set_pmask_pixel(struct PMASK *mask, int x, int y, int value) {
	//no bounds checking
	if (value) {
		mask->mask[(mask->h * (x >> MASK_WORD_BITBITS)) + y] |= 1 << (x & (MASK_WORD_BITS-1));
	} else {
		mask->mask[(mask->h * (x >> MASK_WORD_BITBITS)) + y] &=~(1 << (x & (MASK_WORD_BITS-1)));
	}
}
#else
#	define _get_pmask_pixel get_pmask_pixel
#	define _set_pmask_pixel set_pmask_pixel
#endif


#endif          /* ifndef pmask_H */

