#define IN_MONSTBASE_CC 1
#include "albgi.h"
#include "mmam.h"

//////////////////// food chain //////////////////////////////
#define LIKES_TO_EAT(who, x...) static monst_base  *who ## _likes_to_eat[] = {x NULL};
#define WILL_EAT(who, x...)	static monst_base  *who ## _will_eat[] = {x NULL};
#define HATES(who, x...)	static monst_base  *who ## _hates[] = {x NULL};
#define SCARED_OF(who, x...)	static monst_base  *who ## _scared_of[] = {x NULL};
#define NO_CHAIN(who) LIKES_TO_EAT(who) WILL_EAT(who) HATES(who) SCARED_OF(who)


NO_CHAIN(netship)
NO_CHAIN(ufo)
NO_CHAIN(meteor)

NO_CHAIN(raider) NO_CHAIN(raidship) NO_CHAIN(hunter) // their ai knows to attack everything
NO_CHAIN(fern)	 NO_CHAIN(shrub)  NO_CHAIN(stalkplant)
NO_CHAIN(poisonshroom) NO_CHAIN(psychoshroom) NO_CHAIN(pyroshroom)
NO_CHAIN(happytree) // plants don't eat

//mimmacks are photosynthetic
LIKES_TO_EAT(mimmack, )
WILL_EAT(mimmack,)
HATES(mimmack, &mb_netship, &mb_raider, &mb_hunter,)
SCARED_OF(mimmack,)


LIKES_TO_EAT(tourist,)
       WILL_EAT(tourist,)
       HATES(tourist, )
       SCARED_OF(tourist, &mb_raider, &mb_raidship, &mb_raidercamp, &mb_raiderguntower, &mb_raiderdgtower,
                        &mb_raiderxcannontower, &mb_raidergaltixtower, &mb_meteor,)
LIKES_TO_EAT(merchant,)
       WILL_EAT(merchant,)
       HATES(merchant, )
       SCARED_OF(merchant, &mb_raider, &mb_raidship, &mb_raidercamp, &mb_raiderguntower, &mb_raiderdgtower,
                        &mb_raiderxcannontower, &mb_raidergaltixtower, &mb_golgithan,  &mb_meteor,)
       
LIKES_TO_EAT(trainer,)
       WILL_EAT(trainer,)
       HATES(trainer, )
       SCARED_OF(trainer, &mb_raider, &mb_raidship, &mb_raidercamp, &mb_raiderguntower, &mb_raiderdgtower,
                        &mb_raiderxcannontower, &mb_raidergaltixtower, &mb_golgithan,  &mb_meteor,)
       
LIKES_TO_EAT(sentry,)
       WILL_EAT(sentry,)
       HATES(sentry, &mb_raider, &mb_raidship, )
       SCARED_OF(sentry,)

LIKES_TO_EAT(sentinel,)
       WILL_EAT(sentinel,)
       HATES(sentinel, &mb_raider, &mb_raidship, &mb_raidercamp, &mb_raiderguntower, &mb_raiderdgtower,
                        &mb_raiderxcannontower, &mb_raidergaltixtower,)
       SCARED_OF(sentinel,)

// violent workers and security forces will fight raiders  
LIKES_TO_EAT(worker,)
       WILL_EAT(worker,)
       HATES(worker, )
       SCARED_OF(worker, &mb_raider, &mb_raidship, &mb_raidercamp, &mb_raiderguntower, &mb_raiderdgtower,
                        &mb_raiderxcannontower, &mb_raidergaltixtower, &mb_meteor,)


LIKES_TO_EAT(squadship,)
       WILL_EAT(squadship,)
       HATES(squadship, &mb_raider, &mb_raidship, &mb_raidercamp, &mb_raiderguntower, &mb_raiderdgtower,
                        &mb_raiderxcannontower, &mb_raidergaltixtower,)
       SCARED_OF(squadship,)
       
LIKES_TO_EAT(watcher,)
       WILL_EAT(watcher,)
       HATES(watcher, &mb_raider, &mb_raidship, &mb_raidercamp, &mb_raiderguntower, &mb_raiderdgtower,
                      &mb_raiderxcannontower, &mb_raidergaltixtower,)
       SCARED_OF(watcher,)

       

LIKES_TO_EAT(enforcer,)
       WILL_EAT(enforcer,)
       HATES(enforcer, &mb_raider, &mb_raidship, &mb_raidercamp, &mb_raiderguntower, &mb_raiderdgtower,
                        &mb_raiderxcannontower, &mb_raidergaltixtower,)
       SCARED_OF(enforcer,)

LIKES_TO_EAT(hippie,)
       WILL_EAT(hippie,)
       HATES(hippie, &mb_tourist, &mb_hunter,)
       SCARED_OF(hippie, &mb_enforcer, &mb_squadship, &mb_netship, &mb_raider, &mb_raidship, &mb_sentry, &mb_sentinel, &mb_meteor,)

LIKES_TO_EAT(cheech, &mb_frogert, &mb_quiver, &mb_psychoshroom,)
       WILL_EAT(cheech, &mb_shrub, &mb_pygment,)
       HATES(cheech,)
       SCARED_OF(cheech, &mb_raider, &mb_raidship, &mb_hunter, &mb_yazzert, &mb_vaerix, &mb_golgithan, &mb_meteor, &mb_malcumbit,)

LIKES_TO_EAT(hoveler, &mb_mubark, &mb_furbit, &mb_shruver, &mb_bigasaurus,)
       WILL_EAT(hoveler, &mb_stalkplant, &mb_molber,)
       HATES(hoveler, &mb_malcumbit, )
       SCARED_OF(hoveler, &mb_raider, &mb_raidship, &mb_hunter, &mb_dragoonigon, &mb_golgithan, &mb_meteor,)

LIKES_TO_EAT(orick, &mb_mubark, &mb_furbit, &mb_molber,)
       WILL_EAT(orick, &mb_stalkplant, &mb_pygment, &mb_hippie,)
       HATES(orick, &mb_raider, &mb_hunter, &mb_raidship, &mb_malcumbit, &mb_sentry, &mb_sentinel,)
       SCARED_OF(orick, &mb_golgithan, &mb_meteor,)


LIKES_TO_EAT(furbit, &mb_fern, &mb_shrub,)
       WILL_EAT(furbit, &mb_stalkplant, &mb_roachy,)
       HATES(furbit)
       SCARED_OF(furbit, &mb_netship, &mb_raider, &mb_raidship, &mb_hunter, &mb_happytree, &mb_mubark, &mb_throbid, &mb_squarger,
         &mb_quitofly, &mb_toothworm, &mb_sneeker, &mb_squeeker,  &mb_molber, &mb_drexler, &mb_bigasaurus,
	 &mb_uberworm,	&mb_dragoonigon, &mb_krakin,   &mb_rexrex, &mb_bervul,  &mb_yazzert, &mb_vaerix,
         &mb_frogert, &mb_ackatoo, &mb_quiver, &mb_crysillian, &mb_golgithan, &mb_meteor, &mb_titurtilion,  &mb_sentry, &mb_sentinel,)

LIKES_TO_EAT(pygment, &mb_stalkplant,)
       WILL_EAT(pygment, &mb_fern, &mb_shrub,)
       HATES(pygment,)
       SCARED_OF(pygment,)


LIKES_TO_EAT(shruver, &mb_fern, &mb_shrub, &mb_roachy,)
       WILL_EAT(shruver, &mb_stalkplant,)
       HATES( shruver,)
       SCARED_OF( shruver, &mb_netship, &mb_raider, &mb_raidship, &mb_hunter, &mb_throbid, &mb_squarger, &mb_quitofly, &mb_toothworm,
         &mb_molber, &mb_drexler, &mb_bigasaurus, &mb_uberworm, &mb_dragoonigon, &mb_krakin, &mb_rexrex, &mb_bervul,
         &mb_yazzert, &mb_vaerix, &mb_frogert, &mb_crysillian, &mb_golgithan, &mb_meteor,  &mb_sentry, &mb_sentinel,)


LIKES_TO_EAT(radrunner, &mb_stalkplant, &mb_roachy,)
       WILL_EAT(radrunner, &mb_shrub, &mb_fern,)
       HATES( radrunner,)
       SCARED_OF( radrunner, &mb_netship, &mb_raider, &mb_raidship, &mb_hunter, &mb_throbid, &mb_squarger,
         &mb_quitofly, &mb_toothworm, &mb_drexler, &mb_bigasaurus, &mb_uberworm, &mb_dragoonigon, 
         &mb_krakin, &mb_rexrex, &mb_bervul, &mb_crysillian, &mb_golgithan, &mb_meteor, &mb_malcumbit, &mb_sentry, &mb_sentinel,)


LIKES_TO_EAT(roachy, &mb_fern, &mb_shrub, &mb_stalkplant, &mb_quiver,)
       WILL_EAT(roachy, &mb_psychoshroom,)
       HATES(roachy, &mb_happytree,)
       SCARED_OF(roachy,&mb_netship, &mb_raider, &mb_raidship,  &mb_sentry, &mb_sentinel, &mb_mimmack, &mb_furbit, &mb_pygment,
		&mb_shruver, &mb_mubark, &mb_throbid,  &mb_squarger, &mb_quitofly, &mb_toothworm,
		&mb_sneeker,  &mb_squeeker, &mb_molber, &mb_drexler, &mb_golgithan, &mb_meteor,)


LIKES_TO_EAT(krulasite, &mb_squarger, &mb_drexler,  &mb_rexrex, &mb_ackatoo, &mb_mubark, &mb_quiver, &mb_yazzert,
                        &mb_vaerix, &mb_throbid, &mb_toothworm, &mb_molber, &mb_hunter, &mb_raider, &mb_mimmack,
                        &mb_bervul, &mb_titurtilion, &mb_hippie, &mb_cheech, &mb_orick, &mb_hoveler,
                        &mb_sentry, &mb_sentinel, )
       WILL_EAT(krulasite, &mb_bigasaurus, &mb_uberworm, &mb_mamothquito, &mb_dragoonigon, &mb_krakin,
			&mb_quitofly, &mb_roachy,  &mb_furbit, &mb_shruver, &mb_pygment, &mb_meeker,
			&mb_squeeker, &mb_sneeker, &mb_stalker, &mb_radrunner,)
       HATES(krulasite, )
       SCARED_OF(krulasite,)


LIKES_TO_EAT(mubark, &mb_happytree, &mb_poisonshroom, &mb_psychoshroom,)
       WILL_EAT(mubark,&mb_fern, &mb_shrub,)
       HATES(mubark, )
       SCARED_OF(mubark, &mb_stalkplant, &mb_golgithan, &mb_meteor, &mb_squarger, &mb_malcumbit, )
       

LIKES_TO_EAT(malcumbit, &mb_fern, &mb_shrub, &mb_happytree,)
       WILL_EAT(malcumbit,&mb_stalkplant, &mb_roachy, &mb_mimmack,)
       HATES(malcumbit, &mb_hunter, &mb_raider, &mb_hippie,  &mb_sentry, &mb_sentinel, &mb_stalker, &mb_cheech, &mb_orick, &mb_hoveler, )
       SCARED_OF(malcumbit, &mb_bigasaurus, &mb_uberworm, &mb_golgithan, &mb_meteor,)


LIKES_TO_EAT(throbid, &mb_mubark, &mb_pygment, &mb_shruver, &mb_molber, &mb_hippie,)
WILL_EAT(throbid, &mb_furbit, &mb_sneeker, &mb_squeeker, &mb_frogert, &mb_radrunner,)
HATES(throbid, &mb_raider, &mb_hunter, &mb_toothworm, &mb_ackatoo, &mb_bervul, &mb_hippie, &mb_sentry, &mb_sentinel,)
SCARED_OF(throbid,  &mb_drexler, &mb_raidship,  &mb_uberworm, &mb_dragoonigon, &mb_krakin, &mb_squarger,
          &mb_rexrex, &mb_yazzert, &mb_vaerix, &mb_frogert, &mb_golgithan, &mb_meteor,)


LIKES_TO_EAT(squarger, &mb_happytree,)
WILL_EAT(squarger ,)
HATES(squarger ,&mb_raider, &mb_hunter, &mb_furbit, &mb_pygment, &mb_shruver, &mb_mubark, &mb_throbid, &mb_quitofly,
          &mb_roachy, &mb_toothworm, &mb_sneeker,  &mb_squeeker, &mb_molber, &mb_drexler, &mb_hippie, 
          &mb_sentry, &mb_sentinel,)
SCARED_OF(squarger, &mb_netship, &mb_golgithan, &mb_meteor,)

LIKES_TO_EAT(quitofly, &mb_furbit, &mb_shruver, &mb_mubark, &mb_sneeker, &mb_squeeker, &mb_molber,
	&mb_rexrex, &mb_bervul, &mb_ackatoo, &mb_quiver, &mb_hippie, &mb_malcumbit,)
WILL_EAT(quitofly, &mb_raider, &mb_hunter, &mb_throbid, &mb_bigasaurus, &mb_dragoonigon,
	&mb_krakin, &mb_meeker, &mb_frogert, &mb_radrunner, &mb_sentry, &mb_sentinel,)
HATES(quitofly, )
SCARED_OF(quitofly,)

LIKES_TO_EAT(mamothquito, &mb_raider, &mb_hunter, &mb_throbid, &mb_krakin, &mb_rexrex, &mb_bervul,  &mb_yazzert,
         &mb_vaerix, &mb_frogert, &mb_hippie, &mb_sentry, &mb_sentinel,)
WILL_EAT(mamothquito, &mb_furbit, &mb_shruver, &mb_mubark,  &mb_toothworm, &mb_sneeker,  &mb_squeeker,
          &mb_molber, &mb_drexler, &mb_radrunner,)
HATES(mamothquito, &mb_uberworm, )
SCARED_OF(mamothquito,)


LIKES_TO_EAT(toothworm, &mb_furbit, &mb_shruver, &mb_pygment, &mb_quitofly, &mb_roachy, &mb_hippie, &mb_pyroshroom,)
WILL_EAT(toothworm, &mb_mubark, &mb_molber, &mb_bervul, &mb_frogert, &mb_radrunner,)
HATES(toothworm, &mb_vaerix, &mb_yazzert, )
SCARED_OF(toothworm,)


LIKES_TO_EAT(uberworm, &mb_mubark, &mb_molber, &mb_bervul, &mb_frogert, &mb_hippie, &mb_pyroshroom,)
WILL_EAT(uberworm, &mb_furbit, &mb_shruver, &mb_pygment, &mb_quitofly, &mb_roachy,
                   &mb_krakin, &mb_rexrex, &mb_throbid, &mb_malcumbit,)
HATES(uberworm, &mb_squarger, &mb_mamothquito, &mb_vaerix, &mb_yazzert,)
SCARED_OF(uberworm, )


LIKES_TO_EAT(sneeker,&mb_fern, &mb_shrub, )
WILL_EAT(sneeker ,&mb_stalkplant, &mb_roachy,)
HATES(sneeker ,)
SCARED_OF(sneeker ,&mb_netship, &mb_raider, &mb_hunter, &mb_throbid, &mb_squarger, &mb_quitofly,
	 &mb_toothworm, &mb_squeeker,  &mb_molber, &mb_drexler, &mb_bigasaurus, &mb_mamothquito,
	 &mb_uberworm,	&mb_dragoonigon, &mb_krakin,   &mb_rexrex, &mb_bervul,  &mb_yazzert, &mb_vaerix,
         &mb_frogert, &mb_golgithan, &mb_meteor, &mb_titurtilion, &mb_stalker, &mb_malcumbit, &mb_sentry, &mb_sentinel,)

LIKES_TO_EAT(squeeker, &mb_happytree, &mb_psychoshroom,)
WILL_EAT(squeeker, &mb_fern, &mb_shrub, &mb_stalkplant,)
HATES(squeeker)
SCARED_OF(squeeker, &mb_raider, &mb_hunter, &mb_throbid, &mb_squarger, &mb_quitofly,
	 &mb_toothworm, &mb_molber, &mb_drexler, &mb_bigasaurus, &mb_mamothquito,
	 &mb_uberworm,	&mb_dragoonigon, &mb_krakin,   &mb_rexrex, &mb_bervul,  &mb_yazzert, &mb_vaerix,
         &mb_frogert, &mb_golgithan, &mb_meteor, &mb_titurtilion, &mb_malcumbit, &mb_sentry, &mb_sentinel,)


LIKES_TO_EAT(meeker, &mb_happytree, &mb_roachy, &mb_quitofly, &mb_psychoshroom,)
WILL_EAT(meeker, &mb_fern, &mb_shrub, &mb_stalkplant,)
HATES(meeker, &mb_raider, &mb_hunter, &mb_throbid, &mb_sentry, &mb_sentinel,)
SCARED_OF(meeker,  &mb_drexler, &mb_bigasaurus, &mb_mamothquito, &mb_uberworm, 
	&mb_dragoonigon, &mb_krakin, &mb_rexrex, &mb_golgithan, &mb_meteor,)

LIKES_TO_EAT(molber, &mb_fern, &mb_shrub,)
WILL_EAT(molber,)
HATES(molber,)
SCARED_OF(molber, &mb_netship, &mb_raider, &mb_raidship, &mb_meteor, &mb_hunter,)


LIKES_TO_EAT(drexler, &mb_stalkplant,)
WILL_EAT(drexler, &mb_furbit, &mb_shruver, &mb_mubark, &mb_sneeker, &mb_squeeker,
                  &mb_roachy, &mb_molber, &mb_bervul, &mb_frogert, &mb_hippie,)
HATES(drexler, &mb_rexrex, &mb_toothworm, &mb_uberworm, &mb_throbid, &mb_raider,
               &mb_hunter, &mb_krakin, &mb_mamothquito, &mb_stalker,)
SCARED_OF(drexler, )

LIKES_TO_EAT(bigasaurus, &mb_mubark, &mb_molber, &mb_raider, &mb_hunter, &mb_hippie, &mb_sentry, &mb_sentinel,)
WILL_EAT(bigasaurus, &mb_bervul, &mb_frogert, &mb_toothworm, &mb_squarger, &mb_ackatoo, &mb_quiver, &mb_hoveler,)
HATES(bigasaurus,)
SCARED_OF(bigasaurus, &mb_golgithan, &mb_meteor,)


LIKES_TO_EAT(dragoonigon, &mb_vaerix, &mb_quiver, &mb_hoveler, &mb_orick, &mb_raider, &mb_hunter,
                          &mb_sentry, &mb_sentinel,)
