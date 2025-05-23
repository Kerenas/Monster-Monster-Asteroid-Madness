#ifndef _alignment_h
#define _alignment_h 1
#include "rand.h"

#define ALIGN_UNK 0x0
#define NUM_ORICK_CLANS 5
#define HERE_ORICK0       0X00001
#define HERE_ORICK1       0X00002
#define HERE_ORICK2       0X00004
#define HERE_ORICK3       0X00008
#define HERE_ORICK4       0X00010
#define HERE_MIMMACK     0X00020
#define HERE_CRYSILLIANS 0X00040
#define how_aligned(who) org_list[org_ ## who].al
extern char **orick_clan_names;

enum { 
	org_none, org_inanim, org_ast0,
	org_bfc_corp, org_jmart_corp, org_merchants_guild, org_raiders,
	org_galictic_rifle_assc,  org_stalker_clan, org_fight_commission,
	org_trainers_assc, org_tournet, org_wise_old_guys, org_crysillians, org_police,
	org_fisec, org_hippies, org_orick_clan0,  org_orick_clan1,  org_orick_clan2,  org_orick_clan3,
	org_orick_clan4, org_last
};

typedef struct org_t {
	int idx; char *nam;  int flags, known, rawal, al, max, min, kill, dam, polkill, poldam, peaceful;
	class monst_t **memb;
};

typedef class alignment_t {
        private:
	void fix(int og);
        public:
	void clear();

	void see(class monst_t *m);
	void kill(class monst_t *m);
	void hit(class monst_t *m, int n);

	void offend(int ix, int n);
	void offend(class monst_t *m, int n);
	void befriend(int ix, int n, int max = 6);
	void befriend(class monst_t *m, int n, int max = 6);
	
        int align(class monst_t *m);
	
	void save(PACKFILE *pf, int flags);
	int  parse(char *cmd, char *next, int line);	
};

extern alignment_t alignment;
extern org_t org_list[];

#endif
