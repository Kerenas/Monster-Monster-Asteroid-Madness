#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pmask.h"
#ifdef USE_ALLEGRO
#	include <allegro.h>
#endif
#include "bubbles.h"


#ifdef main
#	undef main
#endif


int get_pmask_pixel(CONST struct PMASK *mask, int x, int y) {
	return 1 & (mask->mask[(mask->h * (x >> MASK_WORD_BITBITS)) + y] >> (x & (MASK_WORD_BITS-1)));
}
void set_pmask_pixel(struct PMASK *mask, int x, int y, int value) {
	if (value) {
		mask->mask[(mask->h * (x >> MASK_WORD_BITBITS)) + y] |= 1 << (x & (MASK_WORD_BITS-1));
	} else {
		mask->mask[(mask->h * (x >> MASK_WORD_BITBITS)) + y] &=~(1 << (x & (MASK_WORD_BITS-1)));
	}
}


#define COMPILE_TIME_ASSERT(condition) {typedef char _compile_time_assert__[(condition) ? 1 : -1];}
void install_pmask() {
	COMPILE_TIME_ASSERT((1 << MASK_WORD_BITBITS) == MASK_WORD_BITS);
	return;
}

void init_pmask (struct PMASK *mask, int w, int h)
{
	int words, total_words, x, error = 0;

	mask->w = w;
	mask->h = h;
	if ((mask->w != w) || (mask->h != h)) {
		mask->w = mask->h = 0;
#		ifndef MASK_SINGLE_MEMORY_BLOCK
			mask->mask = NULL;
#		endif
		return;
	}

	words = 1 + ((w-1) >> MASK_WORD_BITBITS);

	total_words = words * h;

#	ifdef MASK_SINGLE_MEMORY_BLOCK

#	else
		mask->mask = (MASK_WORD_TYPE *) malloc(
			MASK_WORD_SIZE * total_words);
		if (!mask->mask) {
			mask->w = mask->h = 0;
			return;
		}
#	endif

	//Now we initialize some of the fields of the structure...
	mask->w = w;
	mask->h = h;

#	ifdef CLEAR_pmask
		//Now we have a proper mask structure allocated and mostly initialized, but the mask data has garbage! We have to initialize it to 0's:
		for(x=0; x < total_words; x+=1) {
			maskt->mask[x] = 0;
		}
#	else
		//only clear right hand edge if CLEAR_MASK is not defined
		for(x=total_words-h; x < total_words; x+=1) {
			mask->mask[x] = 0;
		}
#	endif
	return;
}

void deinit_pmask(struct PMASK *mask) {
	mask->w = 0;
	mask->h = 0;
#	ifndef MASK_SINGLE_MEMORY_BLOCK
		if (mask->mask) free(mask->mask);
		mask->mask = NULL;
#	endif
	return;
}

void destroy_pmask(struct PMASK *mask) {
	deinit_pmask(mask);
	free(mask);
	return;
}

PMASK *create_pmask (int w, int h) {
	struct PMASK *maskout;

#	ifdef MASK_SINGLE_MEMORY_BLOCK
		int words, total_words;
		words = 1 + ((w-1) >> MASK_WORD_BITBITS);
		total_words = words * h;
		maskout = (PMASK *) malloc(
			sizeof(PMASK) +
			MASK_WORD_SIZE * total_words );
		if(!maskout) return NULL;
#	else
		maskout = (PMASK *) malloc(sizeof(PMASK));
		if(!maskout) return NULL;
#	endif

	init_pmask(maskout, w, h);

#	ifndef MASK_SINGLE_MEMORY_BLOCK
		if (!maskout->mask) {
			destroy_pmask(maskout);
			return NULL;
		}
#	endif

	return maskout;
}