WILL_EAT(dragoonigon, &mb_yazzert, &mb_mimmack, &mb_molber, &mb_radrunner, &mb_hippie, &mb_malcumbit, &mb_pyroshroom,)
HATES(dragoonigon, &mb_crysillian, &mb_raider, &mb_hunter, )
SCARED_OF(dragoonigon, &mb_meeker, &mb_squeeker, &mb_golgithan, &mb_meteor,)

LIKES_TO_EAT(krakin, &mb_fern, &mb_quitofly,)
WILL_EAT(krakin, &mb_pygment, &mb_frogert, &mb_roachy,)
HATES(krakin, &mb_quiver,)
SCARED_OF(krakin, &mb_bigasaurus, &mb_uberworm, &mb_mamothquito, &mb_golgithan, &mb_meteor, )

LIKES_TO_EAT(bervul, &mb_furbit, &mb_shruver, &mb_radrunner,)
WILL_EAT(bervul, &mb_pygment, &mb_frogert, &mb_roachy,)
HATES(bervul, &mb_raider,)
SCARED_OF(bervul, &mb_bigasaurus, &mb_uberworm, &mb_mamothquito, &mb_drexler,
	&mb_throbid, &mb_squarger, &mb_toothworm, &mb_malcumbit,
        &mb_dragoonigon, &mb_krakin, &mb_golgithan, &mb_meteor, &mb_titurtilion, )

LIKES_TO_EAT(rexrex, &mb_furbit, &mb_shruver, &mb_molber, )
WILL_EAT(rexrex, &mb_pygment, &mb_ackatoo, &mb_frogert, &mb_roachy, &mb_toothworm, &mb_hippie,)
HATES(rexrex, &mb_raider, &mb_malcumbit,)
SCARED_OF(rexrex, &mb_bigasaurus, &mb_uberworm, &mb_mamothquito, &mb_dragoonigon, &mb_krakin, &mb_golgithan, &mb_meteor, )

LIKES_TO_EAT(vaerix, &mb_shrub,)
WILL_EAT(vaerix, )
HATES(vaerix, &mb_hunter, &mb_raider, &mb_quitofly, &mb_cheech, )
SCARED_OF(vaerix, &mb_uberworm, &mb_dragoonigon, &mb_golgithan, &mb_meteor, &mb_hippie,)

LIKES_TO_EAT(yazzert, &mb_shrub,)
WILL_EAT(yazzert, )
HATES(yazzert, &mb_hunter, &mb_raider, &mb_quitofly, &mb_mimmack, &mb_toothworm, &mb_uberworm, &mb_cheech, )
SCARED_OF(yazzert, &mb_dragoonigon, &mb_golgithan, &mb_meteor, &mb_hippie,)

LIKES_TO_EAT(frogert, &mb_quitofly, &mb_krulasite,)
WILL_EAT(frogert, &mb_roachy,)
HATES(frogert, &mb_krulasite,)
SCARED_OF(frogert, &mb_netship, &mb_mubark, &mb_throbid, &mb_squarger, &mb_toothworm, &mb_molber, &mb_drexler,
                   &mb_bigasaurus, &mb_uberworm, &mb_dragoonigon, &mb_krakin, &mb_rexrex, &mb_bervul, &mb_ackatoo,
                   &mb_golgithan, &mb_meteor, &mb_titurtilion, )


LIKES_TO_EAT(ackatoo, &mb_fern, &mb_shrub,)
WILL_EAT(ackatoo, &mb_stalkplant, &mb_roachy,)
HATES(ackatoo, &mb_hunter, &mb_raider,)
SCARED_OF(ackatoo, &mb_squarger, &mb_drexler, &mb_bigasaurus, &mb_uberworm, &mb_dragoonigon, &mb_krakin, &mb_rexrex,
                   &mb_golgithan, &mb_meteor, &mb_titurtilion, )


LIKES_TO_EAT(quiver, &mb_quitofly, &mb_roachy,)
WILL_EAT(quiver, )
HATES(quiver, &mb_quitofly, &mb_roachy,)
SCARED_OF(quiver, &mb_cheech,)

// chemosinthetic
LIKES_TO_EAT(crysillian,)
WILL_EAT(crysillian,)
HATES(crysillian, &mb_raider, &mb_mimmack, &mb_squarger, &mb_golgithan, &mb_hippie, &mb_hunter,)
SCARED_OF(crysillian,)


LIKES_TO_EAT(slymer, &mb_psychoshroom, &mb_frogert, &mb_shruver, &mb_raider, &mb_molber, &mb_hunter,)
WILL_EAT(slymer, &mb_furbit, &mb_stalkplant, &mb_hippie, &mb_sentry, &mb_sentinel,)
HATES(slymer,)
SCARED_OF(slymer,)

LIKES_TO_EAT(stalker, &mb_ackatoo, &mb_hunter, &mb_raider, &mb_pygment, &mb_hippie,)
WILL_EAT(stalker, &mb_throbid, &mb_bervul, &mb_shruver, &mb_sentry, &mb_sentinel,)
HATES(stalker,&mb_sneeker, &mb_malcumbit,)
SCARED_OF(stalker,&mb_meeker, &mb_squeeker,)


LIKES_TO_EAT(titurtilion, &mb_happytree, )
WILL_EAT(titurtilion, &mb_fern, &mb_shrub, )
HATES(titurtilion, )
SCARED_OF(titurtilion, &mb_golgithan, &mb_meteor,)


LIKES_TO_EAT(golgithan, &mb_squarger, &mb_drexler,  &mb_rexrex, &mb_ackatoo, &mb_mubark, &mb_quiver, &mb_yazzert,
                        &mb_vaerix, &mb_throbid, &mb_toothworm, &mb_molber, &mb_hunter, &mb_raider, &mb_mimmack,
                        &mb_bervul, &mb_slymer, &mb_titurtilion, &mb_malcumbit,)
WILL_EAT(golgithan, &mb_bigasaurus, &mb_uberworm, &mb_mamothquito, &mb_dragoonigon, &mb_krakin, &mb_sentry, &mb_sentinel,
	&mb_quitofly, &mb_roachy,  &mb_furbit, &mb_shruver, &mb_pygment, &mb_frogert, &mb_meeker,
	&mb_squeeker, &mb_sneeker, &mb_stalker, &mb_cheech, &mb_hoveler, &mb_orick, &mb_radrunner, &mb_hippie,)
HATES(golgithan, )
SCARED_OF(golgithan, &mb_happytree,)



#undef LIKES_TO_EAT
#undef WILL_EAT
#undef HATES
#undef SCARED_OF
#undef NO_CHAIN

///////////////////////////// Animation //////////////////////////////////////////////
#define ENDASL {-1,0,-1,-1}
#define SPR_BIG(who, x)    static int  who ## _spr_big = SPR_ ## x;
#define SPR_CHILL(who, x...)	static aspath  who ## _spr_chill[] = {x ENDASL};
#define SPR_WALK(who, y, x...)	   static aspath  who ## _spr_walk[] = {x ENDASL};  static int  who ## _walk_span = y;
#define SPR_EAT(who, x...)	static aspath  who ## _spr_eat[] = {x ENDASL};
#define SPR_ATTACK(who, x...)	static aspath  who ## _spr_attack[] = {x ENDASL};
#define SPR_DIE(who, x...)	static aspath  who ## _spr_die[] = {x ENDASL};
#define NO_SPRITES(who) static int  who ## _spr_big = SPR_NO_BIG_PIC; SPR_CHILL(who) SPR_WALK(who,0) SPR_EAT(who) SPR_ATTACK(who) SPR_DIE(who)
#define SPR_QSHIP(who, big, top) SPR_BIG(who, big) SPR_CHILL(who, W(top),) SPR_WALK(who,0, W(top),) SPR_EAT(who, W(top),) SPR_ATTACK(who, W(top),) SPR_DIE(who, W(DEBRIES),)
#define SPR_QBLD(who, big, top) SPR_BIG(who, big) SPR_CHILL(who, W(top),) SPR_DIE(who, W(DEBRIES),)
#define SOUNDTAB(who, bark, bark_freq, die, pain, freek, eat) static int who ## _snds[] = {bark, bark_freq, die, pain, freek, eat};
#define NO_SOUND(who) static int who ## _snds[] = {0, 0, 0, 0, 0, 0};
// temp workarround
#define SND_SML_BRK 0


#define W(idx) {SPR_ ## idx,0,-1,-1}
#define A(idx,ms) {SPR_ ## idx,ms,-1,-1}
#define G(idx,ms,wx,wy) {SPR_ ## idx,ms,wx,wy}
#define Y(idx,wx,wy) {SPR_ ## idx,0,wx,wy}

#undef SPR_DEBRIES
#define SPR_DEBRIES SPR_DEBRIES1
/**************************
BAD SPRITES
rexrex attack,
*cheech/hoveler too low res/contrast
uberworm movement anim


MISSING SPRITES
SPR_DIE : toothworm
SPR_DIE : quitofly
SPR_DIE : squarger
SPR_DIE : slymer
SPR_DIE : mamothquito
SPR_DIE : quiver
SPR_DIE, EAT : vaerix (ok?)
SPR_DIE, EAT : yazzert (ok?)
CHILL ANIM, EAT ANIM, ATK : rexrex
CHILL ANIM, EAT ANIM, MAKE SPEAR : orick
CHILL ANIM, EAT ANIM, MAKE : hoveler
CHILL ANIM, EAT? : shruver
CHILL ANIM, EAT? : mubark
CHILL ANIM : throbid
CHILL ANIM? : squarger
CHILL ANIM : molber
CHILL ANIM : ackatoo
CHILL ANIM (crabs don't move much?) : drexler
CHILL ANIM (should patrol?) : enforcer
CHILL ANIM (wanders ?) : krakin
CHILL ANIM : bigasaurus
CHILL ANIM (eats everything?) : golgithan
EAT?: pygment
EAT?: shruver
ATTACK (special code fix?): krulasite
EAT?: dragoonigon
ATK : bervel

**************************/

///////////////// UNANIMATED BUILDINGS ////////////
SPR_QBLD(astbase, BASEF, BASE)   NO_SOUND(astbase)
SPR_QBLD(raidercamp, RAIDERCAMPF, RAIDERCAMP) NO_SOUND(raidercamp)
SPR_QBLD(orickcamp, ORICKCAMPF, ORICKCAMP)  NO_SOUND(orickcamp)
SPR_QBLD(crysillianruins, CRYSILLIANRUINSF, CRYSILLIANRUINS) NO_SOUND(crysillianruins)
SPR_QBLD(crashsite, CRASHSITEF, CRASHSITE) NO_SOUND(crashsite)
SPR_QBLD(arena, ARENAF, ARENA) NO_SOUND(arena)
SPR_QBLD(jmart, JMARTF, JMART) NO_SOUND(jmart)
SPR_QBLD(bfc, BFCF, BFC) NO_SOUND(bfc)
SPR_QBLD(cityblda, CITYAF, CITYA) NO_SOUND(cityblda)
SPR_QBLD(citybldb, CITYBF, CITYB) NO_SOUND(citybldb)
SPR_QBLD(policestation, POLICESTATIONF, POLICESTATION) NO_SOUND(policestation)
SPR_QBLD(fisec, FISECF, FISEC) NO_SOUND(fisec)


NO_SOUND(arenahq)

/////////////// UNANIMATED SHIPS ////////////////
SPR_QSHIP(merchant, TRADESHIPF, TRADESHIP) NO_SOUND(merchant)
SPR_QSHIP(trainer, PITSHIPF, PITSHIP) NO_SOUND(trainer)
SPR_QSHIP(tourist, TOURISTSHIPF, TOURISTSHIP) NO_SOUND(tourist)
SPR_QSHIP(raidship, RAIDSHIPF, RAIDERSHIP) NO_SOUND(raidship)


////////////// THINGS WITH TURRETS ////////
// gets tweeked by code.
int netship_spr_big = SPR_NO_BIG_PIC;
aspath  netship_spr_chill[] = { Y(NO_LIL_PIC, 49, 42), ENDASL};
static int netship_walk_span = 10;  SPR_DIE(netship, W(DEBRIES),)
#define netship_spr_walk   netship_spr_chill
#define netship_spr_eat    netship_spr_chill
#define netship_spr_attack netship_spr_chill
NO_SOUND(netship)

SPR_BIG(watcher, WATCHERF)
SPR_CHILL(watcher, Y(WAT_STILL,39,44),)
SPR_WALK(watcher, 7, Y(WAT_MOV1,38,43), Y(WAT_MOV2,38,43), Y(WAT_MOV1,38,43), Y(WAT_STILL,39,44),
                   Y(WAT_MOV3,40,43), Y(WAT_MOV4,40,43), Y(WAT_MOV3,40,43), Y(WAT_STILL,39,44),)
SPR_EAT(watcher, Y(WAT_STILL,39,44),)
SPR_ATTACK(watcher, Y(WAT_STILL,39,44),)
SPR_DIE(watcher, W(DEBRIES),)
NO_SOUND(watcher)

SPR_BIG(enforcer, ENFORCERF)
SPR_CHILL(enforcer, Y(ENF_STILL,41,39),)
SPR_WALK(enforcer, 6, Y(ENF_MOV1,40,40), Y(ENF_MOV2,40,40), Y(ENF_MOV1,40,40), Y(ENF_STILL,41,39),
                   Y(ENF_MOV3,42,40), Y(ENF_MOV4,42,41), Y(ENF_MOV3,42,40), Y(ENF_STILL,41,39),)
SPR_EAT(enforcer, Y(ENF_STILL,41,37),)
SPR_ATTACK(enforcer, Y(ENF_STILL,41,37),)
SPR_DIE(enforcer, W(DEBRIES),)
NO_SOUND(enforcer)

/*
SPR_BIG(minienforcer, MENFORCERF)
SPR_CHILL(minienforcer, Y(MENF_STILL,41,19),)
SPR_WALK(minienforcer, 6, Y(MENF_MOV1,46,20), Y(MENF_MOV2,49,20), Y(MENF_MOV1,46,20), Y(MENF_STILL,41,19),
                   Y(MENF_MOV3,33,20), Y(MENF_MOV4,30,21), Y(MENF_MOV3,33,20), Y(MENF_STILL,41,19),)
SPR_EAT(minienforcer, Y(MENF_STILL,41,17),)
SPR_ATTACK(minienforcer, Y(MENF_STILL,41,17),)
SPR_DIE(minienforcer, W(DEBRIES),)
*/

SPR_BIG(squadship, SQUADSHIPF)
SPR_DIE(squadship, W(DEBRIES),)
SPR_CHILL(squadship, Y(SQUADSHIP1, 18,13), Y(SQUADSHIP2, 18, 13),)
static int squadship_walk_span = 10;
#define squadship_spr_walk   squadship_spr_chill
#define squadship_spr_eat    squadship_spr_chill
#define squadship_spr_attack squadship_spr_chill
NO_SOUND(squadship)


SPR_BIG(ufo, CRY_SHIPF)
SPR_CHILL(ufo, W(CRY_SHIP),)
SPR_WALK(ufo, 6, W(CRY_SHIP),)
SPR_EAT(ufo, W(CRY_SHIP),)
SPR_ATTACK(ufo, W(CRY_SHIP),)
SPR_DIE(ufo, W(CRASHSITE),)
NO_SOUND(ufo)

///////// STRAPPED HUMANOIDS //////////
SPR_BIG(hunter, HUNTER)
SPR_CHILL(hunter, Y(HNT_STILL,27,26),) // wanders
SPR_WALK(hunter, 10, Y(HNT_MOV1,27,28), Y(HNT_MOV2,27,26),);
SPR_EAT(hunter, W(HNT_ATK),)  // filler
SPR_ATTACK(hunter, G(HNT_ATK, 100, 27,28), G(HNT_STILL, 80, 27, 26),);
SPR_DIE(hunter, W(HNT_DEAD),);
SOUNDTAB(hunter, 0, 0, SND_MAN_DIE, SND_MAN_HRT, SND_MAN_FRK, 0);

SPR_BIG(sentry, SENTRY)
SPR_CHILL(sentry, Y(STY_STILL,27,26),) // wanders
SPR_WALK(sentry, 10, Y(STY_MOV1,27,28), Y(STY_MOV2,27,26),);
SPR_EAT(sentry, W(STY_ATK),)  // filler
SPR_ATTACK(sentry, G(STY_ATK, 100, 27,28), G(STY_STILL, 80,27,28),);
SPR_DIE(sentry, W(STY_DEAD),);
SOUNDTAB(sentry, 0, 0, SND_MAN_DIE, SND_MAN_HRT, SND_MAN_FRK, 0);

SPR_BIG(sentinel, SENTINEL)
SPR_CHILL(sentinel, Y(STL_STILL,27,30),) // wanders
SPR_WALK(sentinel, 10, Y(STL_MOV1,27,30), Y(STL_MOV2,27,31),);
SPR_EAT(sentinel, W(STL_ATK),)  // filler
SPR_ATTACK(sentinel, G(STL_ATK, 60, 27, 31), G(STL_STILL, 20, 27, 30),);
SPR_DIE(sentinel, W(STL_DEAD),);
SOUNDTAB(sentinel, 0, 0, SND_MAN_DIE, SND_MAN_HRT, SND_MAN_FRK, 0);

SPR_BIG(worker, SENTRY)
SPR_CHILL(worker, Y(STL_STILL,27,30),) // workers wear martec uniforms for now.
SPR_WALK(worker, 10, Y(STL_MOV1,27,30), Y(STL_MOV2,27,31),);
SPR_EAT(worker, W(STL_ATK),)  // filler
SPR_ATTACK(worker, G(STL_ATK, 60, 27, 31), G(STL_STILL, 20, 27, 30),);
SPR_DIE(worker, W(STL_DEAD),);
SOUNDTAB(worker, 0, 0, SND_MAN_DIE, SND_MAN_HRT, SND_MAN_FRK, 0);

SPR_BIG(raider, RAIDER)
SPR_CHILL(raider, Y(RAD_STILL,27,26),) // wanders
SPR_WALK(raider, 10, Y(RAD_MOV1,27,28), Y(RAD_MOV2,27,26),);
SPR_EAT(raider, W(RAD_ATK),)  // filler
SPR_ATTACK(raider, G(RAD_ATK, 80, 27,28), G(RAD_STILL, 20, 27, 28),);
SPR_DIE(raider, W(RAD_DEAD),);
SOUNDTAB(raider, 0, 0, SND_MAN_DIE, SND_MAN_HRT, SND_MAN_FRK, 0);

