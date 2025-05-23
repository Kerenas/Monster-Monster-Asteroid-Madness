#include "mmam.h"
#include "bubbles.h"

blood_t::blood_t(asteroid_t *a, monst_t *m, float ang, float vel, int amt, int alt_fluid) {
	  	x = (int)(m->x); y = (int)(m->y); vx = vy = 0;
	  	my_ast = a;
		if (!blood_pool[0]) init_blood_pool();
		dry_time = ms_count + 12000;
	  	color = m->base->blood_color;
	  	if (alt_fluid == 1) color = &White;
		if (alt_fluid == 2) color = &Yellow;
		if (alt_fluid == 3) color = &Brown;
		shape = (amt>=0 ? 0 : 1);
		pool_size = (int)(m->radius * 2);
		pool = (
			shape == 0 ? NULL :
			*color == Red   || *color == LightRed ? blood_pool[0] :
			*color == Green || *color == LightGreen ? blood_pool[1] :
			NULL
		);
		if (pool == NULL) shape = 0; else dry_time = ms_count + 45000;

}

void blood_t::poll() {}

void blood_t::draw() {
	if (shape == 0) {
		putpixel(active_page, x - my_ast->camera_x,     y - my_ast->camera_y, *color);
		putpixel(active_page, x - my_ast->camera_x,     y - my_ast->camera_y + 1, *color);
		putpixel(active_page, x - my_ast->camera_x + 1, y - my_ast->camera_y, *color);
		putpixel(active_page, x - my_ast->camera_x + 1, y - my_ast->camera_y + 1, *color);
	} else {
		int tl = dry_time - ms_count;
		int rp = (int)(pool_size * (tl < 42000 ? 1. : 1. - ((tl - 42000) / 3000.)));
		sprite_rsw(active_page, pool,
			x - my_ast->camera_x,     y - my_ast->camera_y, 0.0,
			rp, (tl > 10000 ? 220 : tl * 220 / 10000)
		);
	}
}