void pmask_load_func (struct PMASK *mask, int _x, int _y, void *surface, int trans_color, int (*func)(void*,int,int))
{
	int words, x, y, x2, w, h;
	if(!mask) return;

	w = mask->w;
	h = mask->h;

	words = 1 + ((w-1) >> MASK_WORD_BITBITS);

	//Now we have to create the bit mask array for the sprite:
	for(x=0; x < words; x+=1) {
		for(y=0; y < h; y+=1) {
			MASK_WORD_TYPE m = 0;
			for (x2=MASK_WORD_BITS-1; x2 >= 0; x2-=1) {
				int x3 = (x << MASK_WORD_BITBITS) + x2 + _x;
				m <<= 1;
				if ( x3 < w ) {
					if ( func(surface, x3, y+_y) != trans_color ) {
						m |= 1;
					}
				}
			}
			mask->mask[y+x*h] = m;
		}
	}
	return;
}

void pmask_load_pixels (struct PMASK *mask, void *pixels, int pitch, int bytes_per_pixel, int trans_color)
{
	int words, x, y, x2, w, h;
	if(!mask) return;

	w = mask->w;
	h = mask->h;

	words = 1 + ((w-1) >> MASK_WORD_BITBITS);

	//Now we have to create the bit mask array for the sprite:
	for(x=0; x < words; x+=1) {
		for(y=0; y < h; y+=1) {
			MASK_WORD_TYPE m = 0;
			for (x2=MASK_WORD_BITS-1; x2 >= 0; x2-=1) {
				int x3 = (x << MASK_WORD_BITBITS) + x2;
				m <<= 1;
				if ( x3<w ) {
					//beware of endianness!!!!!!!!!!
					if ( memcmp(((char*)pixels) + x3 * bytes_per_pixel + y * pitch, &trans_color, bytes_per_pixel) == 0 ) {
						m |= 1;
					}
				}
			}
			mask->mask[y+x*h] = m;
		}
	}
	return;
}

