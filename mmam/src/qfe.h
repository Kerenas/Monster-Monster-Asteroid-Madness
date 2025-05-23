#ifndef _qfe_h
#define _qfe_h 1
#ifdef USE_ADIME
#include <adime.h>
#define qdlg(title, x, y, w, format, strs...)  \
   adime_lowlevel_dialogf(title, x, y, w, format "%buttonrow[OK;CTRL+O;ENTER]", strs, ok_adime)
int  ok_adime(DIALOG *d, int n);
#else 

#endif

#define QFE_NEW_GAME -3
#define QFE_LOAD_GAME -2
#define QFE_QUIT -1

extern int ai_test, enable_sound, enable_music, sound_vol,
          music_vol, music_buffer_size,  music_freq, chat_linger,
          default_screen_w, default_screen_h, prefer_windowed, enable_page_flip[2],
          skip_intro, lev_editor, lev_edit_time0, dump_ast_flags, make_nobones, 
          show_fps, autoshot_freq_sec;
extern char *arg_cfg_file, *arg_command_list, *command_list;
extern int arg_net_mode, arg_ai_test, arg_def_port, arg_skip_intro, arg_quick, arg_cheater,
	arg_nosound, sackless_mode,  arg_nobones, arg_fps, arg_mute_add, arg_buyany,
	arg_money, arg_xinfo;
extern char player_name[30], music_dir[200], data_dir[200], scr_shot_dir[200], home_dir[200];
extern float gamma_lvl;

int  qfe_gamesetup();
void getstartupvals(char **argv);
int  sound_options_dlg(DIALOG *d);
void set_badmode();
void unset_badmode();

#endif