SPR_BIG(orick, ORICK_BANCLAW)  // spr_big deffined by Orick tribe (Banclaw, Dracomajia, Grelblade, Monrojo, & Skulibula)
SPR_CHILL(orick, W(ORC_STILL_AXE),)
SPR_WALK(orick, 10, W(ORC_MOV1_AXE), W(ORC_STILL_AXE), W(ORC_MOV2_AXE), W(ORC_STILL_AXE),);
SPR_EAT(orick, A(ORC_ATK2_SPEAR, 100), A(ORC_ATK3_SPEAR, 100),)
SPR_ATTACK(orick, A(ORC_ATK1_AXE, 100), A(ORC_ATK2_AXE, 100), A(ORC_ATK3_AXE, 100), A(ORC_STILL_AXE, 100),);
SPR_DIE(orick, W(ORC_DEAD),);
aspath orick_spear_spr_chill[] = {W(ORC_STILL_SPEAR), ENDASL};
aspath orick_spear_spr_walk[] = { W(ORC_MOV1_SPEAR), W(ORC_STILL_SPEAR), W(ORC_MOV2_SPEAR), W(ORC_STILL_SPEAR), ENDASL };
aspath orick_spear_spr_attack[] = { A(ORC_ATK1_SPEAR, 100), A(ORC_ATK2_SPEAR, 100), A(ORC_ATK3_SPEAR, 100), A(ORC_STILL_SPEAR, 100), ENDASL};
aspath orick_gun_spr_chill[] = {  Y(ORC_STILL_GUN,34,27),  ENDASL};
aspath orick_gun_spr_walk[] = { Y(ORC_MOV1_GUN,35,26), Y(ORC_STILL_GUN,34,26), Y(ORC_MOV2_GUN,33,26), Y(ORC_STILL_GUN,34,26), ENDASL};
aspath orick_gun_spr_attack[] = {  G(ORC_ATK_GUN, 100,34,26), G(ORC_STILL_GUN, 80,34,27), ENDASL};
SOUNDTAB(orick, 0, 0, SND_MAN_DIE, SND_MAN_HRT, SND_MAN_FRK, SND_MED_EAT);

struct orick_clan_t orick_clan[] = {
  {SPR_ORICK_BANCLAW,    (char *)"Banclaw", &White},
  {SPR_ORICK_DRACOMAJIA, (char *)"Dracomajia", &Purple},
  {SPR_ORICK_GRELBLADE,  (char *)"Grelblade", &Green},
  {SPR_ORICK_MONROJO,    (char *)"Monrojo", &Red},
  {SPR_ORICK_SKULIBULA,  (char *)"Skulibula", &Blue},
};


/////////////// ANIMATED AST 0  EQUIP ///////////////////
SPR_BIG(radiomarker, SCAN0_INV) SPR_DIE(radiomarker, W(DEBRIES),)
SPR_CHILL(radiomarker, A(SCAN0_00, 1000), A(SCAN0_01, 250),)

SPR_BIG(organicsscanner, SCAN1_INV) SPR_DIE(organicsscanner, W(DEBRIES),)
SPR_CHILL(organicsscanner, A(SCAN1_00, 125), A(SCAN1_01, 125), A(SCAN1_02, 125),
                 A(SCAN1_03, 125), A(SCAN1_04, 125), A(SCAN1_05, 125),
                 A(SCAN1_06, 125), A(SCAN1_07, 125), A(SCAN1_08, 125),
                 A(SCAN1_09, 125), A(SCAN1_10, 125), A(SCAN1_11, 125),)


SPR_BIG(deltawavescanner, SCAN2_INV) SPR_DIE(deltawavescanner, W(DEBRIES),)
SPR_CHILL(deltawavescanner, A(SCAN2_00, 125), A(SCAN2_01, 125), A(SCAN2_02, 125),
                 A(SCAN2_03, 125), A(SCAN2_04, 125), A(SCAN2_05, 125),
                 A(SCAN2_06, 125), A(SCAN2_07, 125),)

SPR_BIG(qparticlescanner, SCAN3_INV) SPR_DIE(qparticlescanner, W(DEBRIES),)
SPR_CHILL(qparticlescanner, A(SCAN3_00, 125), A(SCAN3_01, 125), A(SCAN3_02, 125),
                 A(SCAN3_03, 125), A(SCAN3_04, 125), A(SCAN3_05, 125),)


SPR_BIG(epsilonhiscanner, SCAN4_INV) SPR_DIE(epsilonhiscanner, W(DEBRIES),)
SPR_CHILL(epsilonhiscanner, A(SCAN4_00, 125), A(SCAN4_01, 125), A(SCAN4_02, 125),
                 A(SCAN4_03, 125),A(SCAN4_02, 125), A(SCAN4_01, 125),)

SPR_BIG(solararray, BGEN1_INV) SPR_DIE(solararray, W(DEBRIES),)
SPR_CHILL(solararray, A(BGEN1_1, 1000), A(BGEN1_2, 200),)

SPR_BIG(fissionplant, BGEN2_INV) SPR_DIE(fissionplant, W(DEBRIES),)
SPR_CHILL(fissionplant, A(BGEN2_1, 200), A(BGEN2_2, 200), A(BGEN2_3, 200),A(BGEN2_2, 200),)

SPR_BIG(fusionplant, BGEN3_INV) SPR_DIE(fusionplant, W(DEBRIES),)
SPR_CHILL(fusionplant, A(BGEN3_1, 200), A(BGEN3_2, 200), A(BGEN3_3, 200),
                        A(BGEN3_4, 200),  A(BGEN3_5, 200),  A(BGEN3_6, 200),)


//////// ANIMATED RAIDER EQUIPMENT ////////////
SPR_BIG(raidersolararray, RGEN1_INV) SPR_DIE(raidersolararray, W(DEBRIES),)
SPR_CHILL(raidersolararray, A(RGEN1_1, 1000), A(RGEN1_2, 200),)

SPR_BIG(raiderfissionplant, RGEN2_INV) SPR_DIE(raiderfissionplant, W(DEBRIES),)
SPR_CHILL(raiderfissionplant, A(RGEN2_1, 200), A(RGEN2_2, 200), A(RGEN2_3, 200),A(RGEN2_2, 200),)

SPR_BIG(raiderfusionplant, RGEN3_INV) SPR_DIE(raiderfusionplant, W(DEBRIES),)
SPR_CHILL(raiderfusionplant, A(RGEN3_1, 200), A(RGEN3_2, 200), A(RGEN3_3, 200),
                        A(RGEN3_4, 200),  A(RGEN3_5, 200),  A(RGEN3_6, 200),)

//////////// MISC ANIMATED BUILDINGS ////////////

SPR_BIG(wisehut, WIZE_GUYF) SPR_DIE(wisehut, W(DEBRIES),)
SPR_CHILL(wisehut, A(WIZE_GUY1,100),A(WIZE_GUY2,100),A(WIZE_GUY3,100),)

SPR_BIG(orickcampfire, CAMPFIREF) SPR_DIE(orickcampfire, W(DEBRIES),)
SPR_CHILL(orickcampfire, A(CAMPFIRE1, 200), A(CAMPFIRE2, 200), A(CAMPFIRE3, 200),)


/////////// MISC ANIMATED ITEMS ////////////
SPR_BIG(hole, HOLEF) SPR_CHILL(hole, A(HOLEEYES, 1800), A(HOLE, 200),) SPR_DIE(hole, W(HOLE),)

SPR_BIG(money, MONEY_INV) SPR_DIE(money, W(DEBRIES),)
SPR_CHILL(money, A(MONEY1, 220), A(MONEY2, 220), A(MONEY3, 220),A(MONEY4, 220), A(MONEY5, 220), A(MONEY6, 200),)


SPR_BIG(powercore, CGEN_INV) SPR_DIE(powercore, W(DEBRIES),)
SPR_CHILL(powercore, A(CGEN_1, 200), A(CGEN_2, 200), A(CGEN_3, 200), A(CGEN_2, 200),)

SPR_BIG(meteor, AA0001)
SPR_CHILL(meteor, W(AA0001),)
SPR_WALK(meteor, 10, W(AA0001), W(AA0002), W(AA0003), W(AA0004), W(AA0005), W(AA0006), W(AA0007), W(AA0008), W(AA0009), W(AA0010), W(AA0011), W(AA0012), W(AA0013), W(AA0014),)
SPR_EAT(meteor, )
SPR_ATTACK(meteor, )
SPR_DIE(meteor, W(HOLE),)
SOUNDTAB(meteor, 0,0, SND_SMASHCAGE,0,0,0)

SPR_BIG(cheech, CHEECH)
SPR_CHILL(cheech, A(CCH_STILL1, 2200),A(CCH_STILL2, 800),
                  A(CCH_STILL1, 1200),A(CCH_STILL2, 2200),A(CCH_STILL1, 3200),A(CCH_STILL2, 1400),)
SPR_WALK(cheech, 22, W(CCH_MOV1), W(CCH_STILL2), W(CCH_MOV2), W(CCH_STILL1),)
SPR_EAT(cheech, W(CCH_ATK),)
SPR_ATTACK(cheech, A(CCH_ATK, 100), A(CCH_STILL1, 80),)
SPR_DIE(cheech, W(CCH_DEAD),)
SOUNDTAB(cheech, SND_CCH_BRK, 150000, SND_COH_DIE, SND_COH_HRT, SND_COH_FRK, SND_SML_EAT);


SPR_BIG(hoveler, HOVELER)
SPR_CHILL(hoveler, W(HOV_STILL),)
SPR_WALK(hoveler, 10, W(HOV_MOV1), W(HOV_STILL), W(HOV_MOV2), W(HOV_STILL),)
SPR_EAT(hoveler, W(HOV_ATK),)
SPR_ATTACK(hoveler, A(HOV_ATK, 100), A(HOV_STILL, 80),)
SPR_DIE(hoveler, W(HOV_DEAD),)
SOUNDTAB(hoveler, SND_HOV_BRK, 150000, SND_COH_DIE, SND_COH_HRT, SND_COH_FRK, SND_SML_EAT);

SPR_BIG(hippie, HIPPI)
SPR_CHILL(hippie, A(HIP_STILL1, 350),A(HIP_STILL2, 350),)
SPR_WALK(hippie, 10, W(HIP_MOV1), W(HIP_STILL1), W(HIP_MOV2), W(HIP_STILL1),)
SPR_EAT(hippie, A(HIP_STILL1, 350),A(HIP_STILL2, 350),)
SPR_ATTACK(hippie, A(HIP_STILL1, 350),A(HIP_STILL2, 350),)
SPR_DIE(hippie, W(HIP_DEAD),)
SOUNDTAB(hippie, SND_HIP_BRK, 50000, SND_MAN_DIE, SND_MAN_HRT, SND_MAN_FRK, 0);

SPR_BIG(shrub, SHRUB_PIC)
SPR_CHILL(shrub, W(SHRUB),)
SPR_WALK(shrub, 0,)
SPR_EAT(shrub, )
SPR_ATTACK(shrub, )
SPR_DIE(shrub, W(SHRUB_DEAD),)
NO_SOUND(shrub)

SPR_BIG(stalkplant, STALKPLANT)
SPR_CHILL(stalkplant, A(STALK_CHILL1, 5200),  A(STALK_CHILL2, 200),)
SPR_WALK(stalkplant, 0,)
SPR_EAT(stalkplant, )
SPR_ATTACK(stalkplant, )
SPR_DIE(stalkplant, W(STALK_DEAD),)
NO_SOUND(stalkplant)

SPR_BIG(fern, FERN_PIC)
SPR_CHILL(fern, W(FERN),)
SPR_WALK(fern, 0,)
SPR_EAT(fern, )
SPR_ATTACK(fern, )
SPR_DIE(fern, W(FERN_DEAD),)
NO_SOUND(fern)


SPR_BIG(happytree, HTREE_PIC)
SPR_CHILL(happytree,
	A(HTREE_STILL1, 2200),  A(HTREE_STILL2,  300),
	A(HTREE_STILL1, 2200),  A(HTREE_STILL2,  300),
        A(HTREE_STILL1, 1000),  A(HTREE_STILL2,  300),
        A(HTREE_STILL1, 5000),  A(HTREE_STILL2,  300),
        A(HTREE_STILL3,  300),  A(HTREE_STILL2,  300),
        A(HTREE_STILL3,  300),  A(HTREE_STILL2,  300),
        A(HTREE_STILL3,  300),  A(HTREE_STILL2,  300),
        A(HTREE_STILL3,  300),  A(HTREE_STILL2,  300),
        A(HTREE_STILL3,  300),  A(HTREE_STILL2,  300),)
SPR_WALK(happytree, 0,)   SPR_EAT(happytree, )
SPR_ATTACK(happytree, )
SPR_DIE(happytree, W(HTREE_DEAD),)
NO_SOUND(happytree)

SPR_BIG(mimmack, MIMMACK)
SPR_CHILL(mimmack,
	A(MIM_STILL, 2200),  A(HTREE_STILL2,  300),
	A(MIM_STILL, 2200),  A(HTREE_STILL2,  300),
	A(MIM_STILL, 2200),  A(HTREE_STILL2,  300),
        A(MIM_STILL, 1000),  A(HTREE_STILL2,  300),
        A(MIM_STILL, 5000),  A(HTREE_STILL2,  300),)
SPR_WALK(mimmack, 10, W(MIM_MOV1), W(MIM_MOV2), W(MIM_MOV3),
W(MIM_MOV2),)
SPR_EAT(mimmack, ) // photosynthetic
SPR_ATTACK(mimmack, A(MIM_ATK2, 200), A(MIM_ATK1,  200),)
SPR_DIE(mimmack, W(HTREE_DEAD),)
SOUNDTAB(mimmack, SND_SQR_BRK, 300000, 0, 0, 0, 0)

SPR_BIG(poisonshroom, POISENSF)
SPR_CHILL(poisonshroom, W(POISENS),)
SPR_WALK(poisonshroom, 0, W(POISENS),)
SPR_EAT(poisonshroom, W(POISENS),)
SPR_ATTACK(poisonshroom, W(POISENS),)
SPR_DIE(poisonshroom, W(POISENS),)
NO_SOUND(poisonshroom)

SPR_BIG(psychoshroom, PSYCOSF)
SPR_CHILL(psychoshroom, W(PSYCOS),)
SPR_WALK(psychoshroom, 0, W(PSYCOS),)
SPR_EAT(psychoshroom, W(PSYCOS),)
SPR_ATTACK(psychoshroom, W(PSYCOS),)
SPR_DIE(psychoshroom, W(PSYCOS),)
NO_SOUND(psychoshroom)

SPR_BIG(pyroshroom, PYROSF)
SPR_CHILL(pyroshroom, W(PYROS),)
SPR_WALK(pyroshroom, 0, W(PYROS),)
SPR_EAT(pyroshroom, W(PYROS),)
SPR_ATTACK(pyroshroom, W(PYROS),)
SPR_DIE(pyroshroom, W(PYROS),)
NO_SOUND(pyroshroom)



SPR_BIG(furbit, FIRBIT)
SPR_CHILL(furbit, A(FIRBIT_STILL,1200), A(FIRBIT_STILL2,200), A(FIRBIT_STILL,3200), A(FIRBIT_STILL2,200),)
SPR_WALK(furbit, 10, W(FIRBIT_MOV1), W(FIRBIT_STILL), W(FIRBIT_MOV2),)
SPR_EAT(furbit, A(FIRBIT_EAT1,   200),  A(FIRBIT_EAT2,  200),)
SPR_ATTACK(furbit, A(FIRBIT_EAT1,   200),  A(FIRBIT_EAT2,  200),)
SPR_DIE(furbit, W(FIRBIT_DEAD),)
SOUNDTAB(furbit, SND_RAT_BRK, 150000, SND_SML_DIE, SND_RAT_HRT, 0, SND_SML_EAT)

SPR_BIG(pygment, PYGMENT)
SPR_CHILL(pygment, W(PYG_STILL),)
SPR_WALK(pygment, 10, W(PYG_MOV1), W(PYG_STILL), W(PYG_MOV2),)
SPR_EAT(pygment, A(PYG_ATK2,   3200),  A(PYG_MOV2,  200),)
SPR_ATTACK(pygment, A(PYG_ATK1, 200), A(PYG_ATK2, 300),)
SPR_DIE(pygment, W(PYG_DEAD),)
SOUNDTAB(pygment, SND_BUG_BRK, 150000, SND_BUG_DIE, 0, 0, SND_BUG_EAT)

SPR_BIG(shruver, SHRUVER)
SPR_CHILL(shruver, W(SHV_STILL),)
SPR_WALK(shruver, 10, W(SHV_MOV1), W(SHV_STILL), W(SHV_MOV2), W(SHV_STILL),)
SPR_EAT(shruver, A(SHV_EAT1,   3200),  A(SHV_EAT2,  200),)
SPR_ATTACK(shruver, A(SHV_EAT1, 200), A(SHV_EAT2, 300),)
SPR_DIE(shruver, A(SHV_DEAD, 500),)
SOUNDTAB(shruver, SND_RAT_BRK, 150000, SND_SML_DIE, SND_RAT_HRT, 0, SND_SML_EAT)

SPR_BIG(radrunner, RADRUNNER)
SPR_CHILL(radrunner, W(RAR_STILL),)
SPR_WALK(radrunner, 20, W(RAR_MOV1), W(RAR_STILL), W(RAR_MOV2), W(RAR_STILL),)
SPR_EAT(radrunner, A(RAR_ATK,   3200),  A(RAR_STILL,  200),)
SPR_ATTACK(radrunner, A(RAR_ATK,   3200),  A(RAR_STILL,  200),)
SPR_DIE(radrunner, A(RAR_DEAD, 500),)
SOUNDTAB(radrunner, SND_RAR_BRK, 150000, SND_SML_DIE, SND_RAR_HRT, 0, SND_SML_EAT)

SPR_BIG(roachy, ROACHY)
SPR_CHILL(roachy, W(RCH_STILL),)
SPR_WALK(roachy, 10, W(RCH_MOV1), W(RCH_MOV2), W(RCH_STILL),)
SPR_EAT(roachy, A(RCH_ATK,   400),  A(RCH_STILL,  200), A(RCH_ATK,   400),  A(RCH_STILL,  200),)
SPR_ATTACK(roachy, A(RCH_ATK, 200), A(RCH_STILL, 300),)
SPR_DIE(roachy, A(RCH_DEAD, 500),)
SOUNDTAB(roachy, SND_BUG_BRK, 150000, SND_BUG_DIE, 0, 0, SND_BUG_EAT)