#ifdef USE_ALLEGRO
	typedef char pmask_int_8;
	typedef short int pmask_int_16;
	typedef int pmask_int_32;
	static void load_allegro_pmask(PMASK *mask, BITMAP *sprite) {
		pmask_load_func (mask, 0, 0, sprite, bitmap_mask_color(sprite), (int (*)(void*,int,int))getpixel);
	}
	void init_allegro_pmask(struct PMASK *mask, struct BITMAP *sprite) {
		init_pmask(mask, sprite->w, sprite->h);
		load_allegro_pmask(mask, sprite);
	}
	PMASK *create_allegro_pmask(struct BITMAP *sprite) {
		PMASK *ret;
		ret = create_pmask(sprite->w, sprite->h);
		if (!ret) return NULL;
		load_allegro_pmask(ret, sprite);
		return ret;
	}
	void draw_allegro_pmask(CONST PMASK *mask, BITMAP *destination, int dx, int dy, int color) {
/*		int mx, my;
		for (my = 0; my < mask->h; my += 1) {
			for (mx = 0; mx < mask->w; mx += 1) {
				if (_get_pmask_pixel(mask, mx, my))
					putpixel(destination, x+mx, y+my, color);
			}
		}*/
		int sx, sy, x, y, w, h;
		if (dx >= destination->w) return;
		if (dy >= destination->h) return;
		w = mask->w;
		h = mask->h;
		if (dx >= 0) sx = 0; else {
			sx = -dx;
			w += dx;
			dx = 0;
		}
		if (dy >= 0) sy = 0; else {
			sy = -dy;
			h += dy;
			dy = 0;
		}
		if (sx > mask->w) return;
		if (sy > mask->h) return;
		if (w > destination->w - dx) w = destination->w - dx;
		if (h > destination->h - dy) h = destination->h - dy;
		if (is_memory_bitmap(destination)) {
#			define BLAH_WRITE1(bpp) {for (y = 0; y < h; y += 1) 
#			define BLAH_WRITE2(bpp) {pmask_int_ ## bpp *line = &(((pmask_int_ ## bpp *)destination->line[y])[dx]);
#			define BLAH_WRITE3(bpp) {for (x = 0; x < w; x += 1)
#			define BLAH_WRITE4(bpp) {if(_get_pmask_pixel(mask, sx+x, sy+y)) line[x] = color;}
#			define BLAH_WRITE5(bpp) }}}
#			define BLAH_WRITE(bpp) BLAH_WRITE1(bpp)BLAH_WRITE2(bpp)BLAH_WRITE3(bpp)BLAH_WRITE4(bpp)BLAH_WRITE5(bpp)
			switch (bitmap_color_depth(destination)) {
				case 8: BLAH_WRITE(8) break;
				case 15:
				case 16: BLAH_WRITE(16) break;
				case 32: BLAH_WRITE(32) break;
				default: goto handle_special_case;
			}
#			undef BLAH_WRITE1
#			undef BLAH_WRITE2
#			undef BLAH_WRITE3
#			undef BLAH_WRITE4
#			undef BLAH_WRITE5
#			undef BLAH_WRITE
		}
		else {
#			define BLAH_WRITE1(bpp) {bmp_select(destination); for (y = 0; y < h; y += 1) 
#			define BLAH_WRITE2(bpp) {unsigned long addr = bmp_write_line(destination, y+dy) + dx * ((bpp+7) / 8);
#			define BLAH_WRITE3(bpp) {for (x = 0; x < w; x += 1)
#			define BLAH_WRITE4(bpp) {if(_get_pmask_pixel(mask, sx+x, sy+y)) bmp_write ## bpp (addr + x*((bpp+7) / 8), color);}
#			define BLAH_WRITE5(bpp) }} bmp_unwrite_line(destination); }
#			define BLAH_WRITE(bpp) BLAH_WRITE1(bpp)BLAH_WRITE2(bpp)BLAH_WRITE3(bpp)BLAH_WRITE4(bpp)BLAH_WRITE5(bpp)
			switch (bitmap_color_depth(destination)) {
				case 8: BLAH_WRITE(8) break;
				case 15:
				case 16: BLAH_WRITE(16) break;
				case 32: BLAH_WRITE(32) break;
				default: goto handle_special_case;
			}
#			undef BLAH_WRITE1
#			undef BLAH_WRITE2
#			undef BLAH_WRITE3
#			undef BLAH_WRITE4
#			undef BLAH_WRITE5
#			undef BLAH_WRITE
		}
		return;
		handle_special_case: 
		{
			for (y = 0; y < h; y += 1) {
				for (x = 0; x < w; x += 1) {
					if (_get_pmask_pixel(mask, sx+x, sy+y)) {
						putpixel(destination, dx+x, dy+y, color);
					}
				}
			}
		}
		return;
	}
	void draw_allegro_pmask_stretch(CONST PMASK *mask, BITMAP *destination, int x, int y, int w, int h, int color) {
		int _x, _xmin, _y, _w, _h;
		int scale;
		if (y >= 0) _y = 0; else _y = -y;
		if (y + h <= destination->h) _h = h; else _h = destination->h - y;
		if (x >= 0) _xmin = 0; else _xmin = -x;
		if (x + w <= destination->w) _w = w; else _w = destination->w - x;
		scale = (mask->w << 16) / w;

		bmp_select(destination);
		switch (bitmap_color_depth(destination)) {
			case 8: {
				while (_y < _h) {
					int ty, tx;
					unsigned long addr = bmp_write_line(destination, y + _y) + x * sizeof(char);
					//unsigned long *dat = mask->sp_mask[_y * mask->h / h];
					ty = _y * mask->h / h;
					_x = _xmin;
					tx = _x * scale;
					while (_x < _w) {
						//if ( (dat[(tx>>21)] << ((tx>>16) & 31)) & 0x80000000UL )
						if ( _get_pmask_pixel(mask,tx>>16,ty) )
							bmp_write8(addr+_x*sizeof(char), color);
						tx += scale;
						_x += 1;
					}
					_y += 1;
				}
			}
			break;
			case 15: {
				while (_y < _h) {
					int ty, tx;
					unsigned long addr = bmp_write_line(destination, y + _y) + x * sizeof(short);
					ty = _y * mask->h / h;
					_x = _xmin;
					tx = _x * scale;
					while (_x < _w) {
						//if ( (dat[(tx>>21)] << ((tx>>16) & 31)) & 0x80000000UL )
						if ( _get_pmask_pixel(mask,tx>>16,ty) )
							bmp_write15(addr+_x*sizeof(short), color);
						tx += scale;
						_x += 1;
					}
					_y += 1;
				}
			}
			break;
			case 16: {
				while (_y < _h) {
					int ty, tx;
					unsigned long addr = bmp_write_line(destination, y + _y) + x * sizeof(short);
					ty = _y * mask->h / h;
					_x = _xmin;
					tx = _x * scale;
					while (_x < _w) {
						//if ( (dat[(tx>>21)] << ((tx>>16) & 31)) & 0x80000000UL )
						if ( _get_pmask_pixel(mask,tx>>16,ty) )
							bmp_write16(addr+_x*sizeof(short), color);
						tx += scale;
						_x += 1;
					}
					_y += 1;
				}
			}
			break;
			case 24: {
				while (_y < _h) {
					int ty, tx;
					unsigned long addr = bmp_write_line(destination, y + _y) + x * 3;
					ty = _y * mask->h / h;
					_x = _xmin;
					tx = _x * scale;
					while (_x < _w) {
						//if ( (dat[(tx>>21)] << ((tx>>16) & 31)) & 0x80000000UL )
						if ( _get_pmask_pixel(mask,tx>>16,ty) )
							bmp_write24(addr+_x*3, color);
						tx += scale;
						_x += 1;
					}
					_y += 1;
				}
			}
			break;
			case 32: {
				while (_y < _h) {
					int ty, tx;
					unsigned long addr = bmp_write_line(destination, y + _y) + x * sizeof(long);
					ty = _y * mask->h / h;
					//unsigned long *dat = mask->mask + ty;
					_x = _xmin;
					tx = _x * scale;
					while (_x < _w) {
						//if ( (dat[(tx>>21)] << ((tx>>16) & 31)) & 0x80000000UL )
						if ( _get_pmask_pixel(mask,tx>>16,ty) )
							bmp_write32(addr+_x*sizeof(long), color);
						tx += scale;
						_x += 1;
					}
					_y += 1;
				}
			}
			break;
		}
		bmp_unwrite_line(destination);
		return;
	}

