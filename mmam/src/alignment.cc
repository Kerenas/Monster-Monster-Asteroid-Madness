#include "mmam.h"

alignment_t alignment;

/*
typedef struct org_t {
	int idx; char *nam;  int known, rawal, al, max, min, kill, dam,  polkill, poldam, peaceful;
	class monst_t **memb;
};
*/

org_t org_list[] = {
	{org_none, "none", 0x0, -1,0,0,0,0,0,0, 0, 0, 1, NULL},
        {org_inanim, "Inanimate Objects", 0x0, -1,0,0,0,0,0,0, 0, 0,1, NULL},
	{org_ast0, "Ast Zero", 0x0, -1,0,0,0,0,0,0, 100, 0, 1, NULL},
	{org_bfc_corp, "BFC Corp", 0x0, 0,0,0,40000,-40000,500,1, 500, 1,1, NULL},
	{org_jmart_corp, "JMart Corp", 0x0, 0,0,0,40000,-40000,500,1, 600, 1,1, NULL},
	{org_merchants_guild, "Merchants Guild", 0x0, 0,0,0,40000,-40000,5000,0, 2500, 0,1, NULL},
	{org_raiders, "Raiders", 0x0, 0,0,0,40000,-60000,500,1, -100, 0, NULL},
	{org_galictic_rifle_assc, "Galictic Rifle Assc", 0x0, 0,0,0,60000,-60000,500,1, 100, 0,0, NULL},
	{org_stalker_clan, "Stalker Clan", 0x0, 0,0,0,60000,-60000,500,1, 0, 0,0, NULL},
	{org_fight_commission,  "Fight Commission", 0x0, 0,0,0,40000,-40000,500,1, 500, 1,1, NULL},
	{org_trainers_assc,  "Trainers Assc", 0x0, 0,0,0,40000,-40000,500,1, 500, 1,1, NULL},
	{org_tournet,  "Tournet", 0x0, 0,0,0,40000,-40000,500,5, 1000, 0,1, NULL},
	{org_wise_old_guys,  "Wise Old Guys", 0x0, -1,0,0,40000,-40000,500,1, 1000, 0,1, NULL},
	{org_crysillians,  "Crysillians", 0x0,  -1,0,0,60000,-60000,500,1, 0, 0,1, NULL},
	{org_police,  "Police", 0x0, 0,0,0,60000,-60000,500,1, 0, 0,0, NULL},
	{org_fisec,  "Fisec", 0x0, 0,0,0,40000,-40000,500,1, 800, 1,0, NULL},
	{org_hippies,  "Hippies", 0x0, 0,0,0,40000,-40000,500,1, 800, 1,1, NULL},
	{org_orick_clan0, "Banclaw", 0x0, 0,0,0,40000,-40000,500,1, 0, 0,0, NULL},
	{org_orick_clan1, "Dracomajia", 0x0, 0,0,0,40000,-40000,500,1, 0, 0,0, NULL},
	{org_orick_clan2, "Grelblade", 0x0, 0,0,0,40000,-40000,500,1, 0, 0,0, NULL},
	{org_orick_clan3, "Monrojo", 0x0, 0,0,0,40000,-40000,500,1, 0, 0,0, NULL},
	{org_orick_clan4, "Skulibula", 0x0, 0,0,0,40000,-40000,500,1, 0, 0,0, NULL},
};


void alignment_t::clear() {
        for (int c = 0; c < org_last; c++) {
   		if (org_list[c].known == 1) org_list[c].known = 0;
   		org_list[c].rawal = 0;
   		if (c != org_list[c].idx) {
        		allegro_message("org_list failed self-check!");
        	}
        	alignment.fix(c);
 	}
 	mb_netship.org_idx =	     mb_astbase.org_idx = mb_worker.org_idx  =
 	mb_solararray.org_idx =      mb_fissionplant.org_idx =	mb_fusionplant.org_idx =
	mb_radiomarker.org_idx = mb_organicsscanner.org_idx = mb_deltawavescanner.org_idx =
	mb_qparticlescanner.org_idx =	mb_epsilonhiscanner.org_idx =
	mb_guntower.org_idx = mb_dgtower.org_idx = mb_xcannontower.org_idx = mb_galtixtower.org_idx =
	mb_chainlinkenclosure.org_idx =	mb_gratedenclosure.org_idx =	mb_shieldedenclosure.org_idx =
	mb_jumpgatenode.org_idx =	mb_jumpgatestabilizer.org_idx =
	mb_transphasitestabilizer.org_idx =	mb_multiphasitestabilizer.org_idx =
	mb_omniphasitestabilizer.org_idx =  org_ast0;

	mb_raidship.org_idx =
	mb_raider.org_idx =
	mb_raidercamp.org_idx =
	mb_raiderguntower.org_idx =
	mb_raiderdgtower.org_idx =
	mb_raiderxcannontower.org_idx =
	mb_raidergaltixtower.org_idx =
	mb_raidersolararray.org_idx =
	mb_raiderfissionplant.org_idx =
	mb_raiderfusionplant.org_idx =  org_raiders;

 	mb_jmart.org_idx = org_jmart_corp;
	mb_bfc.org_idx = org_bfc_corp;
        mb_arena.org_idx = org_fight_commission;
        mb_trainer.org_idx = org_trainers_assc;
        mb_merchant.org_idx = org_merchants_guild;
        mb_tourist.org_idx = org_tournet;
        mb_hunter.org_idx = org_galictic_rifle_assc;
        mb_wisehut.org_idx = org_wise_old_guys;
        
        mb_stalker.org_idx = mb_vojodust.org_idx = org_stalker_clan;

	mb_squadship.org_idx =  mb_enforcer.org_idx =
           mb_cityblda.org_idx =   mb_citybldb.org_idx =
           mb_policestation.org_idx =   org_police;

	//mb_happytree.org_idx.org_idx = mb_mimmack.org_idx = org_ast->mimmack_mood;
	mb_crysillian.org_idx = mb_crysillianruins.org_idx = mb_ufo.org_idx =
	   mb_crashsite.org_idx = org_crysillians;

        mb_hippie.org_idx = org_hippies;
        mb_fisec.org_idx = org_fisec; 
}


