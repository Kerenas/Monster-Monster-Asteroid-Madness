#ifndef _input_h
#define _input_h 1

typedef struct input_map_t {
	int vx, vy, vt;
	int target_x, target_y, target_valid, last_menu;
	int fire[4];
	int monster_but, menu_but, monster_hover, menu_hover, menu_exit_but;
	int sel;
	int old_mouse_z, old_mouse_x, old_mouse_y, old_mouse_b;
	int click_ok;
	char chat_buffer[100];
	int paused;
	int last_input_time;
	int vehicle_btn, repair_btn, get_btn, quick_weap_btn;
	struct ivalu_t *quick_weap_tab[10];
};

extern input_map_t input_map;

void check_kb_input();

extern int tiredofplaying, want_to_quit;

void safty_pause();

#define PAUSED (input_map.paused || key[KEY_PAUSE])

#endif