#endif



int _check_pmask_collision(struct PMASK *mask1, struct PMASK *mask2, int dx, int dy1)
{
	MASK_WORD_TYPE *raw1, *end1;
	MASK_WORD_TYPE *raw2, *end2;
	int h1, h2, words1, words2;
	int dy2; //We will use these deltas...
	int py; //This will store the Y position...
	int maxh; //This will store the maximum height...
	int block1, block2;

	//First we do normal bounding box collision detection...
	if ( (dx >= mask1->w) || (dy1 > mask1->h) || (dx < -mask2->w) || (dy1 < -mask2->h) )
		return 0;

  	reinit_pmask(mask1);
	reinit_pmask(mask2);

	if (dx < 0) { //swap 1 & 2 to make sure mask1 is on the left
		PMASK *mtmp;
		dx = -dx;
		dy1 = -dy1;
		mtmp = mask1; mask1 = mask2; mask2 = mtmp;//swap masks
	}

	if(dy1<0) {
		dy2 = -dy1;
		dy1 = 0;
	} else {
		dy2 = 0;
	}

	block1 = dx>>MASK_WORD_BITBITS;
	block2 = 0;
	dx &= MASK_WORD_BITS-1;
	
	//This will calculate the maximum height that we will reach...
	h1 = mask1->h;
	h2 = mask2->h;
	if(h1-dy1 > h2-dy2) {
		maxh=h2-dy2-1;
	} else {
		maxh=h1-dy1-1;
	}

	words1 = 1 + ((mask1->w-1) >> MASK_WORD_BITBITS);
	words2 = 1 + ((mask2->w-1) >> MASK_WORD_BITBITS);
	block1 = block1 * h1 + dy1;
	block2 = block2 * h2 + dy2;

	raw1 = &mask1->mask[block1];
	end1 = &mask1->mask[words1 * h1];
	raw2 = &mask2->mask[block2];
	end2 = &mask2->mask[words2 * h2];

	if ( dx == 0 ) {//if perfectly aligned
		while((raw1<end1) && (raw2<end2) ) { //search horizantolly in the outer loop
			for(py=maxh;py>=0;py--) { //Search vertically
				if( raw1[py] & raw2[py] )
					return 1;
			}
			raw1 += h1;
			raw2 += h2;
		}
		return 0;
	}
	else {//if not perfectly aligned
		while( 1 ) { //search horizantolly in the outer loop
			for(py=maxh;py>=0;py--) { //Search vertically
				if( (raw1[py] >> dx) & raw2[py] ) return 1;
			}
			raw1 += h1;
			if (raw1>=end1) return 0;
			dx = MASK_WORD_BITS - dx;
			//dx = -dx & (MASK_WORD_BITS - 1);
			for(py=maxh;py>=0;py--) { //Search vertically
				if( raw1[py] & (raw2[py] >> dx) ) return 1;
			}
			raw2 += h2;
			if (raw2>=end2) return 0;
			dx = MASK_WORD_BITS - dx;
			//dx = -dx & (MASK_WORD_BITS - 1);
		}
	}
}

