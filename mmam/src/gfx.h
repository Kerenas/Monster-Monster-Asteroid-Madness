#ifndef _gfx_h
#define _gfx_h 1
void create_tile_map16(BITMAP *from, int start);
int getimages();
void load_all_to_vram();

void check_scale();
int init_graphics();
void reinit_graphics();

int copy_sprite_to_vram(int idx);
int unload_sprite_from_vram(int idx);
void load_all_to_vram();
void fix_fubar_vram();

int setup_graphics(int w, int h, int want_windowed, int want_pageflip, int accel);

#endif