SPR_BIG(krulasite, KRULASITE)
SPR_CHILL(krulasite, W(KRL_STILL),)
SPR_WALK(krulasite, 10, W(KRL_MOV1), W(KRL_MOV2), W(KRL_STILL),)
SPR_EAT(krulasite, W(KRL_STILL),)
SPR_ATTACK(krulasite, W(KRL_DEAD),)
SPR_DIE(krulasite, W(KRL_DEAD),)
SOUNDTAB(krulasite, SND_BUG_BRK, 150000, SND_BUG_DIE, 0, 0, SND_BUG_EAT)

SPR_BIG(mubark, MUBARK)
SPR_CHILL(mubark, W(MBK_STILL),)
SPR_WALK(mubark, 10, W(MBK_MOV1), W(MBK_STILL), W(MBK_MOV2),W(MBK_STILL),)
SPR_EAT(mubark, A(MBK_ATK,   3200),  A(MBK_STILL,  200),)
SPR_ATTACK(mubark, A(MBK_ATK, 200), A(MBK_STILL, 300),)
SPR_DIE(mubark, A(MBK_DEAD, 500),)
SOUNDTAB(mubark, SND_MBK_BRK, 150000, SND_MBK_DIE, SND_MBK_HRT, 0, SND_HRB_EAT)

SPR_BIG(throbid, THROBID)
SPR_CHILL(throbid, W(THR_STILL),)
SPR_WALK(throbid, 10, W(THR_MOV1), W(THR_STILL), W(THR_MOV2), W(THR_STILL),)
SPR_EAT(throbid, A(THR_ATK1, 200), A(THR_ATK2, 300), A(THR_ATK1, 200), A(THR_STILL, 200),)
SPR_ATTACK(throbid, A(THR_ATK1, 125), A(THR_ATK2, 125), A(THR_ATK1, 125), A(THR_STILL, 125),)
SPR_DIE(throbid, A(THR_DEAD, 500),)
SOUNDTAB(throbid, SND_THR_BRK, 150000, SND_REP_DIE, SND_REP_HRT, 0, SND_MED_EAT)

SPR_BIG(malcumbit, MALACUMEBIT)
SPR_CHILL(malcumbit, A(MAL_STILL1, 5000), A(MAL_STILL2, 500), A(MAL_STILL3, 200),
                       A(MAL_STILL2, 500), A(MAL_STILL4, 200), A(MAL_STILL1, 9000),
                       A(MAL_STILL2, 1500), A(MAL_STILL3, 200), A(MAL_STILL2, 500), A(MAL_STILL4, 200),)
SPR_WALK(malcumbit, 10, W(MAL_MOV1), W(MAL_MOV2), W(MAL_MOV3), W(MAL_MOV2), W(MAL_MOV1),
                      W(MAL_MOV4), W(MAL_MOV5), W(MAL_MOV4),)
SPR_EAT(malcumbit, A(MAL_MOV1, 200), A(MAL_EAT1, 200), A(MAL_EAT2, 200), A(MAL_EAT1, 200),)
SPR_ATTACK(malcumbit, A(MAL_ATK1, 125), A(MAL_ATK2, 125), A(MAL_ATK3, 125), A(MAL_MOV2, 125),)
SPR_DIE(malcumbit, A(MAL_DEAD, 500),)
SOUNDTAB(malcumbit, SND_MAL_BRK, 150000, SND_MED_DIE, SND_REP_HRT, 0, SND_MED_EAT)

SPR_BIG(squarger, SQUARGER)
SPR_CHILL(squarger, W(SQR_STILL),)
SPR_WALK(squarger, 10, W(SQR_MOV1), W(SQR_STILL), W(SQR_MOV2), W(SQR_STILL),)
SPR_EAT(squarger, A(SQR_ATK1, 300), A(SQR_ATK2, 300),)
SPR_ATTACK(squarger, A(SQR_ATK1, 180), A(SQR_ATK2, 180), A(SQR_STILL, 180),)
SPR_DIE(squarger, A(SQR_STILL, 500),)
SOUNDTAB(squarger, SND_SQR_BRK, 150000, 0, 0, 0, SND_HRB_EAT)

SPR_BIG(quitofly, QUITOFLY)
SPR_CHILL(quitofly, A(QTF_MOV2, 2000), A(QTF_MOV3,2000),)
SPR_WALK(quitofly, 10, W(QTF_MOV1), W(QTF_MOV2), W(QTF_MOV3), W(QTF_MOV2),)
SPR_EAT(quitofly, A(QTF_MOV1,100), A(QTF_MOV2,100), A(QTF_MOV3,100), A(QTF_MOV2,100),)
SPR_ATTACK(quitofly, A(QTF_MOV1,100), A(QTF_MOV2,100), A(QTF_MOV3,100), A(QTF_MOV2,100),)
SPR_DIE(quitofly, A(QTF_MOV1,500),)
SOUNDTAB(quitofly, SND_QTF_BRK, 150000, SND_BUG_DIE, 0, 0, SND_BUG_EAT)

SPR_BIG(mamothquito, MAMOTHQUITO)
SPR_CHILL(mamothquito, A(MMQ_MOV1, 2000), A(MMQ_MOV2,2000),)
SPR_WALK(mamothquito, 10, W(MMQ_MOV1), W(MMQ_MOV2), W(MMQ_MOV1), W(MMQ_MOV3),)
SPR_EAT(mamothquito, A(MMQ_ATK1,100), A(MMQ_MOV1,100),)
SPR_ATTACK(mamothquito, A(MMQ_ATK1,100), A(MMQ_MOV2,100),)
SPR_DIE(mamothquito, A(MMQ_MOV1,500),)
SOUNDTAB(mamothquito, SND_MMQ_BRK, 150000, SND_SLM_DIE, 0, 0, SND_BUG_EAT)


SPR_BIG(toothworm, TOOTHWORM)
SPR_CHILL(toothworm, A(TWM_STILL,290), A(TWM_MOV1,260), A(TWM_STILL,290), A(TWM_MOV3,260),)
SPR_WALK(toothworm, 10, W(TWM_MOV1), W(TWM_MOV2), W(TWM_MOV1), W(TWM_STILL),
                    W(TWM_MOV3), W(TWM_MOV4), W(TWM_MOV3), W(TWM_STILL),)
SPR_EAT(toothworm, A(TWM_ATK1, 300), A(TWM_ATK2, 200), A(TWM_STILL,200), )
SPR_ATTACK(toothworm,  A(TWM_ATK1, 250), A(TWM_ATK2, 200), A(TWM_STILL, 150), )
SPR_DIE(toothworm, A(TWM_STILL,500),)
SOUNDTAB(toothworm, 0, 0, SND_MED_DIE, SND_REP_HRT, 0, SND_MED_EAT)


SPR_BIG(sneeker, SNEEKER)
SPR_CHILL(sneeker,  A(SNK_STILL1, 1200), A(SNK_STILL2,	200),
                    A(SNK_STILL1,  200), A(SNK_STILL3,  200),
	            A(SNK_STILL1, 1000), A(SNK_STILL2,  300),
                    A(SNK_STILL1, 1000), A(SNK_STILL3,  300),)
SPR_WALK(sneeker, 10, W(SNK_MOV1), W(SNK_STILL1), W(SNK_MOV2), W(SNK_STILL1),)
SPR_EAT(sneeker,  A(SNK_ATK2, 3200), A(SNK_ATK1, 200), A(SNK_STILL1,  200),
                  A(SNK_STILL3,  300), A(SNK_STILL1,  200), A(SNK_STILL2, 300), A(SNK_STILL1,  200),)
SPR_ATTACK(sneeker, A(SNK_ATK1, 150), A(SNK_ATK2,  150), A(SNK_STILL1,  200), )
SPR_DIE(sneeker, A(SNK_DEAD, 500),)
SOUNDTAB(sneeker, SND_SNK_BRK, 150000, SND_EEK_DIE, SND_EEK_HRT, 0, SND_SML_EAT)


SPR_BIG(squeeker, SQUEEKER)
SPR_CHILL(squeeker,  A(SQU_STILL1, 1200), A(SQU_STILL2,  200),
                     A(SQU_STILL1,  200), A(SQU_STILL3,  200),
	             A(SQU_STILL1, 1000), A(SQU_STILL2,  300),
                     A(SQU_STILL1, 1000), A(SQU_STILL3,  300),)
SPR_WALK(squeeker, 10, W(SQU_MOV1), W(SQU_STILL1), W(SQU_MOV2),)
SPR_EAT(squeeker,  A(SQU_ATK2,	 3200),   A(SQU_ATK1,	 200),
                   A(SQU_STILL1,  200),  A(SQU_STILL3,  300), A(SQU_STILL1,  200),
                      A(SQU_STILL2, 300), A(SQU_STILL1,  200),)
SPR_ATTACK(squeeker, A(SQU_ATK1, 150), A(SQU_ATK2,  150), A(SNK_STILL1,  200), )
SPR_DIE(squeeker, A(SQU_DEAD, 500),)
SOUNDTAB(squeeker, SND_SQU_BRK, 150000, SND_EEK_DIE, SND_EEK_HRT, 0, SND_SML_EAT)


SPR_BIG(meeker, MEEKER)
SPR_CHILL(meeker,  A(MEK_STILL1, 1200), A(MEK_STILL2,  200),
                   A(MEK_STILL1,  200),  A(MEK_STILL3,  200),
	           A(MEK_STILL1, 1000),  A(MEK_STILL2,  300),
                   A(MEK_STILL1, 1000),	A(MEK_STILL3,  300),)
SPR_WALK(meeker, 10, W(MEK_MOV1), W(MEK_STILL1), W(MEK_MOV2),)
SPR_EAT(meeker,  A(MEK_ATK2, 3200), A(MEK_ATK1, 200), A(MEK_STILL1, 200),
                 A(MEK_STILL3,  300), A(MEK_STILL1,  200), A(MEK_STILL2, 300),
                 A(MEK_STILL1,  200),)
SPR_ATTACK(meeker, A(MEK_ATK1, 200), A(MEK_ATK2,  200),)
SPR_DIE(meeker, A(MEK_DEAD, 500),)
SOUNDTAB(meeker, SND_MEK_BRK, 150000, SND_EEK_DIE, SND_EEK_HRT, 0, SND_SML_EAT)


SPR_BIG(molber, MOLBER)
SPR_CHILL(molber, A(MOL_ATK1, 125), A(MOL_ATK2, 300), A(MOL_ATK1, 125), A(MOL_STILL, 6000), )
SPR_WALK(molber, 10, W(MOL_MOV1), W(MOL_MOV2), W(MOL_MOV1), W(MOL_STILL),
                    W(MOL_MOV3), W(MOL_MOV4), W(MOL_MOV3), W(MOL_STILL),)
SPR_EAT(molber, A(MOL_EAT1, 300), A(MOL_STILL,300), A(MOL_EAT2, 300), A(MOL_STILL,300), )
SPR_ATTACK(molber, A(MOL_ATK1, 125), A(MOL_ATK2, 125), A(MOL_ATK1, 125), A(MOL_STILL, 125), )
SPR_DIE(molber, A(MOL_DEAD, 500), )
SOUNDTAB(molber, SND_MOL_BRK, 150000, SND_MBK_DIE, SND_MOL_BRK, 0, SND_HRB_EAT)



SPR_BIG(drexler, DREXLER)
SPR_CHILL(drexler, W(DRX_STILL), )
SPR_WALK(drexler, 10, W(DRX_MOV1), W(DRX_MOV2), W(DRX_MOV3), W(DRX_STILL), )
SPR_EAT(drexler, A(DRX_EAT1, 250), A(DRX_EAT1, 250), )
SPR_ATTACK(drexler, A(DRX_ATK1, 125), A(DRX_ATK2, 125), A(DRX_ATK1, 125), A(DRX_STILL, 125), )
SPR_DIE(drexler, A(DRX_DEAD, 500), )
SOUNDTAB(drexler, SND_BUG_BRK, 150000, SND_REP_HRT, 0, 0, SND_BUG_EAT)


SPR_BIG(bigasaurus, BIGASAURUS)
SPR_CHILL(bigasaurus, A(BIG_STILL, 5000), A(BIG_ATK3, 250),)
SPR_WALK(bigasaurus, 10, W(BIG_MOV1), W(BIG_MOV2), W(BIG_MOV1), W(BIG_STILL),
                    W(BIG_MOV3), W(BIG_MOV4), W(BIG_MOV3), W(BIG_STILL), )
SPR_EAT(bigasaurus, A(BIG_ATK1, 150), A(BIG_ATK2, 150), A(BIG_ATK3, 150), )
SPR_ATTACK(bigasaurus, A(BIG_ATK1, 150), A(BIG_ATK2, 150), A(BIG_ATK3, 150), )
SPR_DIE(bigasaurus, W(BIG_DEAD), )
SOUNDTAB(bigasaurus, SND_BIG_BRK, 150000, SND_BIG_DIE, SND_BIG_HRT, 0, SND_BIG_EAT)


SPR_BIG(uberworm, UBERWORM)
SPR_CHILL(uberworm, A(UWM_STILL,5000), A(UWM_ATK3,200),)
SPR_WALK(uberworm, 30, W(UWM_MOV1), W(UWM_MOV2), W(UWM_MOV3), W(UWM_MOV4), W(UWM_MOV5), W(UWM_MOV6), )
SPR_EAT(uberworm, A(UWM_ATK2,200),A(UWM_ATK3,200),)
SPR_ATTACK(uberworm, A(UWM_ATK1,200),A(UWM_ATK2,200),A(UWM_ATK3,200), A(UWM_STILL,200),)
SPR_DIE(uberworm, A(UWM_MOV5, 250), W(UWM_DEAD), )
SOUNDTAB(uberworm, SND_UWM_BRK, 150000, SND_BIG_DIE, SND_REP_HRT, 0, SND_MED_EAT)


SPR_BIG(dragoonigon, DRAGOONIGON)
SPR_CHILL(dragoonigon, A(DRA_MOV1,300), A(DRA_MOV2,300), A(DRA_MOV3,300), A(DRA_MOV4,300),)
SPR_WALK(dragoonigon, 40, W(DRA_MOV1), W(DRA_MOV2), W(DRA_MOV3), W(DRA_MOV4),)
SPR_EAT(dragoonigon, A(DRA_MOV3, 100), A(DRA_MOV4, 100),)
SPR_ATTACK(dragoonigon, A(DRA_ATK1,125), A(DRA_ATK2,125), A(DRA_ATK3,125), A(DRA_ATK4,125),)
SPR_DIE(dragoonigon, W(DRA_DEAD), )
SOUNDTAB(dragoonigon, SND_DRA_BRK, 150000, SND_BIG_DIE, SND_REP_HRT, 0, SND_BIG_EAT)


SPR_BIG(krakin, KRAKIN)
SPR_CHILL(krakin, W(KRK_STILL),) // wanders
SPR_WALK(krakin, 10, W(KRK_MOV1), W(KRK_MOV2), W(KRK_MOV3), W(KRK_MOV1),
                 W(KRK_MOV4), W(KRK_MOV5), W(KRK_MOV6), W(KRK_MOV4),)
SPR_EAT(krakin, A(KRK_MOV1, 200), A(KRK_MOV4, 200),)
SPR_ATTACK(krakin, A(KRK_ATK1, 200), A(KRK_ATK2, 200), A(KRK_ATK3, 200), A(KRK_STILL, 200),)
SPR_DIE(krakin, A(KRK_ATK3, 200), A(KRK_DEAD1, 200), W(KRK_DEAD2),)
SOUNDTAB(krakin, SND_KRK_BRK, 150000, SND_BIG_DIE, SND_KRK_HRT, 0, SND_MED_EAT)


SPR_BIG(bervul, BERVUL)
SPR_CHILL(bervul, A(BRV_STILL1, 125), A(BRV_STILL2, 125), A(BRV_STILL3, 125), A(BRV_STILL2, 125),)
SPR_WALK(bervul, 10, W(BRV_MOV1), W(BRV_MOV2), W(BRV_MOV3),)
SPR_EAT(bervul, A(BRV_EAT,400), A(BRV_MOV1, 125),)
SPR_ATTACK(bervul, A(BRV_MOV1, 125), A(BRV_MOV2, 125), A(BRV_MOV3, 125),)
SPR_DIE(bervul, W(BRV_DEAD),)
SOUNDTAB(bervul, SND_BRV_BRK, 150000, SND_SML_DIE, SND_MBK_HRT, 0, SND_SML_EAT)


SPR_BIG(rexrex, REXREX)
SPR_CHILL(rexrex, W(RRX_STILL),) // wanders
SPR_WALK(rexrex, 10, W(RRX_MOV1), W(RRX_STILL), W(RRX_MOV2), W(RRX_STILL),)
SPR_EAT(rexrex, A(RRX_ATK1, 100), A(RRX_ATK2, 100),)
SPR_ATTACK(rexrex, A(RRX_ATK1, 100), A(RRX_ATK2, 100),)
SPR_DIE(rexrex, A(RRX_DEAD, 500),)
SOUNDTAB(rexrex, SND_RRX_BRK, 150000, SND_MED_DIE, SND_RRX_HRT, 0, SND_RRX_BRK)

SPR_BIG(vaerix, VAERIX)
SPR_CHILL(vaerix, A(VRX_STILL1, 125), A(VRX_STILL2, 125), A(VRX_STILL1, 125), A(VRX_STILL3, 125), )
SPR_WALK(vaerix, 10, W(VRX_MOV1), W(VRX_MOV2), W(VRX_MOV3), )
SPR_EAT(vaerix, A(VRX_STILL1, 125), A(VRX_STILL2, 125), A(VRX_STILL1, 125), A(VRX_STILL3, 125), )
SPR_ATTACK(vaerix, A(VRX_ATK1, 125), A(VRX_ATK2, 125), A(VRX_ATK3, 125), )
SPR_DIE(vaerix, W(VRX_STILL3), )
SOUNDTAB(vaerix, 0, 0, SND_FAL_DIE, 0, 0, SND_SML_EAT)