int bsf ( int a ) {
	int r = 0;
	if (!a) return -1;
	if (!(a & 0xFFFF)) {
		r += 16;
		a >>= 16;
	}
	if (!(a & 0xFF)) {
		r += 8;
		a >>= 8;
	}
	if (!(a & 0xF)) {
		r += 4;
		a >>= 4;
	}
	if (!(a & 0x3)) {
		r += 2;
		a >>= 2;
	}
	if (!(a & 0x1)) {
		r += 1;
		a >>= 1;
	}
	return r;
}

int _check_pmask_collision_position(PMASK *mask1, PMASK *mask2, int dx, int dy1, int *result) {
        if (!mask1 || !mask2) return 0;
	MASK_WORD_TYPE *raw1, *end1;
	MASK_WORD_TYPE *raw2, *end2;
	int h1, h2, words1, words2;
	int dy2; //We will use these deltas...
	int py; //This will store the Y position...
	int maxh; //This will store the maximum height...
	int block1, block2;
	int rx = 0, odx = dx;

	//First we do normal bounding box collision detection...
	if ( (dx >= mask1->w) || (dy1 > mask1->h) || (dx < -mask2->w) || (dy1 < -mask2->h) ) return 0;

	reinit_pmask(mask1);
	reinit_pmask(mask2);

	if (dx < 0) { //swap 1 & 2 to make sure mask1 is on the left
		PMASK *mtmp;
		dx = -dx;
		dy1 = -dy1;
		mtmp = mask1; mask1 = mask2; mask2 = mtmp;//swap masks
	}

	if(dy1<0) {
		dy2 = -dy1;
		dy1 = 0;
	} else {
		dy2 = 0;
	}

	block1 = dx>>MASK_WORD_BITBITS;
	block2 = 0;
	dx &= MASK_WORD_BITS-1;

	//This will calculate the maximum height that we will reach...
	h1 = mask1->h;
	h2 = mask2->h;
	if(h1-dy1 > h2-dy2) {
		maxh=h2-dy2-1;
	} else {
		maxh=h1-dy1-1;
	}

	words1 = 1 + ((mask1->w-1) >> MASK_WORD_BITBITS);
	words2 = 1 + ((mask2->w-1) >> MASK_WORD_BITBITS);
	block1 = block1 * h1 + dy1;
	block2 = block2 * h2 + dy2;

	raw1 = &mask1->mask[block1];
	end1 = &mask1->mask[words1 * h1];
	raw2 = &mask2->mask[block2];
	end2 = &mask2->mask[words2 * h2];


	if ( dx == 0 ) {//if perfectly aligned
		while((raw1<end1) && (raw2<end2) ) { //search horizantolly in the outer loop
			for(py=maxh;py>=0;py--) { //Search vertically
				if( raw1[py] & raw2[py] ) {
					rx += bsf(raw1[py] & raw2[py]);
					goto found;
				}
			}
			rx += MASK_WORD_BITS;
			raw1 += h1;
			raw2 += h2;
		}
		return 0;
	}
	else {//if not perfectly aligned
		while( 1 ) { //search horizantolly in the outer loop
			for(py=maxh;py>=0;py--) { //Search vertically
				if( (raw1[py] >> dx) & raw2[py] ) {
					rx += bsf((raw1[py] >> dx) & raw2[py]);
					goto found;
				}
			}
			raw1 += h1;
			if (raw1>=end1) return 0;
			dx = MASK_WORD_BITS - dx;
			for(py=maxh;py>=0;py--) { //Search vertically
				if( raw1[py] & (raw2[py] >> dx) ) {
					rx += bsf(raw1[py] & (raw2[py] >> dx)) + dx;
					goto found;
				}
			}
			raw2 += h2;
			rx += MASK_WORD_BITS;
			if (raw2>=end2) return 0;
			dx = MASK_WORD_BITS - dx;
			//dx = -dx & (MASK_WORD_BITS - 1);
		}
	}
found:
	if (result) {
		int ry = dy2 + py;
		if (odx < 0) {
			rx += (-odx)>>MASK_WORD_BITBITS;
			}
		if (odx >= 0) {
			rx += odx>>MASK_WORD_BITBITS;
			ry += dy1 - dy2;
			rx += odx;
		}
		result[0] = rx;
		result[1] = ry;
	}
	return 1;
}