void alignment_t::fix(int og) {
  	int t = org_list[og].rawal;
  	org_list[og].rawal = MID(org_list[og].min, org_list[og].rawal, org_list[og].max);
        switch (og) {
        case org_jmart_corp:
	case org_bfc_corp:
	case org_fight_commission:
	case org_trainers_assc:
	case org_merchants_guild:
        	org_list[og].al = (t < -20000 ? -3 : t < -10000 ? -2 : t > 1000 ? MIN( t / 1000, 2) : 0);
        break;
	case org_galictic_rifle_assc:
        	org_list[og].al = MID(-6,t / 2000, 3);
        break;
	case org_tournet:
        	org_list[og].al =  MID(-10, t / 2000, 3);
        break;
	case org_raiders:
        	org_list[og].al =  MID(-6, t / 10000,-3);
        break;
	case org_police:
        	org_list[og].al =  MID(-6, t / 8000,3);
	default:
        	org_list[og].al =  MID(-2,t / 5000, 2);
	}
}

/*
int *alignment_t::of_group(class monst_base *t) {
	if (!t) return NULL;
	int *i = NULL;
	if (t == &mb_jmart) i = &jmart_corp;
	else if (t == &mb_bfc) i = &bfc_corp;
	else if (t == &mb_arena) i = &fight_commission;
	else if (t == &mb_trainer) i = &trainers_assc;
	else if (t == &mb_merchant) i = &merchants_guild;
	else if (t == &mb_tourist) i = &tournet;
	else if (t == &mb_raidercamp || t == &mb_raider || t == &mb_raidship) i = &raiders;
	else if (t == &mb_hunter) i = &galictic_rifle_assc;
	else if (t == &mb_squadship || t == &mb_enforcer ||
            t == &mb_cityblda || t == &mb_citybldb || t == &mb_policestation
	) i = &police;
	else if (t == &mb_happytree || t == &mb_mimmack) i = &ast->mimmack_mood;
	else if (t == &mb_stalker || t == &mb_vojodust) i = &stalker_clan;
	else if (t == &mb_crysillian || t == &mb_crysillianruins || t == &mb_crashsite) i = &crysillians;
	else if (t == &mb_wisehut)  i = &wise_old_guys;
        return i;
}

int *alignment_t::of_group(class monst_t *m) {
	if (!m) return NULL;
	if (m->base == &mb_orick || m->base == 	&mb_orickcamp || m->base == &mb_orickcampfire
	) {
		return &orick_clan[m->orick_clan()];
	}
	return of_group(m->base);
}
*/

int alignment_t::align(class monst_t *m) {
        if (!m) return 0;
	org_t *oi = (&org_list[m->org_idx]);
	return (oi->al);
}

void alignment_t::see(class monst_t *m) {
	if (!m) return;
	if (m) m->base->stat_didscan = 1;
	org_t *oi = (&org_list[m->org_idx]);
	if (!oi->known) oi->known = 1;
}

void alignment_t::kill(class monst_t *m) {
	if (!m) return;
	see(m);
	if (m) m->base->stat_killed++;
	asteroid_t *a;
	int oix = m->org_idx;
	org_t *oi = (&org_list[m->org_idx]);
	oi->rawal -= oi->kill;
	org_list[org_police].rawal -= oi->polkill;
	fix(m->org_idx);
	fix(org_police);
	for (int c = 0;; c++) {
   		if (m->base == mb_list_all[c] && ((m->flags & AF_INORGANIC) == 0)) {
                        org_list[org_hippies].rawal -= (m->base->common >= 10 ? 1 : 15 - m->base->common ) * 250;
        		fix(org_hippies);
   		}
   		if (mb_list_all[c] == &mb_golgithan) break;
   	} 
	if (a) {
   		if (oix >= org_orick_clan0 && oix <= org_orick_clan4) {
   			for (int clan = org_orick_clan0; clan <= org_orick_clan4; clan++) if (clan != oix && a->orgs_here[clan]) {
                                befriend(clan, 100);
       			}
     		}
		//if (m->base == &mb_happytree && m->my_ast )
	}
}

void alignment_t::hit(class monst_t *m, int n) {
	if (!m && !n) return;
        org_t *oi = (&org_list[m->org_idx]);
	int minus = oi->dam * n;
	int polminus = oi->poldam * n;
	oi->rawal -= minus;
	org_list[org_police].rawal -= polminus;
	fix(m->org_idx);
	fix(org_police);
	printf("al-hit %s (%s) for %i [%i,%i]\n", oi->nam, m->name,  n, minus, polminus);
}

void alignment_t::offend(int ix, int n) {
	org_t *oi = (&org_list[ix]);
	oi->rawal -= n;
	fix(ix);
}

void alignment_t::befriend(int ix, int n, int max) {
	org_t *oi = (&org_list[ix]);
	if (oi->al > max) return;
	oi->rawal += n;
	fix(ix);
	if (oi->al > max) {
		oi->rawal -= n;
		fix(ix);
   	}
}


void alignment_t::offend(class monst_t *m, int n) {if (m) offend(m->org_idx,n);}

void alignment_t::befriend(class monst_t *m, int n, int max) {
	if (!m) return;
	if (m->org_idx > 0) befriend(m->org_idx, n, max);
}