SPR_BIG(yazzert, YAZZERT)
SPR_CHILL(yazzert, A(YZT_STILL1, 150), A(YZT_STILL2, 150), A(YZT_STILL1, 150), A(YZT_STILL3, 150), )
SPR_WALK(yazzert, 10, W(YZT_MOV1), W(YZT_MOV2), W(YZT_MOV3), W(YZT_MOV4),)
SPR_EAT(yazzert, A(YZT_STILL1, 125), A(YZT_STILL2, 125), A(YZT_STILL1, 125), A(YZT_STILL3, 125), )
SPR_ATTACK(yazzert, A(YZT_ATK1, 125), A(YZT_ATK2, 125), A(YZT_ATK1, 125), A(YZT_STILL1, 125),)
SPR_DIE(yazzert, W(YZT_STILL3), )
SOUNDTAB(yazzert, 0, 0, SND_FAL_DIE, 0, 0, SND_SML_EAT)



SPR_BIG(frogert, FROGERT)
SPR_CHILL(frogert, A(FRG_STILL1, 1500), A(FRG_STILL2, 250), A(FRG_STILL3, 250), A(FRG_STILL2, 250),)
SPR_WALK(frogert, 10, W(FRG_MOV1), W(FRG_MOV2), W(FRG_MOV3), W(FRG_STILL1), )
SPR_EAT(frogert,  A(FRG_ATK1, 60), A(FRG_ATK2, 60), A(FRG_ATK1, 60), A(FRG_STILL1, 320), )
SPR_ATTACK(frogert,  A(FRG_ATK1, 60), A(FRG_ATK2, 60), A(FRG_ATK1, 60), A(FRG_STILL1, 320), )
SPR_DIE(frogert, A(FRG_DEAD, 500), )
SOUNDTAB(frogert, SND_FRG_BRK, 150000, SND_SML_DIE, 0, 0, SND_SML_EAT)


SPR_BIG(ackatoo, ACKATOO)
SPR_CHILL(ackatoo, W(AKT_STILL),)
SPR_WALK(ackatoo, 10, W(AKT_MOV1), W(AKT_STILL), W(AKT_MOV2), W(AKT_STILL),)
SPR_EAT(ackatoo, A(AKT_ATK1, 150), A(AKT_STILL, 150),
                 A(AKT_ATK1, 150), A(AKT_STILL, 150),
                 A(AKT_ATK1, 150), A(AKT_STILL, 150),
                 A(AKT_ATK1, 150), A(AKT_STILL, 150),
                 A(AKT_ATK1, 150), A(AKT_STILL, 700),)
SPR_ATTACK(ackatoo, A(AKT_ATK1, 300), A(AKT_STILL, 200),)
SPR_DIE(ackatoo, A(AKT_DEAD, 500),)
SOUNDTAB(ackatoo, SND_RAR_BRK, 150000, SND_SML_DIE, SND_RAR_HRT, 0, SND_SML_EAT)


SPR_BIG(quiver, QUIVER)
SPR_CHILL(quiver, A(QUV_STILL1, 500), A(QUV_STILL2, 500),)
SPR_WALK(quiver, 12, W(QUV_STILL1), W(QUV_STILL2),)
SPR_EAT(quiver, A(QUV_STILL1, 500), A(QUV_STILL2, 500),)
SPR_ATTACK(quiver, A(QUV_ATK1, 150), A(QUV_ATK2, 150),)
SPR_DIE(quiver, A(QUV_ATK2, 500),)
NO_SOUND(quiver)


SPR_BIG(crysillian, CRYSILLIAN)
SPR_CHILL(crysillian, W(CRY_STILL),)
SPR_WALK(crysillian, 10, W(CRY_MOV1), W(CRY_MOV2), W(CRY_MOV3), W(CRY_MOV2),)
SPR_EAT(crysillian, ) //Does not eat
SPR_ATTACK(crysillian, A(CRY_ATK1, 100), A(CRY_ATK2, 100), A(CRY_ATK3, 100), A(CRY_ATK4, 100), A(CRY_STILL, 100), )
SPR_DIE(crysillian, A(CRY_DEAD, 500),)
SOUNDTAB(crysillian, 0, 0, SND_CRY_DIE, SND_CRY_HRT, 0, 0)

SPR_BIG(slymer, SLYMER)
SPR_CHILL(slymer, A(SLM_STILL1,125),A(SLM_STILL2,125),A(SLM_STILL3,125),)
SPR_WALK(slymer, 10, W(SLM_MOV1), W(SLM_MOV2), W(SLM_MOV1), W(SLM_STILL1),
                 W(SLM_MOV3), W(SLM_MOV4), W(SLM_MOV3), W(SLM_STILL1),)
SPR_EAT(slymer, A(SLM_ATK1, 125), A(SLM_ATK2, 125), A(SLM_ATK1, 125), A(SLM_STILL1, 125),)
SPR_ATTACK(slymer, A(SLM_ATK1, 125), A(SLM_ATK2, 125), A(SLM_ATK1, 125), A(SLM_STILL1, 125),)
SPR_DIE(slymer, W(SLM_STILL1),)
SOUNDTAB(slymer, SND_SLM_BRK, 150000, SND_SLM_DIE, SND_SLM_DIE, 0, SND_SLM_BRK)


SPR_BIG(stalker, STALKER)
SPR_CHILL(stalker, W(STK_STILL),) // wanders
SPR_WALK(stalker, 10, W(STK_MOV1), W(STK_MOV2), W(STK_MOV1), W(STK_STILL),
                 W(STK_MOV3), W(STK_MOV4), W(STK_MOV3), W(STK_STILL),)
SPR_EAT(stalker, A(STK_EAT1, 125), A(STK_EAT2, 125),)
SPR_ATTACK(stalker, A(STK_ATK1, 125), A(STK_ATK2, 125), A(STK_ATK3, 125), A(STK_ATK4, 125), A(STK_STILL, 125),)
SPR_DIE(stalker, W(STK_DEAD),)
SOUNDTAB(stalker, 0, 0, SND_MED_DIE, 0, 0, SND_SML_EAT)


SPR_BIG(titurtilion, TITURTILION)
SPR_CHILL(titurtilion, A(TIT_STILL1,1300), A(TIT_STILLB,300),
		A(TIT_STILL1,1700), A(TIT_STILL2,300), A(TIT_STILL1,300), A(TIT_STILL3,300),)
SPR_WALK(titurtilion, 10, W(TIT_MOV1), W(TIT_MOV2), W(TIT_MOV1), W(TIT_STILL1),
                W(TIT_MOV3), W(TIT_MOV4), W(TIT_MOV3), W(TIT_STILL1), )
SPR_EAT(titurtilion, W(TIT_ATK1), )
SPR_ATTACK(titurtilion, A(TIT_ATK1, 100), A(TIT_ATK2, 100), A(TIT_ATK3, 100), A(TIT_ATK4, 100), A(TIT_STILL1, 100),)
SPR_DIE(titurtilion, A(TIT_MOV2, 250), W(TIT_DEAD), )
SOUNDTAB(titurtilion, SND_DRA_BRK, 150000, SND_BIG_DIE, SND_REP_HRT, 0, SND_BIG_EAT)


SPR_BIG(golgithan, GOLGITHAN)
SPR_CHILL(golgithan, W(GOL_STILL),)
SPR_WALK(golgithan, 10, W(GOL_MOV1), W(GOL_MOV2), W(GOL_MOV3), W(GOL_MOV2), W(GOL_MOV1), W(GOL_STILL),
                    W(GOL_MOV4), W(GOL_MOV5), W(GOL_MOV6), W(GOL_MOV5), W(GOL_MOV4), W(GOL_STILL),)
SPR_EAT(golgithan, A(GOL_EAT1, 300),  A(GOL_EAT2, 200), A(GOL_EAT3, 300), A(GOL_EAT2, 200), )
SPR_ATTACK(golgithan, A(GOL_EAT1, 200),  A(GOL_EAT2, 200), A(GOL_EAT3, 200), A(GOL_STILL, 200),)
SPR_DIE(golgithan, A(GOL_DEAD1, 200), W(GOL_DEAD2),)
SOUNDTAB(golgithan, SND_GOL_BRK, 150000, SND_BIG_DIE, SND_BIG_HRT, 0, SND_BIG_EAT)

static aspath  dumby_aspath[] = { ENDASL };
NO_SOUND(dumby)

//	int monster_idx, stat_caught, stat_inpark, stat_killed, stat_didscan, stat_won_medals, stat_turrets_target;


#define LIST_SUPPORT  0,0,0,0,0,0,0,0


#define LISTS(who) \
	(monst_base **) &who ## _likes_to_eat, \
	(monst_base **) &who ## _will_eat,     \
	(monst_base **) &who ## _hates,        \
	(monst_base **) &who ## _scared_of,    \
 	&who ## _spr_big, \
	(aspath *) &who ## _spr_chill,    \
        &who ## _walk_span, \
        who ## _snds, \
	(aspath *) &who ## _spr_walk,     \
	(aspath *) &who ## _spr_eat,      \
	(aspath *) &who ## _spr_attack,   \
	(aspath *) &who ## _spr_die,      \
        LIST_SUPPORT

#define BUILDING(who,name,sd,ld,ai,hp,size,flags) \
	name,sd,ld,ai,hp,0,0,0,0,0,0,0,0,size,0,0,10000,0,0,0,flags, NULL, 0,0,0,0,0,0,0, \
	NULL,NULL,NULL,NULL, \
        &who ## _spr_big, (aspath *) &who ## _spr_chill,  NULL, \
        dumby_snds, \
        (aspath *) &dumby_aspath, \
        (aspath *) &dumby_aspath, \
        (aspath *) &dumby_aspath, \
        (aspath *) &who ## _spr_die, LIST_SUPPORT

#include "auto_monstbase.inc"

#undef SPR_CHILL
#undef SPR_WALK
#undef SPR_EAT
#undef SPR_ATTACK
#undef NO_SPRITES


/////////////////////////// Monster Base Types  ///////////////////////////////////

#define M(n) (60000 * n)

#define BUILDING_STD_FLAGS (AF_INORGANIC | AF_NOHOOK | AF_SPARKS |  AF_ASEXUAL )
#define SHIP_STD_FLAGS (AF_FLYING | BUILDING_STD_FLAGS)




/////////////// PLANTS ////////////////

monst_base mb_fern = {
(char *)"Fern",(char *)"plant",
(char *)"Ferns are the most common plant on in the asteroid belt.  "
"They form an important part of a balanced breakfast for many monsters.",
ai_plant,
100 /* max_hitpoints */,
M(11) /* max_age (ms) */,
M(2) /* mature_age (ms) */,
0  /* speed */, 0 /* lat_speed */,  0 /* turn_rate */,
0 /* str */,    0  /* attack_rate (ms/attack) */,
0 /* stomach */,
100 /* size */,
0 /* met_rate (ms/nutritionpt) */,
4000 /* nutrish */,
200 /* energy */,
50 /* od_thresh */,
M(3) /* gestation (ms)*/,
0 /* agression */,
AF_ASEXUAL /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
7 /* herd_size */,
20 /* fame */,
25 /* bfc_price */,
20 /* combat_price */,
95 /*commonality*/,
LISTS(fern)
};

monst_base  mb_shrub = {
(char *)"Shrub",(char *)"plant",
(char *)"Shrubs are something like small trees.  They are a favorite of many herbivores.  "
"Shrubs are most commonly found on the inner track of the asteroid belt.",
ai_plant,
100 /* max_hitpoints */,
M(15) /* max_age (ms) */,
M(5) /* mature_age (ms) */,
0  /* speed */, 0 /* lat_speed */,  0 /* turn_rate */,
0 /* str */,    0  /* attack_rate (ms/attack) */,
0 /* stomach */,
100 /* size */,
0 /* met_rate (ms/nutritionpt) */,
10000 /* nutrish */,
120 /* energy */,
50 /* od_thresh */,
M(5) /* gestation (ms)*/,
0 /* agression */,
AF_ASEXUAL /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
0.0 /* fear_factor */,
8 /* herd_size */,
20 /* fame */,
55 /* bfc price */,
20 /* combat price */,
90 /*commonality*/,
LISTS(shrub)
};


monst_base  mb_stalkplant = {
(char *)"Stalk Plant",(char *)"plant",
(char *)"Stalk plants grow a single eyestalk out of their foliage.  "
"They grow quickly but are only found on a small number of asteroids.  "
"Some animals only eat stalk plants so it is "
"usually a good idea to harvest them when you can.",
ai_plant,
50 /* max_hitpoints */,
M(6) /* max_age (ms) */,
M(1) /* mature_age (ms) */,
0  /* speed */, 0 /* lat_speed */,  0 /* turn_rate */,
0 /* str */,    0  /* attack_rate (ms/attack) */,
0 /* stomach */,
150 /* size */,
0 /* met_rate (ms/nutritionpt) */,
1500 /* nutrish */,
120 /* energy */,
50 /* od_thresh */,
M(1) /* gestation (ms)*/,
0 /* agression */,
AF_ASEXUAL /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
0 /* herd_size */,
20 /* fame */,
25 /* bfc price */,
20 /* combat price */,
80 /*commonality*/,
LISTS(stalkplant)
};


monst_base mb_happytree = {
(char *)"Happy Tree",(char *)"plant",
(char *)"These trees grow on a few isolated asteroids. "
"A Happy Tree always die shortly after the "
"next generation sprouts.  While there are some animals that eat "
"these things, there is probably an equal number who find these things "
"just plain creepy.",
ai_plant,
100 /* max_hitpoints */,
M(40) /* max_age (ms) */,
M(37) /* mature_age (ms) */,
0  /* speed */, 0 /* lat_speed */,  0 /* turn_rate */,
0 /* str */,    500  /* attack_rate (ms/attack) */,
0 /* stomach */,
2900 /* size */,
0 /* met_rate (ms/nutritionpt) */,
60000 /* nutrish */,
120 /* energy */,
10000 /* od_thresh */,
M(1) /* gestation (ms)*/,
0 /* agression */,
AF_ASEXUAL /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
11 /* herd_size */,
400 /* fame */,
100 /* bfc price */,
80 /* combat price */,
50 /*commonality*/,
LISTS(happytree)
};

/////////////// PSUDO PLANTS ////////////////


monst_base mb_mimmack = {
(char *)"Mimmack",(char *)"plant",
(char *)"Mimmacks spend most of their time impersonating "
"Happy Trees to avoid predators.  They are photosynthetic so "
"they don't need to move unless provoked but are very strong so "
"they are usually best left alone.",
ai_animal,
1200 /* max_hitpoints */,
M(400) /* max_age (ms) */,
M(380) /* mature_age (ms) */,
26  /* speed */, 0 /* lat_speed */,  180 /* turn_rate */,
100 /* str */,    600  /* attack_rate (ms/attack) */,
0 /* stomach */,
2900 /* size */,
0 /* met_rate (ms/nutritionpt) */,
60000 /* nutrish */,
8000 /* energy */,
50 /* od_thresh */,
M(36) /* gestation (ms)*/,
7000 /* agression */,
AF_ASEXUAL /* flags */,
&Green /* blood_color*/,
50   /* flight_dist*/,
0.25 /* fear_factor */,
0   /* herd_size */,
300 /* fame */,
100 /* bfc price */,
400 /* combat price */,
5 /*commonality*/,
LISTS(mimmack)
};

monst_base mb_quiver = {
(char *)"Quiver", (char *)"small animal",
(char *)"Quivers are gelatinous pseudoplants that smell good but sting bad.  "
"They attract small insects, then engulf them as a "
"means of sustenance.",
ai_animal,
600 /* max_hitpoints */,
M(50) /* max_age (ms) */,
M(10) /* mature_age (ms) */,
0  /* speed */, 0 /* lat_speed */,  0 /* turn_rate */,
30 /* str */,    500  /* attack_rate (ms/attack) */,
2000 /* stomach */,
100 /* size */,
5 /* met_rate (ms/nutritionpt) */,
1000 /* nutrish */,
100 /* energy */,
500 /* od_thresh */,
M(10) /* gestation (ms)*/,
100 /* agression */,
0x0 /* flags */,
&LightBlue /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
6 /* herd_size */,
50 /* fame */,
20 /* bfc price */,
170 /* combat price */,
15 /*commonality*/,
LISTS(quiver)
};

monst_base mb_poisonshroom = {
(char *)"Poison Shroom", (char *)"fungus",
"Poison shrooms pack a powerful toxin that is fatal to most animals that pass by them.  "
"Their spores are often carried in the digestive tracks of Mubarks and are released shortly after feeding.",
ai_plant,
50 /* max_hitpoints */,
M(5) /* max_age (ms) */,
M(1) /* mature_age (ms) */,
0  /* speed */, 0 /* lat_speed */,  0 /* turn_rate */,
0 /* str */,    0  /* attack_rate (ms/attack) */,
0 /* stomach */,
150 /* size */,
0 /* met_rate (ms/nutritionpt) */,
1500 /* nutrish */,
120 /* energy */,
0 /* od_thresh */,
0 /* gestation (ms)*/,
0 /* agression */,
AF_ASEXUAL  /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
0 /* herd_size */,
0 /* fame */,
0 /* bfc price */,
50 /* combat price */,
5 /*commonality*/,
LISTS(poisonshroom)
};

monst_base mb_psychoshroom = {
(char *)"Psycho Shroom", (char *)"fungus",
(char *)"Psycho shrooms produce the universe's most powerful hallucinogen. They often grow on the remains of genetically  "
"altered or drugged up monsters.  The mere proximity of psycho shrooms can alter a monster's behavior.",
ai_plant,
50 /* max_hitpoints */,
M(5) /* max_age (ms) */,
M(1) /* mature_age (ms) */,
0  /* speed */, 0 /* lat_speed */,  0 /* turn_rate */,
0 /* str */,    0  /* attack_rate (ms/attack) */,
0 /* stomach */,
150 /* size */,
0 /* met_rate (ms/nutritionpt) */,
1500 /* nutrish */,
120 /* energy */,
0 /* od_thresh */,
0 /* gestation (ms)*/,
0 /* agression */,
AF_ASEXUAL /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
0 /* herd_size */,
400 /* fame */,
500 /* bfc price */,
200 /* combat price */,
5 /*commonality*/,
LISTS(psychoshroom)
};