int check_pmask_collision(PMASK *mask1, PMASK *mask2, int x1, int y1, int x2, int y2) {
	return _check_pmask_collision(mask1, mask2, x2-x1 - (mask2->w - mask1->w) / 2, y2-y1 - (mask2->h - mask1->h) / 2);
}

int check_pmask_collision_position(PMASK *mask1, PMASK *mask2, int x1, int y1, int x2, int y2, int *result) {
	if (_check_pmask_collision_position(mask1, mask2,  x2-x1 - (mask2->w - mask1->w) / 2, y2-y1 - (mask2->h - mask1->h) / 2, result)) {
		result[0] += x1;
		result[1] += y1;
		return 1;
	}
	else return 0;
}

void reinit_pmask(PMASK *pm) {
	if (pm->src != pm->osrc || pm->dw != pm->odw || pm->a != pm->oa) {
   		deinit_pmask(pm);
   		int w = (int)(pm->odw);
   		BITMAP *t = create_bitmap(w,w);
   		clear_to_color(t, bitmap_mask_color(t));
		sprite_rsw(t, pm->osrc, w/2, w/2, pm->oa, w, 255);
		init_allegro_pmask(pm, t);
		destroy_bitmap(t);
		pm->a = pm->oa;   pm->src = pm->osrc;   pm->dw = pm->odw;
   	}
}

void mark_pmask(PMASK *pm, BITMAP *bm, float a, float dw) {
	pm->oa = a;   pm->osrc = bm;   pm->odw = dw;
        pm->w = pm->h = (int)(dw);
}
