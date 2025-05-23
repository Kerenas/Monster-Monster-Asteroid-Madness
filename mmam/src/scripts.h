#ifndef _scripts_h
#define _scripts_h 1
#define SAVE_COMPRESSED		0x0001
#define SAVE_SPAWN_BY		0x0002
#define SAVE_SPAWN		0x0004
#define SAVE_RELATIVE		0x0008
#define SAVE_GRID		0x0010
#define SAVE_AI_STATE		0x0020
#define SAVE_EXACT_WEAP		0x0040
#define SAVE_EXACT_TRAIT	0x0080
#define SAVE_EXACT_ASTEXIT	0x0100
#define SAVE_EXACT (SAVE_AI_STATE | SAVE_EXACT_WEAP | SAVE_EXACT_TRAIT | SAVE_EXACT_ASTEXIT)
#define SAVE_BONES (SAVE_AI_STATE | SAVE_EXACT_WEAP | SAVE_EXACT_TRAIT)
// | SAVE_COMPRESSED)


extern int script_grid_line, script_level;
void pfwrite(PACKFILE *pf, const char *buff);
int script_load(char *fn, class asteroid_t *script_load_ast = NULL);
int dump_ast(char *fn, int flags);
int script_fuss(char *format, ...);


#endif