monst_base mb_pyroshroom = {
(char *)"Pyro Shroom", (char *)"fungus",
(char *)"Pyro shrooms contain a number of extremely volatile substances.  To defend itself, "
"it shoots out napalm like spores that react with anything organic it touches.",
ai_plant,
50 /* max_hitpoints */,
M(5) /* max_age (ms) */,
M(1) /* mature_age (ms) */,
0  /* speed */, 0 /* lat_speed */,  0 /* turn_rate */,
0 /* str */,    0  /* attack_rate (ms/attack) */,
0 /* stomach */,
150 /* size */,
0 /* met_rate (ms/nutritionpt) */,
1500 /* nutrish */,
120 /* energy */,
0 /* od_thresh */,
0 /* gestation (ms)*/,
0 /* agression */,
AF_ASEXUAL  /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
0 /* herd_size */,
250 /* fame */,
0 /* bfc price */,
90 /* combat price */,
5 /*commonality*/,
LISTS(pyroshroom)
};



////////// VERY SMALL BUGS /////////////

monst_base mb_roachy = {
(char *)"Roachy",(char *)"Small Animal",
(char *)"This small insectoid is considered a pest by most.  "
"It has an incredible resistance to chemicals "
"and have been known to resort to "
"cannibalism when food is scarce.",
ai_animal,
70 /* max_hitpoints */,
M(10) /* max_age (ms) */,
M(1) /* mature_age (ms) */,
12  /* speed */, 0 /* lat_speed */,  90 /* turn_rate */,
10 /* str */,    500  /* attack_rate (ms/attack) */,
250 /* stomach */,
80 /* size */,
5 /* met_rate (ms/nutritionpt) */,
800 /* nutrish */,
190 /* energy */,
50000 /* od_thresh */,
45000 /* gestation (ms)*/,
100 /* agression */,
AF_PSYCHOSTARVER /* flags */,
&Yellow /* blood_color*/,
200 /* flight_dist*/,
5.0 /* fear_factor */,
6 /* herd_size */,
-100 /* fame */,
17 /* bfc price */,
33 /* combat price */,
25 /*commonality*/,
LISTS(roachy)
};

monst_base mb_krulasite = {
(char *)"Krulasite",(char *)"Small Animal",
(char *)"This small parasite dies shortly after injecting its eggs into "
"an unsuspecting host.  Krulasite larva attach themselves to the host animal's "
"nervous system and take control of its motor functions, making them very violent.  ",
ai_krulasite,
70 /* max_hitpoints */,
M(5) /* max_age (ms) */,
0 /* mature_age (ms) */,
10  /* speed */, 0 /* lat_speed */,  150 /* turn_rate */,
10 /* str */,    500  /* attack_rate (ms/attack) */,
270 /* stomach */,
90 /* size */,
13 /* met_rate (ms/nutritionpt) */,
800 /* nutrish */,
190 /* energy */,
50000 /* od_thresh */,
0 /* gestation N/A for krulasite */,
100 /* agression */,
AF_ASEXUAL /* flags */,
&Purple /* blood_color*/,
600 /* flight_dist*/,
0.0  /* fear_factor */,
0 /* herd_size */,
-100 /* fame */,
17 /* bfc price */,
0 /* combat price */,
0 /*commonality*/,
LISTS(krulasite)
};

monst_base mb_quitofly = {
(char *)"Quitofly",(char *)"small animal",
(char *)"Quitoflys are flying, blood-sucking insects who literally "
"eat their prey while it is still alive.  They commonly inhabit "
"swampy asteroids.",
ai_animal,
10 /* max_hitpoints */,
M(15) /* max_age (ms) */,
M(2) /* mature_age (ms) */,
70  /* speed */, 30 /* lat_speed */,  300 /* turn_rate */,
10 /* str */,    500  /* attack_rate (ms/attack) */,
1000 /* stomach */,
100 /* size */,
10 /* met_rate (ms/nutritionpt) */,
1000 /* nutrish */,
500 /* energy */,
500 /* od_thresh */,
M(3) /* gestation (ms)*/,
100 /* agression */,
AF_FLYING | AF_BLOODSUCKER /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
1.0  /* fear_factor */,
6 /* herd_size */,
-100 /* fame */,
20 /* bfc price */,
13 /* combat price */,
25 /*commonality*/,
LISTS(quitofly)
};

monst_base mb_pygment = {
(char *)"Pygment",(char *)"Small Animal",
(char *)"Pygments are small three legged crab-like animals.  "
"They are nearly mindless and prefer sitting in one place, "
"eating stalk plants",
ai_animal,
140 /* max_hitpoints */,
M(600) /* max_age (ms) */,
M(15) /* mature_age (ms) */,
8  /* speed */, 12 /* lat_speed */,  30 /* turn_rate */,
12 /* str */,    500  /* attack_rate (ms/attack) */,
2000 /* stomach */,
400 /* size */,
5 /* met_rate (ms/nutritionpt) */,
1800 /* nutrish */,
130 /* energy */,
800 /* od_thresh */,
M(5) /* gestation (ms)*/,
50 /* agression */,
0x0 /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.05  /* fear_factor */,
26 /* herd_size */,
75 /* fame */,
38 /* bfc price */,
52 /* combat price */,
35 /*commonality*/,
LISTS(pygment)
};



///////// SMALL ANIMALS ///////////


monst_base mb_frogert = {
(char *)"Frogert",(char *)"small animal",
(char *)"Frogerts help you park by keeping it bug free.",
ai_animal,
100 /* max_hitpoints */,
M(20) /* max_age (ms) */,
M(3) /* mature_age (ms) */,
35  /* speed */, 0 /* lat_speed */,  100 /* turn_rate */,
20 /* str */,    500  /* attack_rate (ms/attack) */,
1000 /* stomach */,
340 /* size */,
5 /* met_rate (ms/nutritionpt) */,
2000 /* nutrish */,
200 /* energy */,
500 /* od_thresh */,
M(5) /* gestation (ms)*/,
100 /* agression */,
0x0 /* flags */,
&LightGreen /* blood_color*/,
200 /* flight_dist*/,
0.80  /* fear_factor */,
16 /* herd_size */,
150 /* fame */,
38 /* bfc price */,
57 /* combat price */,
12 /*commonality*/,
LISTS(frogert)
};

monst_base mb_furbit = {
(char *)"Furbit", (char *)"small mammal",
(char *)"A furbit is something like a furry armadillo. "
"They live in holes and are actually quite tasty.",
ai_animal,
50 /* max_hitpoints */,
M(30) /* max_age (ms) */,
M(3) /* mature_age (ms) */,
12  /* speed */, 0 /* lat_speed */,  110 /* turn_rate */,
10 /* str */,    500  /* attack_rate (ms/attack) */,
2500 /* stomach */,
390 /* size */,
30 /* met_rate (ms/nutritionpt) */,
3000 /* nutrish */,
160 /* energy */,
50 /* od_thresh */,
M(5) /* gestation (ms)*/,
25 /* agression */,
AF_DIGS /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
1.6  /* fear_factor */,
4 /* herd_size */,
100 /* fame */,
50 /* bfc price */,
28 /* combat price */,
40 /*commonality*/,
LISTS(furbit)
};


monst_base mb_shruver = {
(char *)"Shruver", (char *)"Small Animal",
(char *)"These bite sized critters make a rewarding treat for almost any predator.  "
"A large herd of shruvers can sustain many hungry animals.",
ai_animal,
30 /* max_hitpoints */,
M(25) /* max_age (ms) */,
M(3) /* mature_age (ms) */,
18  /* speed */, 0 /* lat_speed */,  300 /* turn_rate */,
10 /* str */,    500  /* attack_rate (ms/attack) */,
1000 /* stomach */,
290 /* size */,
10 /* met_rate (ms/nutritionpt) */,
2000 /* nutrish */,
1000 /* energy */,
500 /* od_thresh */,
M(4) /* gestation (ms)*/,
50 /* agression */,
0x0 /* flags */,
&LightGreen /* blood_color*/,
200 /* flight_dist*/,
5.0  /* fear_factor */,
10 /* herd_size */,
100 /* fame */,
37 /* bfc price */,
22 /* combat price */,
50 /*commonality*/,
LISTS(shruver)
};

monst_base mb_radrunner = {
(char *)"Radrunner", (char *)"Small Animal",
(char *)"These quick little guys are pretty hard to "
"catch but they're oh so pretty.",
ai_animal,
30 /* max_hitpoints */,
M(100) /* max_age (ms) */,
M(10) /* mature_age (ms) */,
60  /* speed */, 0 /* lat_speed */,  300 /* turn_rate */,
10 /* str */,    500  /* attack_rate (ms/attack) */,
1000 /* stomach */,
290 /* size */,
15 /* met_rate (ms/nutritionpt) */,
2000 /* nutrish */,
1000 /* energy */,
500 /* od_thresh */,
M(5) /* gestation (ms)*/,
50 /* agression */,
AF_FAMILY /* flags */,
&LightGreen /* blood_color*/,
200 /* flight_dist*/,
5.0  /* fear_factor */,
2 /* herd_size */,
380 /* fame */,
37 /* bfc price */,
24 /* combat price */,
15 /*commonality*/,
LISTS(radrunner)
};


///////////// MEDIUM HERBAVORS /////////////

monst_base mb_mubark = {
(char *)"Mubark",(char *)"Medium Animal",
"A mubark looks like a bizarre cross between a cow and a dog.  "
"It is a rather peaceful vegetarian which enjoys grazing and running around.",
ai_animal,
200 /* max_hitpoints */,
M(100) /* max_age (ms) */,
M(10) /* mature_age (ms) */,
30  /* speed */, 0 /* lat_speed */,  130 /* turn_rate */,
10 /* str */,    500  /* attack_rate (ms/attack) */,
5000 /* stomach */,
400 /* size */,
10 /* met_rate (ms/nutritionpt) */,
8000 /* nutrish */,
400 /* energy */,
500 /* od_thresh */,
M(7) /* gestation (ms)*/,
100 /* agression */,
AF_HERDS | AF_PSYCHOSTARVER /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
1.60  /* fear_factor */,
9 /* herd_size */,
175 /* fame */,
100 /* bfc price */,
57 /* combat price */,
30 /*commonality*/,
LISTS(mubark)
};

monst_base mb_molber = {
(char *)"Molber",(char *)"Medium Animal",
(char *)"Molbers are six legged buffalo.  They are lots of fun to hunt: "
"easy to kill, a good source of food, "
"filling, but not good for much else.",
ai_animal,
500 /* max_hitpoints */,
M(100) /* max_age (ms) */,
M(10) /* mature_age (ms) */,
20  /* speed */, 0 /* lat_speed */,  100 /* turn_rate */,
12 /* str */,    500  /* attack_rate (ms/attack) */,
8000 /* stomach */,
1200 /* size */,
50 /* met_rate (ms/nutritionpt) */,
14000 /* nutrish */,
250 /* energy */,
500 /* od_thresh */,
M(5) /* gestation (ms)*/,
800 /* agression */,
0x0 /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.80  /* fear_factor */,
16 /* herd_size */,
350 /* fame */,
175 /* bfc price */,
98 /* combat price */,
30 /*commonality*/,
LISTS(molber)
};

monst_base mb_ackatoo = {
(char *)"Ackatoo",(char *)"medium animal",
(char *)"Ackatoos are acid-spitting, flightless birds.  "
"Their ability to \"spit a lugie\" two hundred feet makes them one of the most "
"dangerous herbivores in the belt.",
ai_ackatoo,
150 /* max_hitpoints */,
M(40) /* max_age (ms) */,
M(5) /* mature_age (ms) */,
18  /* speed */, 0 /* lat_speed */,  140 /* turn_rate */,
50 /* str */,    500  /* attack_rate (ms/attack) */,
4000 /* stomach */,
500 /* size */,
15 /* met_rate (ms/nutritionpt) */,
4000 /* nutrish */,
500 /* energy */,
1500 /* od_thresh */,
M(5) /* gestation (ms)*/,
800 /* agression */,
AF_FAMILY /* flags */,
&LightGreen /* blood_color*/,
200 /* flight_dist*/,
0.45  /* fear_factor */,
2 /* herd_size */,
250 /* fame */,
62 /* bfc price */,
135 /* combat price */,
8 /*commonality*/,
LISTS(ackatoo)
};

monst_base mb_squeeker  = {
(char *)"Squeeker", (char *)"medium animal",
(char *)"A squeeker is a fun, multicolored, spotted, noisy animal.  "
"Basicly everyone likes these things for some reason.",
ai_animal,
80 /* max_hitpoints */,
M(150) /* max_age (ms) */,
M(20) /* mature_age (ms) */,
9  /* speed */, 0 /* lat_speed */,  130 /* turn_rate */,
10 /* str */,    500  /* attack_rate (ms/attack) */,
2000 /* stomach */,
1500 /* size */,
1 /* met_rate (ms/nutritionpt) */,
10000 /* nutrish */,
200 /* energy */,
500 /* od_thresh */,
M(55) /* gestation (ms)*/,
100 /* agression */,
0x0 /* flags */,
&LightGreen /* blood_color*/,
200 /* flight_dist*/,
0.80  /* fear_factor */,
20 /* herd_size */,
1000 /* fame */,
135 /* bfc price */,
80 /* combat price */,
12 /*commonality*/,
LISTS(squeeker)
};

monst_base mb_meeker  = {
(char *)"Meeker", (char *)"medium animal",
(char *)"They look like a lava lamp's bad dream came true. "
"Meekers are rather rare, consider yourself lucky to have found this one, but "
"don't let their cuteness fool you, they possess incredible hidden powers.",
ai_meeker,
180 /* max_hitpoints */,
M(1000) /* max_age (ms) */,
M(100) /* mature_age (ms) */,
12  /* speed */, 0 /* lat_speed */,  500 /* turn_rate */,
650 /* str */,    500  /* attack_rate (ms/attack) */,
6000 /* stomach */,
650 /* size */,
5 /* met_rate (ms/nutritionpt) */,
15000 /* nutrish */,
2200 /* energy */,
500 /* od_thresh */,
M(120) /* gestation (ms)*/,
100 /* agression */,
0x0 /* flags */,
&Blue /* blood_color*/,
200 /* flight_dist*/,
0.75  /* fear_factor */,
0 /* herd_size */,
1750 /* fame */,
170 /* bfc price */,
500 /* combat price */,
2 /*commonality*/,
LISTS(meeker)
};

monst_base mb_sneeker = {
(char *)"Sneeker",(char *)"Medium Animal",
(char *)"Sneekers are rather indescript animals that can make themselves invisible at will.",
ai_animal,
80 /* max_hitpoints */,
M(200) /* max_age (ms) */,
M(40) /* mature_age (ms) */,
15  /* speed */, 0 /* lat_speed */,  130 /* turn_rate */,
10 /* str */,    500  /* attack_rate (ms/attack) */,
4000 /* stomach */,
600 /* size */,
20 /* met_rate (ms/nutritionpt) */,
10000 /* nutrish */,
100 /* energy */,
500 /* od_thresh */,
M(25) /* gestation (ms)*/,
100 /* agression */,
AF_CLOAKING /* flags */,
&LightBlue /* blood_color*/,
200 /* flight_dist*/,
1.40  /* fear_factor */,
2 /* herd_size */,
1250 /* fame */,
125 /* bfc price */,
46 /* combat price */,
5 /*commonality*/,
LISTS(sneeker)
};


monst_base mb_vaerix = {
(char *)"Vaerix",(char *)"small animal",
(char *)"A vaerix is a small floating eye, which makes a decent sentries against raiders and pests.",
ai_beholder,
200 /* max_hitpoints */,
M(100) /* max_age (ms) */,
M(40) /* mature_age (ms) */,
25  /* speed */, 25 /* lat_speed */,  580 /* turn_rate */,
100 /* str */,    1000  /* attack_rate (ms/attack) */,
2000 /* stomach */,
450 /* size */,
5 /* met_rate (ms/nutritionpt) */,
3000 /* nutrish */,
300 /* energy */,
1500 /* od_thresh */,
M(12) /* gestation (ms)*/,
600 /* agression */,
AF_FLYING /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
1.85  /* fear_factor */,
16 /* herd_size */,
250 /* fame */,
51 /* bfc price */,
220 /* combat price */,
15 /*commonality*/,
LISTS(vaerix)
};

monst_base mb_yazzert = {
(char *)"Yazzert",(char *)"medium animal",
(char *)"A yazzert is a rather unnerving looking floating mass of eyes.  "
"They make excellent sentries against raiders, pests, and dangerous monsters "
"because they can fry an enemy from far away.",
ai_beholder,
800 /* max_hitpoints */,
M(150) /* max_age (ms) */,
M(0) /* mature_age (ms) */,
15  /* speed */, 15 /* lat_speed */,  280 /* turn_rate */,
200 /* str */,    1000  /* attack_rate (ms/attack) */,
3000 /* stomach */,
900 /* size */,
8 /* met_rate (ms/nutritionpt) */,
5000 /* nutrish */,
300 /* energy */,
2500 /* od_thresh */,
M(0) /* gestation (ms)*/,
1000 /* agression */,
AF_FLYING /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.80  /* fear_factor */,
3 /* herd_size */,
400 /* fame */,
80 /* bfc price */,
642 /* combat price */,
8 /*commonality*/,
LISTS(yazzert)
};

monst_base mb_malcumbit = {
(char *)"Malcumbit",(char *)"small animal",
(char *)"Malcumbit literally translated means \"bad evil thing\".  These violent, pre-historic furbits "
"died out long ago; however, modern science has resurrected them from dormant DNA found in its descendants.",
ai_animal,
1000 /* max_hitpoints */,
M(45) /* max_age (ms) */,
M(7) /* mature_age (ms) */,
30  /* speed */, 20 /* lat_speed */,  500 /* turn_rate */,
120 /* str */,    500  /* attack_rate (ms/attack) */,
6000 /* stomach */,
550 /* size */,
50 /* met_rate (ms/nutritionpt) */,
8000 /* nutrish */,
1800 /* energy */,
7000 /* od_thresh */,
M(10) /* gestation (ms)*/,
7500 /* agression */,
AF_PSYCHOSTARVER | AF_SEECLOAKING /* flags */,
&Blue /* blood_color*/,
200 /* flight_dist*/,
0.3 /* fear_factor */,
0 /* herd_size */,
500 /* fame */,
100 /* bfc price */,
400 /* combat price */,
4 /*commonality*/,
LISTS(malcumbit)
};

////////////// MEDIUM OMNIVORS //////////////
////////////// MEDIUM OMNIVORS //////////////

monst_base mb_squarger  = {
(char *)"Squarger",(char *)"Medium Animal",
(char *)"A squarger is kinda like a pissed off over-bloated sponge.",
ai_animal,
800 /* max_hitpoints */,
M(500) /* max_age (ms) */,
M(50) /* mature_age (ms) */,
5  /* speed */, 0 /* lat_speed */,  100 /* turn_rate */,
200 /* str */,    1500  /* attack_rate (ms/attack) */,
20000 /* stomach */,
625 /* size */,
10 /* met_rate (ms/nutritionpt) */,
10000 /* nutrish */,
1200 /* energy */,
400 /* od_thresh */,
M(45) /* gestation (ms)*/,
2000 /* agression */,
0 /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.75  /* fear_factor */,
6 /* herd_size */,
100 /* fame */,
139 /* bfc price */,
290 /* combat price */,
15 /*commonality*/,
LISTS(squarger)
};

monst_base mb_slymer = {
(char *)"Slymer",(char *)"medium monster",
(char *)"Ooooh... gooie.",
ai_slymer,
3000 /* max_hitpoints */,
M(30) /* max_age (ms) */,
M(25) /* mature_age (ms) */,
4  /* speed */, 4 /* lat_speed */,  100 /* turn_rate */,
14 /* str */,    120  /* attack_rate (ms/attack) */,
5000 /* stomach */,
800 /* size */,
4 /* met_rate (ms/nutritionpt) */,
4000 /* nutrish */,
100 /* energy */,
1500 /* od_thresh */,
M(1) /* gestation (ms)*/,
500 /* agression */,
0x0 /* flags */,
&LightGreen /* blood_color*/,
200 /* flight_dist*/,
0.05 /* fear_factor */,
6 /* herd_size */,
50 /* fame */,
68 /* bfc price */,
580 /* combat price */,
11 /*commonality*/,
LISTS(slymer)
};

///////////// MEDIUM CARNIVORS /////////////

monst_base mb_bervul = {
(char *)"Bervul",(char *)"small animal",
(char *)"A bervul is like a cute lil' puppydog with a lot of pep.",
ai_animal,
180 /* max_hitpoints */,
M(30) /* max_age (ms) */,
M(3) /* mature_age (ms) */,
35 /* speed */, 30 /* lat_speed */,  300 /* turn_rate */,
25 /* str */,    500  /* attack_rate (ms/attack) */,
3000 /* stomach */,
425 /* size */,
15 /* met_rate (ms/nutritionpt) */,
2000 /* nutrish */,
10000 /* energy */,
500 /* od_thresh */,
M(5) /* gestation (ms)*/,
1000 /* agression */,
0x0 /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.80  /* fear_factor */,
5 /* herd_size */,
100 /* fame */,
40 /* bfc price */,
84 /* combat price */,
20 /*commonality*/,
LISTS(bervul)
};

monst_base mb_rexrex = {
(char *)"Rexrex",(char *)"medium animal",
(char *)"The rexrex is a horse sized bulldog, commonly bred for pit fighting.",
ai_animal,
600 /* max_hitpoints */,
M(80) /* max_age (ms) */,
M(0) /* mature_age (ms) */,
30  /* speed */, 20 /* lat_speed */,  500 /* turn_rate */,
60 /* str */,    500  /* attack_rate (ms/attack) */,
8000 /* stomach */,
800 /* size */,
50 /* met_rate (ms/nutritionpt) */,
6000 /* nutrish */,
10000 /* energy */,
1800 /* od_thresh */,
M(0) /* gestation (ms)*/,
4000 /* agression */,
AF_WANDERS /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.30  /* fear_factor */,
4 /* herd_size */,
200 /* fame */,
88 /* bfc price */,
240 /* combat price */,
15 /*commonality*/,
LISTS(rexrex)
};

monst_base mb_throbid = {
(char *)"Throbid",(char *)"Medium Animal",
(char *)"Throbids are very territorial, carnivorous animals "
"with the personality of a rottweiler and the body of an impish velocoraptor.",
ai_animal,
300 /* max_hitpoints */,
M(100) /* max_age (ms) */,
M(10) /* mature_age (ms) */,
40  /* speed */, 12 /* lat_speed */,  250 /* turn_rate */,
25 /* str */,    500  /* attack_rate (ms/attack) */,
20000 /* stomach */,
700 /* size */,
20 /* met_rate (ms/nutritionpt) */,
4000 /* nutrish */,
700 /* energy */,
900 /* od_thresh */,
M(20) /* gestation (ms)*/,
9000 /* agression */,
AF_PSYCHOSTARVER /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.18  /* fear_factor */,
7 /* herd_size */,
250 /* fame */,
66 /* bfc price */,
110 /* combat price */,
10 /*commonality*/,
LISTS(throbid)
};


monst_base mb_toothworm = {
(char *)"Toothworm",(char *)"medium animal",
(char *)"According to the Big Book of Monsters (3rd Edition) toothworms are: \"Mean ass flying worms with teeth.\"",
ai_animal,
150 /* max_hitpoints */,
M(35) /* max_age (ms) */,
M(6) /* mature_age (ms) */,
30  /* speed */, 0 /* lat_speed */,  300 /* turn_rate */,
65 /* str */,    800  /* attack_rate (ms/attack) */,
5000 /* stomach */,
1200 /* size */,
35 /* met_rate (ms/nutritionpt) */,
1500 /* nutrish */,
800 /* energy */,
500 /* od_thresh */,
M(5) /* gestation (ms)*/,
3000 /* agression */,
AF_FLYING | AF_DIGS /* flags */,
&Green /* blood_color*/,
200 /* flight_dist*/,
0.75  /* fear_factor */,
6 /* herd_size */,
100 /* fame */,
50 /* bfc price */,
98 /* combat price */,
12 /*commonality*/,
LISTS(toothworm)
};


////////// Humanoids  ////////////

monst_base mb_orick = {
(char *)"Orick",(char *)"Humanoid",
(char *)"Oricks are a race of highly intelligent warriors."
"Their own technology is still rather primitive; however, they are able to quickly learn to use almost any"
"weapon they find.",
ai_soldier,
580 /* max_hitpoints */,
M(70) /* max_age (ms) */,
15 /* mature_age (ms) */,
10  /* speed */,  8 /* lat_speed */,  220 /* turn_rate */,
25 /* str */,    500  /* attack_rate (ms/attack) */,
2800 /* stomach */,
1100 /* size */,
10 /* met_rate (ms/nutritionpt) */,
500 /* nutrish */,
500 /* energy */,
500 /* od_thresh */,
M(15) /* gestation (ms)*/,
1000 /* agression */,
AF_FAMILY /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.20  /* fear_factor */,
12 /* herd_size */,
200 /* fame */,
38 /* bfc price */,
210 /* combat price */,
5 /*commonality*/,
LISTS(orick)
};

monst_base mb_cheech = {
(char *)"Cheech",(char *)"Humanoid ",
(char *)"Cheeches are natural born chemists.  Unfortunately their love for chemicals usually "
"keeps them sitting around completely stoned looking at the colors.",
ai_cheech,
100 /* max_hitpoints */,
M(50) /* max_age (ms) */,
12 /* mature_age (ms) */,
11  /* speed */,  7 /* lat_speed */,  200 /* turn_rate */,
5 /* str */,    500  /* attack_rate (ms/attack) */,
2000 /* stomach */,
760 /* size */,
5 /* met_rate (ms/nutritionpt) */,
500 /* nutrish */,
500 /* energy */,
5000 /* od_thresh */,
M(15) /* gestation (ms)*/,
800 /* agression */,
0 /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
1.2  /* fear_factor */,
0 /* herd_size */,
0 /* fame */,
32 /* bfc price */,
350 /* combat price */,
5 /*commonality*/,
LISTS(cheech)
};

monst_base mb_hoveler = {
(char *)"Hoveler",(char *)"Humanoid",
(char *)"Hovelers are a subterranean subspecies of Oricks."
"Like Oricks they are intelligent enough to produce and use various low-tech weapons; however, their pacifist"
"natures make them prefer tranquilizers to the more lethal weapons used by their more primal brethren.",
ai_hoveler,
900 /* max_hitpoints */,
M(120) /* max_age (ms) */,
18 /* mature_age (ms) */,
8  /* speed */,  7 /* lat_speed */,  180 /* turn_rate */,
8 /* str */,    500  /* attack_rate (ms/attack) */,
2000 /* stomach */,
500 /* size */,
5 /* met_rate (ms/nutritionpt) */,
500 /* nutrish */,
500 /* energy */,
500 /* od_thresh */,
M(15) /* gestation (ms)*/,
1000 /* agression */,
AF_DIGS | AF_FAMILY /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.8  /* fear_factor */,
0 /* herd_size */,
200 /* fame */,
27 /* bfc price */,
350 /* combat price */,
5 /*commonality*/,
LISTS(hoveler)
};

////////////////// SIMI-Humanoid ///////////////////

monst_base mb_stalker = {
(char *)"Stalker",(char *)"Simi-Humanoid",
(char *)"Stalkers are extremely cunning, invisible hunters.  "
"A stalker is easy to kill once you see it, "
"but it's large scythe arms make it a truly "
"effective ambush predator.  There are some rumors that "
"stalkers may have a complex society but it hard to confirm since "
"they are rarely found dead, much less alive.",
ai_stalker,
200 /* max_hitpoints */,
M(60) /* max_age (ms) */,
M(20) /* mature_age (ms) */,
12  /* speed */, 5 /* lat_speed */,  250 /* turn_rate */,
300 /* str */,    500  /* attack_rate (ms/attack) */,
7000 /* stomach */,
800 /* size */,
15 /* met_rate (ms/nutritionpt) */,
4000 /* nutrish */,
800 /* energy */,
4500 /* od_thresh */,
M(10) /* gestation (ms)*/,
2000 /* agression */,
AF_CLOAKING | AF_SEECLOAKING | AF_WANDERS | AF_FAMILY /* flags */,
&Black /* blood_color*/,
200 /* flight_dist*/,
1.0  /* fear_factor */,
0 /* herd_size */,
700 /* fame */,
68 /* bfc price */,
380 /* combat price */,
3 /*commonality*/,
LISTS(stalker)
};


monst_base mb_crysillian = {
(char *)"Crysillian",(char *)"Simi-Humanoid",
(char *)"Crysillians are believed to be the guardian robots of some unknown alien race.  "
"They have infinite power supplies, and are easily persuaded to defend your own facilities.",
ai_crysillian, /*crysillians will only use bomb against golgathans*/
1000 /* max_hitpoints */,
M(99999) /* max_age (ms) */,
M(0) /* mature_age (ms) */,
12  /* speed */, 5 /* lat_speed */,  200 /* turn_rate */,
200 /* str */,    500  /* attack_rate (ms/attack) */,
0 /* stomach */,
800 /* size */,
0 /* met_rate (ms/nutritionpt) */,
0 /* nutrish */,
99999 /* energy */,
0 /* od_thresh */,
M(0) /* gestation (ms)*/,
0 /* agression */,
AF_ASEXUAL | AF_SEECLOAKING | AF_INORGANIC /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
0.0 /* fear_factor */,
0 /* herd_size */,
400 /* fame */,
0 /* bfc price */,
566 /* combat price */,
4 /*commonality*/,
LISTS(crysillian)
};




//////////////// LARGE ANIMALS //////////////


monst_base mb_drexler = {
(char *)"Drexler",(char *)"large animal",
(char *)"Not the mindless herbivorous  pushover pygments they used to be, "
"drexlers are powerful and cunning predators and are never afraid to take on a fight.",
ai_animal,
900 /* max_hitpoints */,
M(700) /* max_age (ms) */,
M(0) /* mature_age (ms) */,
18  /* speed */, 0 /* lat_speed */,  100 /* turn_rate */,
90 /* str */,    500  /* attack_rate (ms/attack) */,
12000 /* stomach */,
3600 /* size */,
60 /* met_rate (ms/nutritionpt) */,
6400 /* nutrish */,
300 /* energy */,
1600 /* od_thresh */,
0 /* gestation (ms)*/,
3000 /* agression */,
0x0 /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.05  /* fear_factor */,
3 /* herd_size */,
300 /* fame */,
124 /* bfc price */,
360 /* combat price */,
9 /*commonality*/,
LISTS(drexler)
};

monst_base mb_krakin = {
(char *)"Krakin",(char *)"large animal",
(char *)"Krakin are rare giant salamanders that only live on large, fertile asteroids.  "
"They spend most of their time living in pockets of water deep inside their lairs, "
"only emerging to feed.",
ai_animal,
800 /* max_hitpoints */,
M(150) /* max_age (ms) */,
M(50) /* mature_age (ms) */,
20  /* speed */, 0 /* lat_speed */,  400 /* turn_rate */,
50 /* str */,    400  /* attack_rate (ms/attack) */,
35000 /* stomach */,
4000 /* size */,
70 /* met_rate (ms/nutritionpt) */,
9000 /* nutrish */,
1000 /* energy */,
1500 /* od_thresh */,
M(25) /* gestation (ms)*/,
5000 /* agression */,
AF_DIGS | AF_WANDERS /* flags */,
&LightGreen /* blood_color*/,
200 /* flight_dist*/,
0.20  /* fear_factor */,
0 /* herd_size */,
500 /* fame */,
153 /* bfc price */,
281 /* combat price */,
5 /*commonality*/,
LISTS(krakin)
};

monst_base mb_dragoonigon = {
(char *)"Dragoonigon",(char *)"large animal",
(char *)"Dragoonigons are large, flying, fire breathing serpents with bat like wings.",
ai_dragoonigon,
3500 /* max_hitpoints */,
M(200) /* max_age (ms) */,
M(30) /* mature_age (ms) */,
36  /* speed */, 20 /* lat_speed */,  230 /* turn_rate */,
90 /* str */, 500  /* attack_rate (ms/attack) */,
32000 /* stomach */,
14000 /* size */,
120 /* met_rate (ms/nutritionpt) */,
15000 /* nutrish */,
2000 /* energy */,
1500 /* od_thresh */,
M(45) /* gestation (ms)*/,
6000 /* agression */,
AF_FLYING /* flags */,
&LightGreen /* blood_color*/,
200 /* flight_dist*/,
0.20  /* fear_factor */,
6 /* herd_size */,
750 /* fame */,
268 /* bfc price */,
710 /* combat price */,
2 /*commonality*/,
LISTS(dragoonigon)
};

monst_base mb_mamothquito = {
(char *)"Mamothquito",(char *)"large animal",
(char *)"This huge cousin to the tiny quitofly actually prefers to kill its prey "
"before eating it.  The wingspan on these is enormous.  Bringing one of "
"these back alive is a challenge to say the least.",
ai_animal,
800 /* max_hitpoints */,
M(900) /* max_age (ms) */,
0 /* mature_age (ms) */,
70  /* speed */, 30 /* lat_speed */,  300 /* turn_rate */,
80 /* str */,    300  /* attack_rate (ms/attack) */,
10000 /* stomach */,
8000 /* size */,
50 /* met_rate (ms/nutritionpt) */,
15000 /* nutrish */,
50000 /* energy */,
5000 /* od_thresh */,
0 /* gestation (ms)*/,
3000 /* agression */,
AF_FLYING /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.75  /* fear_factor */,
2 /* herd_size */,
300 /* fame */,
240 /* bfc price */,
412 /* combat price */,
1 /*commonality*/,
LISTS(mamothquito)
};


monst_base mb_uberworm = {
(char *)"Uberworm",(char *)"large animal",
(char *)"Beware the uberworm!",
ai_animal,
3100 /* max_hitpoints */,
M(300) /* max_age (ms) */,
M(0) /* mature_age (ms) */,
18  /* speed */, 0 /* lat_speed */,  300 /* turn_rate */,
210 /* str */,    900  /* attack_rate (ms/attack) */,
20000 /* stomach */,
18000 /* size */,
130 /* met_rate (ms/nutritionpt) */,
30000 /* nutrish */,
1000 /* energy */,
5000 /* od_thresh */,
M(0) /* gestation (ms)*/,
6500 /* agression */,
AF_FLYING /* flags */,
&GhostGreen /* blood_color*/,
200 /* flight_dist*/,
0.50  /* fear_factor */,
0 /* herd_size */,
850 /* fame */,
434 /* bfc price */,
758 /* combat price */,
5 /*commonality*/,
LISTS(uberworm)
};


monst_base mb_bigasaurus = {
(char *)"Bigasaurus",(char *)"large animal",
(char *)"Like a giant dinosaur with a taste for just about anything "
"it can sink it's teeth into.  The bigasaurus is feared by the many folks "
"who have lost loved ones to its merciless jaws.",
ai_animal,
4400 /* max_hitpoints */,
M(400) /* max_age (ms) */,
M(0) /* mature_age (ms) */,
25  /* speed */, 0 /* lat_speed */,  280 /* turn_rate */,
180 /* str */,    800  /* attack_rate (ms/attack) */,
50000 /* stomach */,
21000 /* size */,
100 /* met_rate (ms/nutritionpt) */,
60000 /* nutrish */,
10000 /* energy */,
500 /* od_thresh */,
M(0) /* gestation (ms)*/,
7000 /* agression */,
AF_PSYCHOSTARVER /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.75  /* fear_factor */,
6 /* herd_size */,
700 /* fame */,
745 /* bfc price */,
888 /* combat price */,
3 /*commonality*/,
LISTS(bigasaurus)
};



monst_base mb_titurtilion = {
(char *)"Titurtilion",(char *)"large animal",
(char *)"Titurtilions are well armored herbivores.  "
"Their massive size and thick shells give them "
"little reason to fear anything.",
ai_animal,
8000 /* max_hitpoints */,
M(250) /* max_age (ms) */,
M(50) /* mature_age (ms) */,
15  /* speed */, 0 /* lat_speed */,  100 /* turn_rate */,
70 /* str */,    500  /* attack_rate (ms/attack) */,
50000 /* stomach */,
18000 /* size */,
150 /* met_rate (ms/nutritionpt) */,
70000 /* nutrish */,
500 /* energy */,
6000 /* od_thresh */,
M(30) /* gestation (ms)*/,
5000 /* agression */,
0x0 /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.05  /* fear_factor */,
6 /* herd_size */,
800 /* fame */,
834 /* bfc price */,
947 /* combat price */,
5 /*commonality*/,
LISTS(titurtilion)
};

monst_base mb_golgithan = {
(char *)"Golgithan",(char *)"large animal",
(char *)"It's really, REALLY, big, with a strength and attitude to match.  "
"When a golgithan comes by, the only safe place to hide is in a grove of happy trees.",
ai_animal,
75000 /* max_hitpoints */,
M(5000) /* max_age (ms) */,
M(100) /* mature_age (ms) */,
20  /* speed */, 25 /* lat_speed */,  180 /* turn_rate */,
450 /* str */,    500  /* attack_rate (ms/attack) */,
100000 /* stomach */,
42500 /* size */,
250 /* met_rate (ms/nutritionpt) */,
100000 /* nutrish */,
10000 /* energy */,
50000 /* od_thresh */,
M(500) /* gestation (ms)*/,
9000 /* agression */,
0x0 /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
0 /* herd_size */,
10000 /* fame */,
4000 /* bfc price */,
7300 /* combat price */,
0 /* commonality*/,
LISTS(golgithan)
};

/////////////////////////// HUMAN OWNED STUFF ////////////////////////////

////////////// AST 0 EQUIP //////////////

monst_base mb_netship = {
(char *)"Net Ship",(char *)"Vehicle",
(char *)"Your ship is cool.",
ai_player,
5000 /* max_hitpoints */,
0 /* max_age (ms) */,
0 /* mature_age (ms) */,
0  /* speed */, 0 /* lat_speed */, 0 /* turn_rate */,
0 /* str */, 0  /* attack_rate (ms/attack) */,
0 /* stomach */,
1200 /* size */,
0 /* met_rate (ms/nutritionpt) */,
0 /* nutrish */,
6000 /* energy */,
0 /* od_thresh */,
0 /* gestation (ms)*/,
0 /* agression */,
SHIP_STD_FLAGS /* flags */,
NULL /* blood_color*/,
0 /* flight_dist*/,
0.0  /* fear_factor */,
0 /* herd_size */,
0 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(netship)
};

// ejk : upped hp to 50k.
monst_base mb_astbase = {
BUILDING(astbase, (char *)"Asteroid Base",(char *)"Building",(char *)"Home, Sweet Home",ai_building,  50000, 10000, BUILDING_STD_FLAGS )
};


////////////// RAIDER EQUIP //////////////


monst_base mb_raidship = {
(char *)"Raidship",(char *)"Vehicle",
(char *)"It is a raider ship.",
ai_hunter,
5500 /* max_hitpoints */,
0 /* max_age (ms) */,
0 /* mature_age (ms) */,
17  /* speed */, 9 /* lat_speed */, 80 /* turn_rate */,
0 /* str */, 0  /* attack_rate (ms/attack) */,
0 /* stomach */,
1800 /* size */,
0 /* met_rate (ms/nutritionpt) */,
0 /* nutrish */,
6000 /* energy */,
0 /* od_thresh */,
0 /* gestation (ms)*/,
250 /* agression */,
SHIP_STD_FLAGS /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
2 /* herd_size */,
-1200 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(raidship)
};

monst_base mb_raider = {
(char *)"Raider",(char *)"Humanoid",
(char *)"Raiders have guns and like shooting things with them.  "
"The raider motto is 'If it moves shoot it.'",
ai_hunter,
100 /* max_hitpoints */,
0 /* max_age (ms) */,
0 /* mature_age (ms) */,
10  /* speed */, 8 /* lat_speed */,  180 /* turn_rate */,
60 /* str */,    100  /* attack_rate (ms/attack) */,
0 /* stomach */,
280 /* size */,
40 /* met_rate (ms/nutritionpt) */,
500 /* nutrish */,
900 /* energy */,
500 /* od_thresh */,
0 /* gestation (ms)*/,
100000 /* agression */,
0 /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
3 /* herd_size */,
-500 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(raider)
};

monst_base mb_raidercamp = {
BUILDING(raidercamp, (char *)"Raider Camp",(char *)"Building",(char *)"You found a secret raider camp!",ai_building, 18000, 6000, BUILDING_STD_FLAGS )
};




////////////// POLICE EQUIP //////////////

monst_base mb_sentry = {
(char *)"Sentry",(char *)"Humanoid",
(char *)"Although sentry battlesuits were first designed for police use, "
"they are now mostly used by independent security agencies.",
ai_tank,
140 /* max_hitpoints */,
0 /* max_age (ms) */,
0 /* mature_age (ms) */,
10  /* speed */, 8 /* lat_speed */,  180 /* turn_rate */,
0 /* str */,    0  /* attack_rate (ms/attack) */,
0 /* stomach */,
280 /* size */,
0 /* met_rate (ms/nutritionpt) */,
500 /* nutrish */,
900 /* energy */,
500 /* od_thresh */,
0 /* gestation (ms)*/,
1000 /* agression */,
0 /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
3 /* herd_size */,
100 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(sentry)
};

monst_base mb_sentinel = {
(char *)"Sentinel",(char *)"Humanoid",
"Sentinels incorporate the most advanced battlesuit technology to date, "
"their myzerium mesh shells offer surprising resilience backed by high powered hydraulics "
"able to hold larger more powerful weapons than other suits.",
ai_tank,
400 /* max_hitpoints */,
0 /* max_age (ms) */,
0 /* mature_age (ms) */,
10  /* speed */, 8 /* lat_speed */,  180 /* turn_rate */,
0 /* str */,    0  /* attack_rate (ms/attack) */,
0 /* stomach */,
320 /* size */,
0 /* met_rate (ms/nutritionpt) */,
500 /* nutrish */,
900 /* energy */,
500 /* od_thresh */,
0 /* gestation (ms)*/,
1000 /* agression */,
0 /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
3 /* herd_size */,
300 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(sentinel)
};

monst_base mb_squadship = {
(char *)"Squadship",(char *)"Vehicle",
(char *)"Just remember, the police are our friends...  They keep us safe from the various "
"criminal factions that plague the belt.",
ai_tank,
1500 /* max_hitpoints */,
0 /* max_age (ms) */,
0 /* mature_age (ms) */,
10  /* speed */,  8 /* lat_speed */, 180 /* turn_rate */,
0 /* str */, 0  /* attack_rate (ms/attack) */,
0 /* stomach */,
500 /* size */,
0 /* met_rate (ms/nutritionpt) */,
0 /* nutrish */,
6000 /* energy */,
0 /* od_thresh */,
0 /* gestation (ms)*/,
250 /* agression */,
SHIP_STD_FLAGS /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
0 /* herd_size */,
800 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(squadship)
};

monst_base mb_watcher = {
(char *)"Watcher",(char *)"Vehicle",
(char *)"Watchers are powerful battle mechas used by many private organizations.  Their mobility and internal power "
"supplies make them more versatile than standard defense turrets.",
ai_tank,
5500 /* max_hitpoints */,
0 /* max_age (ms) */,
0 /* mature_age (ms) */,
14  /* speed */, 8 /* lat_speed */,  190 /* turn_rate */,
0 /* str */,    0  /* attack_rate (ms/attack) */,
0 /* stomach */,
3000 /* size */,
0 /* met_rate (ms/nutritionpt) */,
0 /* nutrish */,
6000 /* energy */,
0 /* od_thresh */,
0 /* gestation (ms)*/,
250 /* agression */,
BUILDING_STD_FLAGS /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
0 /* herd_size */,
0 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(watcher)
};


monst_base mb_enforcer = {
(char *)"Enforcer",(char *)"Vehicle",
(char *)"Enforcers are extremely powerful police battle mechas.  "
"If you ever encounter one of these, you'd better be a law abiding citizen or else.",
ai_tank,
8500 /* max_hitpoints */,
0 /* max_age (ms) */,
0 /* mature_age (ms) */,
10  /* speed */, 8 /* lat_speed */,  180 /* turn_rate */,
200 /* str */,    500  /* attack_rate (ms/attack) */,
0 /* stomach */,
4000 /* size */,
0 /* met_rate (ms/nutritionpt) */,
0 /* nutrish */,
6000 /* energy */,
0 /* od_thresh */,
0 /* gestation (ms)*/,
250 /* agression */,
BUILDING_STD_FLAGS /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
0 /* herd_size */,
2000 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(enforcer)
};

monst_base mb_policestation   = {
BUILDING(policestation, (char *)"Police Station",(char *)"Building",(char *)"Asteroid Police Department.",ai_building, 10000, 8000, BUILDING_STD_FLAGS )
};

monst_base mb_cityblda  = {
BUILDING(cityblda, (char *)"Building",(char *)"Building",(char *)"A rather uninteresting building.",ai_building, 18000, 6000, BUILDING_STD_FLAGS )
};

monst_base mb_citybldb  = {
BUILDING(citybldb, (char *)"Building",(char *)"Building",(char *)"A rather uninteresting building.",ai_building, 16000, 4000, BUILDING_STD_FLAGS )
};




///////////// MISC HUMANS /////////////

// generic asteroid zero employee.  all classes of worker use this as their base.
monst_base mb_worker = {
(char *)"Worker",(char *)"Humanoid",
(char *)"This will be replaced by a computer gened description of the individual",
ai_worker,
100 /* max_hitpoints */,
M(5) /* max_age (ms) */,
0 /* mature_age (ms) */,
8  /* speed */,  7 /* lat_speed */,  180 /* turn_rate */,
50 /* str */,    400  /* attack_rate (ms/attack) */,
0 /* stomach */,
260 /* size */,
5 /* met_rate (ms/nutritionpt) */,
500 /* nutrish */,
900 /* energy */,
500 /* od_thresh */,
600000 /* gestation (ms)*/,
10 /* agression */,
AF_NOHOOK | AF_WANDERS | AF_FAMILY /* flags */,
&LightRed /* blood_color*/,
220 /* flight_dist*/,
0.0  /* fear_factor */,
2 /* herd_size */,
0 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(worker)
};


monst_base mb_hippie = {
(char *)"Hippie",(char *)"Humanoid",
(char *)"Not all guests are exactly welcome.  These environmentally conscious protestors like "
"to harass our other visitors; unfortunately, their lives are still protected by law.",
ai_animal,
15 /* max_hitpoints */,
M(5) /* max_age (ms) */,
0 /* mature_age (ms) */,
10  /* speed */, 8 /* lat_speed */,  180 /* turn_rate */,
45 /* str */,    500  /* attack_rate (ms/attack) */,
0 /* stomach */,
280 /* size */,
5 /* met_rate (ms/nutritionpt) */,
500 /* nutrish */,
400 /* energy */,
250 /* od_thresh */,
0 /* gestation (ms)*/,
500 /* agression */,
0 /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
5.0  /* fear_factor */,
15 /* herd_size */,
-100 /* fame */,            //hippiees will -100 from fame for every hippiee in you park (10 hippiees = -1000 fame)
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(hippie)
};


monst_base mb_trainer = {
(char *)"Monster Trainer Ship",(char *)"Vehicle",
(char *)"These guys love to train and collect monsters.",
ai_trainer,
5000 /* max_hitpoints */,
0 /* max_age (ms) */,
0 /* mature_age (ms) */,
13  /* speed */, 4 /* lat_speed */, 66 /* turn_rate */,
60 /* str */, 100  /* attack_rate (ms/attack) */,
0 /* stomach */,
900 /* size */,
0 /* met_rate (ms/nutritionpt) */,
0 /* nutrish */,
6000 /* energy */,
0 /* od_thresh */,
0 /* gestation (ms)*/,
250 /* agression */,
SHIP_STD_FLAGS /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
1.10  /* fear_factor */,
0 /* herd_size */,
0 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(trainer)
};

monst_base mb_merchant = {
(char *)"Merchant Ship",(char *)"Vehicle",
(char *)"Merchants go from asteroid to asteroid buying and selling stuff.",
ai_merchant,
22000 /* max_hitpoints */,
0 /* max_age (ms) */,
0 /* mature_age (ms) */,
12  /* speed */, 4 /* lat_speed */, 40 /* turn_rate */,
60 /* str */, 100  /* attack_rate (ms/attack) */,
0 /* stomach */,
1800 /* size */,
0 /* met_rate (ms/nutritionpt) */,
0 /* nutrish */,
8000 /* energy */,
0 /* od_thresh */,
0 /* gestation (ms)*/,
250 /* agression */,
SHIP_STD_FLAGS /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
1.13  /* fear_factor */,
0 /* herd_size */,
0 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(merchant)
};

monst_base mb_tourist = {
(char *)"Tourist Shuttle",(char *)"Vehicle",
(char *)"Tourists are cool.  These guys love to see lots of big famous monsters, and pay money to do it.",
ai_tourist,
5000 /* max_hitpoints */,
0 /* max_age (ms) */,
0 /* mature_age (ms) */,
12  /* speed */, 4 /* lat_speed */, 40 /* turn_rate */,
60 /* str */, 100  /* attack_rate (ms/attack) */,
0 /* stomach */,
500 /* size */,
0 /* met_rate (ms/nutritionpt) */,
0 /* nutrish */,
6000 /* energy */,
0 /* od_thresh */,
0 /* gestation (ms)*/,
250 /* agression */,
SHIP_STD_FLAGS /* flags */,
NULL /* blood_color*/,
250 /* flight_dist*/,
99.9  /* fear_factor */,
5 /* herd_size */,
0 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(tourist)
};


monst_base mb_hunter = {
(char *)"Hunter",(char *)"Humanoid",
(char *)"Hunters get a thrill out of blasting the hell out of monsters "
"and are willing to pay big bucks to do it.",
ai_hunter,
80 /* max_hitpoints */,
M(5) /* max_age (ms) */,
0 /* mature_age (ms) */,
8  /* speed */,  7 /* lat_speed */,  180 /* turn_rate */,
200 /* str */,    400  /* attack_rate (ms/attack) */,
0 /* stomach */,
260 /* size */,
40 /* met_rate (ms/nutritionpt) */,
500 /* nutrish */,
900 /* energy */,
500 /* od_thresh */,
0 /* gestation (ms)*/,
10000 /* agression */,
0 /* flags */,
&LightRed /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
6 /* herd_size */,
0 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(hunter)
};




/////////// MISC BUILDINGS ///////////

monst_base mb_jmart = {
BUILDING(jmart, (char *)"J-Mart",(char *)"Building",(char *)"Sells stuff cheep.",ai_building, 10000, 10000, BUILDING_STD_FLAGS )
};

monst_base mb_bfc = {
BUILDING(bfc, (char *)"Buddy's Fried Chicken",(char *)"Building",(char *)"Original or Super crispy \"chicken\" served the way your momma used to make it.",ai_building, 10000, 10000, BUILDING_STD_FLAGS )
};

monst_base mb_arena = {
BUILDING(arena, (char *)"Monster Arena",(char *)"Building",(char *)"Home of the Monster, Monster, Asteroid Maddness!",ai_building, 18000, 40000, BUILDING_STD_FLAGS )
};

monst_base mb_fisec = {
BUILDING(fisec, (char *)"Fisec Weapons Factory",(char *)"Building",(char *)"Fisec is the galaxy's leading developer and distributor of weapons and defense systems.",ai_building, 32000, 10000, BUILDING_STD_FLAGS )
};

monst_base mb_wisehut  = {
BUILDING(wisehut, (char *)"Hut",(char *)"Building",(char *)"A generic wise old guy is said to live here.",ai_building, 6000, 5000, BUILDING_STD_FLAGS )
};

///////////// ORICK EQUIPMENT /////////


monst_base mb_orickcamp = {
BUILDING(orickcamp, (char *)"Base Camp",(char *)"Building",(char *)"You found the base camp of one of the orick tribes!",ai_building, 9000, 4000, BUILDING_STD_FLAGS )
};
monst_base mb_orickcampfire = {
BUILDING(orickcampfire, (char *)"Campfire", (char *)"Building", (char *)"It looks like someone made a campfire.",ai_building, 9000, 100, BUILDING_STD_FLAGS )
};



/////////// CRYSILLIAN EQUIPMENT //////////

monst_base mb_crysillianruins  = {
BUILDING(crysillianruins, (char *)"Ruins",(char *)"Building",(char *)"Ruins of a long forgotten civilization lay here.",ai_building, 26000, 6000, BUILDING_STD_FLAGS |AF_BOMB )
};
monst_base mb_crashsite  = {
BUILDING(crashsite, (char *)"Alien Crash Site",(char *)"Crash Site",(char *)"This appears to be a crashed alien spacecraft.",ai_building, 6000, 12000, BUILDING_STD_FLAGS | AF_BOMB)
};

monst_base mb_ufo = {
(char *)"UFO",(char *)"Vehicle",
(char *)"unidentified particle emissions detected ... sensor readings reveal nothing ... database reveals nothing ... "
"computer analysis: inconclusive ... designating target: U.F.O. ",
ai_tank,
40000 /* max_hitpoints */,
0 /* max_age (ms) */,
0 /* mature_age (ms) */,
30  /* speed */, 24 /* lat_speed */,  200 /* turn_rate */,
0 /* str */,    0  /* attack_rate (ms/attack) */,
0 /* stomach */,
3000 /* size */,
0 /* met_rate (ms/nutritionpt) */,
0 /* nutrish */,
999999 /* energy */,
0 /* od_thresh */,
0 /* gestation (ms)*/,
250 /* agression */,
SHIP_STD_FLAGS | AF_BOMB /* flags */,
NULL /* blood_color*/,
200 /* flight_dist*/,
0.0  /* fear_factor */,
0 /* herd_size */,
2000 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(ufo)
};

monst_base mb_hole = {
BUILDING(hole, (char *)"Hole",(char *)"Hole",(char *)"A hole is a space that used to have some substance there, but no longer does. Some monters have been known to dig them.", ai_building,  100, 5000, (AF_INORGANIC | AF_NOHOOK |  AF_ASEXUAL | AF_NOSHOOT ))
};

monst_base mb_meteor = {
(char *)"Meteor",(char *)"Meteor",
(char *)"Its a falling sky rock... stop reading and get out of its way.", 
ai_meteor,  
5000 /* max_hitpoints */,
0 /* max_age (ms) */,
0 /* mature_age (ms) */,
10  /* speed */, 10 /* lat_speed */,  5 /* turn_rate */,
0 /* str */,    0  /* attack_rate (ms/attack) */,
0 /* stomach */,
8000 /* size */,
0 /* met_rate (ms/nutritionpt) */,
0 /* nutrish */,
999999 /* energy */,
0 /* od_thresh */,
0 /* gestation (ms)*/,
0 /* agression */,
(AF_INORGANIC | AF_NOHOOK |  AF_ASEXUAL | AF_FLYING | AF_NOSHOOT ) /* flags */,
NULL /* blood_color*/,
0 /* flight_dist*/,
0.0  /* fear_factor */,
0 /* herd_size */,
-100 /* fame */,
0 /* bfc price */,
0 /* combat price */,
0 /* commonality*/,
LISTS(meteor)
};  



	


