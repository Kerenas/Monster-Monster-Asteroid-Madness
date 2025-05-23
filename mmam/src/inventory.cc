#define INV_DEF_MACROS 1
#include "mmam.h"
#include "rand.h"

#define ACKATOOSPIT (WP_SKATR | WP_SPIN | WP_INTERNAL | WP_SOLID)
#define FBALL       (WP_SKATR | WP_INTERNAL | WP_FIRE)
#define VRXPSY      (WP_PSY | WP_SPIN | WP_INTERNAL)
#define YZTPSY      (WP_PSY | WP_SPIN | WP_INTERNAL)
#define HE          (WP_EXPL)
#define DEATHRAY    (WP_FLASH | WP_EXPL)
#define RACHET      (WP_SKATR | WP_SOLID)
#define GALTIX      (WP_SKATR | WP_INTERNAL)
#define RPG         (WP_INTERNAL | WP_EXPL | WP_SKATR | WP_SOLID)
#define POISPEAR    (WP_SLEEP | WP_INTERNAL | WP_SOLID)
#define DART        (WP_SLEEP | WP_SOLID)
#define POISPORE    (WP_SLEEP | WP_INTERNAL | WP_SPORE)
#define PSYSPORE    (WP_CHEM  | WP_INTERNAL | WP_SPORE)
#define PYRSPORE    (WP_FIRE  | WP_INTERNAL | WP_SPORE)
#define MEEKBLAST   (WP_INTERNAL | WP_PSY | WP_EXPL)

void auto_fix_desc();

ivalu_t ivalu_list[NUM_IVALU_LIST] = {
// 	Jmart(0x1)    Astbase(0x2)    Arena(0x4)    Fisec(0x8)

// LIST BEGIN
/*typ          nam              lvl   LEN  jm cost common   front sprite*/
UP(inv_rope_up, "200 ft. rope",   0, 200,0x0,    0,  0,      NO_BIG_PIC),
UP(inv_rope_up, "300 ft. rope",   1, 300,0x5, 1000, 90,      ROPE1_INV),
UP(inv_rope_up, "400 ft. rope",   2, 400,0x5, 2000, 80,      ROPE2_INV),
UP(inv_rope_up, "500 ft. rope",   3, 500,0x4, 4000, 60,      ROPE3_INV),
UP(inv_rope_up, "600 ft. rope",   4, 600,0x4, 8000, 40,      ROPE4_INV),

/*typ          nam                       lvl  VEL   jm cost common   front sprite*/
UP(inv_claw_speed_up, "Claw Launcher 0",   0,  180,0x0,    0, 0,NO_BIG_PIC),
UP(inv_claw_speed_up, "Claw Launcher I",   1,  240,0x5, 1500,88,CLAWL1_INV),
UP(inv_claw_speed_up, "Claw Launcher II",  2,  320,0x5, 3000,75,CLAWL2_INV),
UP(inv_claw_speed_up, "Claw Launcher III", 3,  400,0x4,12000,55,CLAWL3_INV),
UP(inv_claw_speed_up, "Claw Launcher IV",  4,  600,0x4,30000,35,CLAWL4_INV),

/*      nam                 lvl FSPD LSPD TRATE   jm     cost common  front */
THRUST("Basic Thruster",     0,  12,   5,  40,   0x0,       0,  0,  NO_BIG_PIC),
THRUST("Thruster II",        1,  16,   4,  50,   0x1,    1500, 90,  THRUSTER1_INV),
THRUST("Thruster IIa",       2,  14,   5,  70,   0x1,    2200, 70,  THRUSTER2_INV),
THRUST("Ramjet Thruster",    3,  32,   0,  40,   0x1,    5000, 50,  THRUSTER3_INV),
THRUST("Hawk Thruster",      4,  22,   9,  90,   0x0,   15000, 20,  THRUSTER4_INV),
THRUST("Thruster Hi-A",      5,  20,  15, 120,   0x0,   25000, 10,  THRUSTER5_INV),
THRUST("Thruster Omega-X",   6,  30,  24, 200,   0x0,  135000, 10,  THRUSTER6_INV),

UP(inv_cage_stat_up, "Cage Analyzer", 0,0,0x0,10000,35,STATAN_INV),

/*    nam                     lvl    HP    jm   cost common  front   little*/
CAGE( "Aluminum Cage",          0,   3000,0x5,   500,90,  CAGE0_INV, CAGE0),
CAGE( "Steel Cage",             1,  10000,0x5,  2000,90,  CAGE1_INV, CAGE1),
CAGE( "Depleted Uranium Cage",  2,  30000,0x4,  7000,70,  CAGE2_INV, CAGE2),
CAGE( "Adamite Cage",           3,  60000,0x4, 17000,20,  CAGE3_INV, CAGE3),
CAGE( "EMF Retainer",           4, 100000,0x0, 90000,10,  CAGE4_INV, CAGE4), // self repairing
CAGE( "Cryopod",                5,  80000,0x0,200000, 1,  CAGE5_INV, CAGE5), // suspends monster in 30 sec


CLAW( "Spring-grip Claw",   0,   100,0x0,    0, 0,      CLAW0_INV,    CLAW0 ),
CLAW( "Pneumatic Claw",     1,   200,0x5, 2000,85,      CLAW1_INV,    CLAW1 ),
CLAW( "Hydraulic  Claw",    2,   300,0x5, 4000,70,      CLAW2_INV,    CLAW2 ),
CLAW( "Micro-tooth Claw",   3,   400,0x4,16000,50,      CLAW3_INV,    CLAW3 ),
CLAW( "R-172 Noslip Claw",  4,   500,0x4,45000,30,      CLAW4_INV,    CLAW4 ),

/*     nam              lvl     MHP  jm    cost raretiy   front sprite  ground sprite(overrides picture)*/
HULL( "Aluminum Hull",   0,     2000,0x9,    800,90, HULL0_INV, HULL0),
HULL( "Titanium Hull",   1,     5000,0x9,   2200,90, HULL1_INV, HULL1),
HULL( "Khabilum Hull",   2,    10000,0x8,   9000,70, HULL2_INV, HULL2),
HULL( "Carapace Hull",   3,    25000,0x8,  34000,30, HULL3_INV, HULL3),
HULL( "Myzerium Hull",   4,    60000,0x8, 120000, 9, HULL4_INV, HULL4),
HULL( "Biocite Hull",    5,    40000,0x0, 200000, 1, HULL5_INV, HULL5), // self repairing



/*    DAMSTYLE                        DAM  T0X  DRG  GRP  RNG VEL RELMS  jm   cost com  FRONTPIC             PRJPIC  HX,HY WP1                   WP2      sound*/
PROJ(DART,     "EZ Sleep darts",       0, 100, 100,  10, 300,110, 800, 0x5,   200, 85, DART1_INV,            DART1, 0,0,   NONE,             NONE,                TOSS_DART), // hovelers: (eyestalk)
PROJ(DART,     "Trank-quil darts",     0,  50, 200,  10, 300,130, 800, 0x5,   300, 80, DART2_INV,            DART2, 0,0,   NONE,             NONE,                TOSS_DART      ), // hovelers: (greencrstal)
PROJ(DART,     "Pacifyer darts",       0, 500, 300,  10, 300,140, 800, 0x4,   400, 70, DART3_INV,            DART3, 0,0,   NONE,             NONE,                TOSS_DART      ), // hovelers: (yellowrock)
PROJ(DART,     "Out Cold darts",       0, 300, 500,  10, 300,160, 800, 0x4,   600, 40, DART4_INV,            DART4, 0,0,   NONE,             NONE,                TOSS_DART      ), // hovelers: (purplerock)
PROJ(DART,     "Heart Attack darts",   0,1200,1000,  10, 300,160, 800, 0x4,  1000, 15, DART5_INV,            DART5, 0,0,   NONE,             NONE,                TOSS_DART      ),
PROJ(DART,     "Coma darts",           0, 800,2100,  10, 300,175, 800, 0x0,  1500, 10, DART6_INV,            DART6, 0,0,   NONE,             NONE,                TOSS_DART      ),
PROJ(DART,     "Gorgon darts",         0,1000,3500,  10, 300,180, 800, 0x0,  7500,  6, DART7_INV,            DART7, 0,0,   NONE,             NONE,                TOSS_DART      ),
PROJ(RACHET,   "Ratchet",             40,   0,   0,  60, 100, 80, 200, 0xb,   500, 90, GUN_1RACHET_INV,      MIS_1, 15,12, GUN_1RACHET,      GUN_1RACHET_F,       FIRE_RACHET      ),
PROJ(WP_STD,   "Rumbler",             30,   0,   0, 200, 100,100, 100, 0x9,  1500, 80, GUN_2RUMBLER_INV,     MIS_2, 15,10, GUN_2RUMBLER,     GUN_2RUMBLER_F,      FIRE_RUMBLER     ),
PROJ(WP_STD,   "Lancer",             200,   0,   0,  50, 530,200, 800, 0x9,  3000, 80, GUN_2LANCER_INV,      MIS_3, 15, 7, GUN_2LANCER,      GUN_2LANCER_F,       FIRE_LANCER      ),
PROJ(WP_STD,   "Morningstar",         45,   0,   0, 350, 230,130,  90, 0x9,  5000, 60, GUN_3MORNINGSTAR_INV, MIS_4, 15,13, GUN_3MORNINGSTAR, GUN_3MORNINGSTAR_F,  FIRE_MORNINGSTAR ),
PROJ(WP_STD,   "Railgun",            500,   0,   0,  75, 800,300, 500, 0x9,  7500, 60, GUN_3RAILGUN_INV,     MIS_5, 15,13, GUN_3RAILGUN,     GUN_3RAILGUN_F,      FIRE_RAILGUN     ),
PROJ(WP_FIRE,  "Incinerator",         50,   0,   0, 500, 190, 90, 100, 0x8,  8000, 50, GUN_3INCINERATOR_INV, FIREBALL, 15,13, GUN_3INCINERATOR, GUN_3INCINERATOR, FIRE_INCENERATOR ),
PROJ(WP_SKATR, "Stormhand",           90,   0,   0, 500, 210,150,  80, 0x8, 12000, 40, GUN_4STORMHAND_INV,   MIS_6, 15,15, GUN_4STORMHAND,   GUN_4STORMHAND_F,    FIRE_STORMHAND   ),
PROJ(WP_STD,   "Pulsar",             150,   0,   0,1000, 200,250, 300, 0x8, 14000, 40, GUN_4PULSAR_INV,      MIS_8, 15,12, GUN_4PULSAR,      GUN_4PULSAR_F,       FIRE_PULSAR      ),
PROJ(WP_STD,   "Sunstriker",        1500,   0,   0,  60, 300,200, 700, 0x8, 25000, 25, GUN_4SUNSTRIKER_INV,  MIS_7, 15,15, GUN_4SUNSTRIKER,  GUN_4SUNSTRIKER_F,   FIRE_SUNSTRIKER  ),
PROJ(HE,       "Pyroclaster",        900,   0,  50,  60, 250,100, 800, 0x8, 30000, 25, GUN_4PYROCLASTER_INV, MIS_10,15,15, GUN_4PYROCLASTER, GUN_4PYROCLASTER_F,  FIRE_PYROCLASTER ),
PROJ(DEATHRAY, "Deathray",          9000,   0, 200,  50, 800,150,1300, 0x0,200000,  1, GUN_5DEATHRAY_INV,    MIS_9, 15,10, GUN_5DEATHRAY,    GUN_5DEATHRAY_F,     FIRE_DEATHRAY    ),

/*    DAMSTYLE                              DAM  T0X  DRG  GRP  RNG VEL RELMS  ZERO     FRONTPIC             PRJPIC  HX,HY WP1                   WP2     sound */
PROJ(WP_INTERNAL, "Gun Turret",              50,   0,   0,2000, 600,300, 500, 0x0,0,0, BTURRET_GUN,         MIS_00, 15,42, B_GUN,           B_GUN,     FIRE_GUNCANNON    ),
PROJ(WP_INTERNAL, "DG Turret",              100,   0,   0,2000, 750,300, 400, 0x0,0,0, BTURRET_DUELGUN,     MIS_01, 15,40, B_DUELGUN,       B_DUELGUN, FIRE_DGCANNON ),
PROJ(WP_INTERNAL, "X-Cannon Turret",        500,   0,   0,1000,1200,300,1000, 0x0,0,0, BTURRET_CANNON,      MIS_02, 15,42, B_CANNON,        B_CANNON,  FIRE_XCANNON  ),
PROJ(GALTIX,      "Galtix Turret",           80,   0,   0,9999, 450,300, 100, 0x0,0,0, BTURRET_MACHGUN,     MIS_03, 15,36, B_MACHGUN,       B_MACHGUN, FIRE_GALTIX ),

PROJ(WP_INTERNAL,  "Gun Turret Raid",        75,   0,   0,2000, 600,300, 500, 0x0,0,0, RTURRET_GUN,         MIS_00, 15,42, R_GUN,           R_GUN,     FIRE_GUNCANNON     ),
PROJ(WP_INTERNAL,  "DG Turret Raid",        150,   0,   0,2000, 750,300, 400, 0x0,0,0, RTURRET_DUELGUN,     MIS_01, 15,40, R_DUELGUN,       R_DUELGUN, FIRE_DGCANNON ),
PROJ(WP_INTERNAL,  "X-Cannon Turret Raid",  750,   0,   0,1000,1200,300,1000, 0x0,0,0, RTURRET_CANNON,      MIS_02, 15,42, R_CANNON,        R_CANNON,  FIRE_XCANNON  ),
PROJ(GALTIX,       "Galtix Turret Raid",    120,   0,   0,9999, 450,300, 100, 0x0,0,0, RTURRET_MACHGUN,     MIS_03, 15,36, R_MACHGUN,       R_MACHGUN, FIRE_GALTIX ),

PROJ(WP_INTERNAL, "Squad Blaster",          120,   0,   0,1000, 400,300, 300, 0x0,0,0, NO_BIG_PIC,          MIS_04, 15,26, P_GUN,           P_GUN_F,       FIRE_SQUADBLAST     ),
PROJ(WP_INTERNAL, "Watcher DG",             150,   0,   0,2000, 750,340, 330, 0x0,0,0, NO_BIG_PIC,          MIS_01, 15,29, WAT_DG,          WAT_DG_F,      FIRE_DGCANNON ),
PROJ(WP_INTERNAL, "Enforcer X-Cannon",      900,   0,   0,9999, 800,300,1000, 0x0,0,0, NO_BIG_PIC,          MIS_05, 15,26, ENF_CANNON,      ENF_CANNON_F,  FIRE_XCANNON  ),
PROJ(RPG,         "Enforcer RPG",           300,   0,  50,  64, 350,190, 150, 0x0,0,0, NO_BIG_PIC,          MIS_06, 15,26, ENF_CANNON,      ENF_CANNON_F2, FIRE_ENFROCKET ),

PROJ(ACKATOOSPIT, "Ackatoo Spit",            80,   0,   0,9999, 100,160, 900, 0x0,0,0, NO_BIG_PIC,         AKT_SPIT, 15,0, NONE,             NONE, FIRE_AKTSPIT    ),
PROJ(MEEKBLAST,   "Meeker Blast",          1000,   0,  40,9999, 550,270,1100, 0x0,0,0, NO_BIG_PIC,         MIS_9,    15,0, NONE,             NONE, FIRE_MEEKBLAST  ),
PROJ(VRXPSY,      "Vrx Psy Attack",         100,   0,  80,9999, 400,200, 900, 0x0,0,0, NO_BIG_PIC,         PSY_1,    15,0, NONE,             NONE, FIRE_PSY   ),
PROJ(YZTPSY,      "Yzt Psy Attack",         200,   0, 100,9999, 400,220, 800, 0x0,0,0, NO_BIG_PIC,         PSY_2,    15,0, NONE,             NONE, FIRE_PSY   ),
PROJ(FBALL,       "Fireball",                50,   0,   0,9999, 190, 90, 100, 0x0,0,0, NO_BIG_PIC,         FIREBALL, 15,0, NONE,             NONE, FIRE_DRAG  ),
PROJ(POISPORE,    "Poison Spore",             0, 220,  40,9999,  50,100, 100, 0x0,0,0, NO_BIG_PIC,         SPORESG,  15,0, NONE,             NONE, FIRE_SPORE ),
PROJ(PSYSPORE,    "Psycho Spore",             0,   0,   0,9999, 125,100, 100, 0x0,0,0, NO_BIG_PIC,         SPORESB,  15,0, NONE,             NONE, FIRE_SPORE ), //needs a weapon class that just causes confusion.
PROJ(PYRSPORE,    "Pyro Spore",              10,   0,   0,9999,  70,100, 100, 0x0,0,0, NO_BIG_PIC,         SPORESR,  15,0, NONE,             NONE, FIRE_SPORE ),
PROJ(POISPEAR,    "Spear",                  200, 500,   0,  10, 300,120, 800, 0x0,0,0, SPEAR_INV,          SPEAR,    0, 0, NONE,             NONE, TOSS_SPEAR ), // oricks: (stalkplant)

/*typ             nam                 lvl EXPL   pow  size   hp    jm  cost common   front sprite*/
DEPL(inv_power, "Solar Array",          0,   0,   50, 1000,  550, 0x2,  1000, 53, BGEN1_INV, BGEN1_1),
DEPL(inv_power, "Fission Plant",        1,2500,  500, 4000, 8300, 0x0,  6000, 43, BGEN2_INV, BGEN2_1),
DEPL(inv_power, "Fusion Plant",         2,   0, 1000, 8000, 8000, 0x0, 10000, 23, BGEN3_INV, BGEN3_1),

DEPL(inv_power, "Power Core",          -1,2860,  800,   80,  300, 0x0, 10000,  0, CGEN_INV, CGEN_2),

/*typ             nam                lvl sv POW size   hp   jm   cost common   front sprite*/
DEPL(inv_scanner, "Radio Marker",       0, 0,  0,  180, 1000, 0x1,  400,70, SCAN0_INV, NONE),
DEPL(inv_scanner, "Organics Scanner",   1, 1, 10, 2000, 4000, 0x1,  800,60, SCAN1_INV, NONE),
DEPL(inv_scanner, "Delta Wave Scanner", 2, 2, 20, 3500, 5000, 0x1, 2800,30, SCAN2_INV, NONE),
DEPL(inv_scanner, "Q-Particle Scanner", 3, 3, 20, 4800, 6200, 0x0, 6000,16, SCAN3_INV, NONE),
DEPL(inv_scanner, "Epsilon Hi Scanner", 4, 4, 80, 6000, 7600, 0x0,15000,12, SCAN4_INV, NONE),

DEPL(inv_tower, "Gun Tower",            1, 0, 50, 1100, 4000, 0xa,  8000,  0, BTURRET_GUN, BTURRET),
DEPL(inv_tower, "DG Tower",             2, 0, 80, 1100, 4400, 0xa, 12000,  0, BTURRET_DUELGUN, BTURRET),
DEPL(inv_tower, "X-Cannon Tower",       3, 0,180, 1100, 5500, 0x8, 16000,  0, BTURRET_CANNON, BTURRET),
DEPL(inv_tower, "Galtix Tower",         4, 0,300, 1100, 5500, 0x8, 20000,  0, BTURRET_MACHGUN, BTURRET),

//all enclosure segments = 42*42 pxl   // mhp = mhp * segements in enclosure
//use box-X cursors where laying sides of encloser and cage cursor for top area. Green denotes it is okay to deploy here
//red denotes where it is not, blue denotes that cage here is being annexed into new enclosor.
//Pin = 2*2, Enclosure = 3*3, Exhibit =  4*4, Grand Exhibit =  6*6  cage segments.
//
//                                          lvl capcity pow    size    hp   jm   cost common  front    sprite
DEPL(inv_encl, "Chainlink Pin",               1,    600,  0,  28224, 2000, 0x2,   800,    0, ENCL1_INV, ENCL1),
DEPL(inv_encl, "Chainlink Enclosure",         1,    600,  0,  44100, 4500, 0x0,  1800,    0, ENCL1_INV, ENCL1),
DEPL(inv_encl, "Chainlink Exhibit",           1,    600,  0,  63504, 8000, 0x0,  3200,    0, ENCL1_INV, ENCL1),
DEPL(inv_encl, "Chainlink Grand Exhibit",     1,    600,  0, 112896, 9000, 0x0,  6400,    0, ENCL1_INV, ENCL1),
DEPL(inv_encl, "Grated Pin",                  2,   6000,  0,  28224, 6000, 0x2,  2750,    0, ENCL2_INV, ENCL2),
DEPL(inv_encl, "Grated Enclosure",            2,   6000,  0,  44100, 9000, 0x0,  5500,    0, ENCL2_INV, ENCL2),
DEPL(inv_encl, "Grated Exhibit",              2,   6000,  0,  63504,11000, 0x0, 11000,    0, ENCL2_INV, ENCL2),
DEPL(inv_encl, "Grated Grand Exhibit",        2,   6000,  0, 112896,14000, 0x0, 21000,    0, ENCL2_INV, ENCL2),
DEPL(inv_encl, "Shielded Pin",                3,  60000, 20,  28224,20000, 0x2,  4000,    0, ENCL3_INV, ENCL3),
DEPL(inv_encl, "Shielded Enclosure",          3,  60000, 45,  44100,30000, 0x0,  9000,    0, ENCL3_INV, ENCL3),
DEPL(inv_encl, "Shielded Exhibit",            3,  60000, 80,  63504,40000, 0x0, 16000,    0, ENCL3_INV, ENCL3),
DEPL(inv_encl, "Shielded Grand Exhibit",      3,  60000,160, 112896,50000, 0x0, 32000,    0, ENCL3_INV, ENCL3),

//stabloizers are upgrades of your warpgate node, not buildings that can be dropped where ever.
//                                              lvl       recrg pow   size   hp    jm   cost common   front           sprite
DEPL(inv_stabilizer, "Jumpgate Node",            0,180000,  0,  2000,10000, 0x0,     0,    0, STABILIZER0_INV, STABILIZER0),
DEPL(inv_stabilizer, "Jumpgate Stabilizer",      1,120000, 20, 16000,24000, 0x0,  5000,    0, STABILIZER1_INV, STABILIZER1),
DEPL(inv_stabilizer, "Trans-Phasite Stabilizer", 2, 90000, 50, 16000,26000, 0x0, 10000,    0, STABILIZER2_INV, STABILIZER2),
DEPL(inv_stabilizer, "Multi-Phasite Stabilizer", 3, 60000, 90, 16000,28000, 0x0, 22000,    0, STABILIZER3_INV, STABILIZER3),
DEPL(inv_stabilizer, "Omni-Phasite Stabilizer",  4, 45000,150, 16000,33000, 0x0, 45000,    0, STABILIZER4_INV, STABILIZER4),


DEPL(inv_tower, "Raider Gun Tower",       1, 0, 50, 1100, 4000, 0x0, 0, 0, RTURRET_GUN, RTURRET),
DEPL(inv_tower, "Raider DG Tower",        2, 0, 80, 1100, 4400, 0x0, 0, 0, RTURRET_DUELGUN, RTURRET),
DEPL(inv_tower, "Raider X-Cannon Tower",  3, 0,180, 1100, 5500, 0x0, 0, 0, RTURRET_CANNON, RTURRET),
DEPL(inv_tower, "Raider Galtix Tower",    4, 0,300, 1100, 5500, 0x0, 0, 0, RTURRET_MACHGUN, RTURRET),

DEPL(inv_power, "Raider Solar Array",     0,   0,   50, 1000,  550, 0x0,0,  0, RGEN1_INV, RGEN1_1),
DEPL(inv_power, "Raider Fission Plant",   1,2500,  500, 4000, 8300, 0x0,0,  0, RGEN2_INV, RGEN2_1),
DEPL(inv_power, "Raider Fusion Plant",    2,   0, 1000, 8000, 8000, 0x0,0,  0, RGEN3_INV, RGEN3_1),

AUTO(inv_food, "Mixed Vegetables",     0,0x1,   50, 30, MIXEDVEGETABLES),
AUTO(inv_food, "Eyestalk Shoots",      1,0x1,   90, 30, EYESTALKSHOOTS),
AUTO(inv_food, "Canned Furbit",        2,0x1,  150, 30, CANNEDFURBIT),
AUTO(inv_food, "Steamed Mubark",       3,0x1,  350, 30, STEAMEDMUBARK),
AUTO(inv_food, "Unlabeled Can",        4,0x1,   30, 15, UNLABELEDCAN), /* various animals [hovelers] */
AUTO(inv_food, "Marja-cola",           5,0x0, 1100,  8, MARJACOLA), /*randomized fight or flight response and adds 5-15 second death delay when HP reaches 0. [fern:cheech]*/
AUTO(inv_food, "IHB-77",               6,0x0, 2000,  4, IHB77), /* mellows animal and saps energy (ocasional death) [eyestalk:cheech]*/
AUTO(inv_food, "Vojo Dust",            7,0x0, 7000,  2, VOJODUST), /* Invis (Rarely Permant) [stalkerparts:stalker] */
AUTO(inv_food, "Zozin",                8,0x0, 3050,  6, ZOZIN), /* mellows animal and increases appatite (rare death) [happytreeparts:cheech]*/
AUTO(inv_food, "Lykedofan",            9,0x0, 2500,  4, LYKEDOFAN), /* Increases vision (Paroinoa sometimes) */
AUTO(inv_food, "Fried \"Chicken\"",    4,0x0,   20, 15, FRIEDCHICKEN), /* various animals */


AUTO(inv_injector, "Detox",                0,0x0,  800, 20, DETOX), /*cures poison (usually makes ansey) */
AUTO(inv_injector, "Rozolin",              1,0x0, 2500, 14, ROZOLIN), /* Minor healing and Improves Max Health (Paranioa sometimes) */
AUTO(inv_injector, "Glowing Liquid",       2,0x0, 1800,  9, GLOWINGLIQUID), /* Random Effect [greencrystal:cheech] */
AUTO(inv_injector, "Super-Stim",           3,0x0, 2200, 18, SUPERSTIM), /* Temporary energy 8xMax (Animals Somtimes Explode on this stuff) [redrock:cheech] */
AUTO(inv_injector, "JR-19AZF",             4,0x0, 3000,  8, JR19AZF),   /* Imunizes monster against sickness, OD damage, tranquilization, and most drug induced side-effects */
AUTO(inv_injector, "Ais Ormba",            5,0x0,12000,  1, AISORMBA), /* Sets Metrate to 0 and life expacantcy x50 (rarely blindness) */
AUTO(inv_injector, "Nanobots",             6,0x0, 7700, 20, NANOBOTS), /* Temp 5x Healing Rate */
AUTO(inv_injector, "HX-12",                7,0x0, 1800, 12, HX12), /* Causes Rapid Growth */
AUTO(inv_injector, "Morodulb",             8,0x0, 1500, 22, MORODULB), /* Increases Strength (known to make violent rarely canabolistic) [wormparts:cheech]*/
AUTO(inv_injector, "Project 17",           9,0x0, 2500,  5, PROJECT17), /* Causes (sometimes Intence Trans-spicies) Mutations [roachyparts:cheech]*/

AUTO(inv_mineral, "Red Rock",              1,0x0,   10,    8, REDROCK),
AUTO(inv_mineral, "Yellow Rock",           2,0x0,   50,    8, YELLOWROCK),
AUTO(inv_mineral, "Purple Rock",           3,0x0,   80,    8, PURPLEROCK),
AUTO(inv_mineral, "Green Crystals",        4,0x0,  100,    8, GREENCRYSTALS),
AUTO(inv_mineral, "Biocite",               5,0x0,25000,    1, BIOCITE), /* can be used to repair biocite hulls, cyropods, and recharge deathrays up to the nearest increment of 50.*/
AUTO(inv_money,   "Money",                 0,0x0,    0,    0, MONEY_INV),


UP(inv_repair,   "Repair All",    99, 0,0x2, 0,0, WRENCH2X_INV),
UP(inv_repair,   "Repair Hull",   98, 0,0x2, 0,0, WRENCH_INV  ),
UP(inv_repair,   "Repair Cage 1",  0, 0,0x6, 0,0, WRENCH_INV  ),
UP(inv_repair,   "Repair Cage 2",  1, 0,0x6, 0,0, WRENCH_INV  ),
UP(inv_repair,   "Repair Cage 3",  2, 0,0x6, 0,0, WRENCH_INV  ),
UP(inv_repair,   "Repair Cage 4",  3, 0,0x6, 0,0, WRENCH_INV  ),
UP(inv_repair,   "Repair Cage 5",  4, 0,0x6, 0,0, WRENCH_INV  ),
UP(inv_repair,   "Repair Cage 6",  5, 0,0x6, 0,0, WRENCH_INV  ),
UP(inv_repair,   "Repair Cage 7",  6, 0,0x6, 0,0, WRENCH_INV  ),
UP(inv_repair,   "Repair Cage 8",  7, 0,0x6, 0,0, WRENCH_INV  ),
UP(inv_repair,   "Repair Cage 9",  8, 0,0x6, 0,0, WRENCH_INV  ),
UP(inv_repair,   "Repair Cage 10", 9, 0,0x6, 0,0, WRENCH_INV  ),
UP(inv_repair,   "Repair Cage 11",10, 0,0x6, 0,0, WRENCH_INV  ),
UP(inv_repair,   "Repair Cage 12",11, 0,0x6, 0,0, WRENCH_INV  ),

UP(inv_hint,   "Minor Hint",1, 0,0x0,  1000,0, HINT1_INV),
UP(inv_hint,   "Major Hint",2, 0,0x0, 20000,0, HINT2_INV),

UP(inv_donation,   "Police Bribe", -1, 0,0x0,  0,0, MONEY_INV), 



// LIST END
};

void dump_docs(); // in dumpdoc.cc

void fix_ivalu_desc() {
	IDF_ROPEUP0.desc = IDF_ROPEUP1.desc = IDF_ROPEUP2.desc = IDF_ROPEUP3.desc =
	IDF_ROPEUP4.desc = "This kit increases the maximum range of your claw.";
	
	IDF_CLAWSPEEDUP0.desc = IDF_CLAWSPEEDUP1.desc = IDF_CLAWSPEEDUP2.desc = IDF_CLAWSPEEDUP3.desc =
	IDF_CLAWSPEEDUP4.desc = "This kit increases the launch speed of your claw.";
	
	IDF_RADIOMARKER.desc = "Drop Radio Markers on remote asteroids to track them.";
	IDF_ORGANICSSCANNER.desc = "These scanners give basic info about the creatures on the asteroid they are dropped on.";
	
	IDF_DELTAWAVESCANNER.desc = "Delta waves are used to track distant radio marked asteroids.  With 5 "
		"of these your stabilizer can reopen a jumpgate to any marked asteroid in the belt.";
	IDF_QPARTICLESCANNER.desc = "Q-Particles can be directed through a jumpgate to give a layout of a remote asteroid.";
	IDF_EPSILONHISCANNER.desc =
	"Epsilon Hi scanners give a detailed medical analysis of monsters on your asteroid.  These are necessary to "
	"detect infections in your park.";
	
	IDF_THRUST2.desc = "The Thruster IIa is the high agility version of the Thruster II.";
	IDF_THRUST3.desc = "The Ramjet Thruster sacrifices agility for the greatest raw speed available for any thruster.";
	IDF_THRUST4.desc = "The Hawk Thruster is a high performance thruster with good speed and agility.";
	IDF_THRUST5.desc = "The Thruster Hi-A is a high performance thruster with excellent speed and agility.";
	IDF_THRUST6.desc = "The Thruster Omega-X is a superbly high performance thruster meant only for pilots of "
					"the highest standards.";
	IDF_CAGESTATUP0.desc = "This mod shows how much damage your cages have taken.";
	
	IDF_CAGE0.desc = "Aluminum cages are cheap, but you get what you pay for.";
	IDF_CAGE1.desc = "Steel cages are much harder for monsters to destroy than aluminum.";
	IDF_CAGE2.desc = "Depleted uranium cages are built to hold monsters(tm)";
	IDF_CAGE3.desc = "Adamite cages are the toughest metal cages you can buy.";
	IDF_CAGE4.desc = "These cages are force field reinforced and will repair themselves constantly from their internal power supply.";
	IDF_CAGE5.desc = "Cryopods freeze captured animals into suspended animation.  "
		"Freezing takes a few seconds so you may want to trank really large animals first.";
	
	
	IDF_CLAW0.desc = IDF_CLAW1.desc = IDF_CLAW2.desc = IDF_CLAW3.desc =
	IDF_CLAW4.desc = "An improved claw is better for gripping larger monsters.";
	
	
	
	IDF_HULL0.desc = IDF_HULL1.desc = IDF_HULL2.desc = IDF_HULL3.desc =
	IDF_HULL4.desc = "Hull improvements increase the amount of damage your ship can take.";
	IDF_HULL5.desc = "This nonstandard hull is made from some strange rare biological material.";
	
	
	
	IDF_EZSLEEPDARTS.desc = "Really cheap tranks.  You'll probably need more than one to down most monsters.";
	IDF_TRANKQUILDARTS.desc = "These darts have an improved solution over easysleep darts that acts twice as quick.  "
				"It also has the least side affects of any dart.";
	IDF_PACIFYERDARTS.desc = "Good general purpose trank darts.";
	IDF_OUTCOLDDARTS.desc = "Good general purpose trank darts for medium sized monsters.";
	IDF_HEARTATTACKDARTS.desc = "These darts have a high concentration of drugs and do their job very well but are also quite toxic.";
	IDF_COMADARTS.desc = "Coma darts will put down just about anything.";
	IDF_GORGONDARTS.desc = "Gorgon darts are designed to take down really big monsters.  You'll probably kill anything small if you hit it with one of these.";
	
	IDF_RATCHET.desc = "A Ratchet is a cheap, short ranged, rapid fire weapon.  "
			"Its better than nothing, but you should find something else "
			"if you are planning on fighting anything big.";
	IDF_RUMBLER.desc = "This high rapidity machine gun is a favorite of raiders.";
	IDF_LANCER.desc = "A Lancer is an accurate long ranged weapon.  It is a favorite weapon for hunters.";
	IDF_MORNINGSTAR.desc = "The morningstar is an improvement on the rumbler design with a stronger powercore and a heavier body.";
	IDF_RAILGUN.desc = "Rail guns fire high velocity rounds at very long distances.";
	IDF_INCINERATOR.desc = "Incinerators project high velocity napalm.  Be cautious with this weapon as napalmn is "
					"easily spread from your intended target to others.";
	IDF_STORMHAND.desc = "The Storm Hand spits out deadly plasma at blinding speeds.";
	IDF_SUNSTRIKER.desc = "Sun Strikers fire armor piercing shells capable of greater damage than any man-made weapon.";
	IDF_PYROCLASTER.desc = "Pyroclasters fire high yield howitzers ideal for devastating both vehicle and infantry formations alike.";
	IDF_PULSAR.desc = "Pulsars are strong, general purpose weapons with a very large supply of ammo.";
	IDF_DEATHRAY.desc = "Death rays fire a devastatingly powerful fusion induction bolt.";
	
	IDF_SOLARARRAY.desc = "Solar cells provide reliable safe energy for your base (just not much of it).";
	IDF_FISSIONPLANT.desc = "Fusion plants provide a large supply of energy for your base, but beware they "
		"tend to blow up things around them if shot up too much.";
	IDF_FUSIONPLANT.desc = "Fusion plants are much safer than the fission plants and put out a whopping ammount of energy.";
	IDF_POWERCORE.desc = "A power core emanates a large amount of energy, from some unknown source.  "
		"They seem somewhat unstable though.";
	
	IDF_GUNTOWER.desc = "A gun tower provides extra security for your base at an affordable price";
	IDF_DGTOWER.desc = "The DG tower has twice the laser gun for twice the fun.";
	IDF_GALTIXTOWER.desc = "Galtix lasers are the most powerful machine guns on the market, perfect for defending a base against any threat.";
	IDF_XCANNONTOWER.desc = "X-Cannon towers are armed with high-powered photon cannons.";
	
	
	IDF_CHAINLINKPIN.desc = IDF_CHAINLINKENCLOSURE.desc = IDF_CHAINLINKEXHIBIT.desc = IDF_CHAINLINKGRANDEXHIBIT.desc =
						"These shallow enclosures are too small for most monsters; however, they are cheap making "
						"them ideal for storing large numbers of feeder animals and plants away from your park's "
						"general population.";
	IDF_GRATEDPIN.desc = IDF_GRATEDENCLOSURE.desc = IDF_GRATEDEXHIBIT.desc = IDF_GRATEDGRANDEXHIBIT.desc =
							"Sturdier and deeper than chainlink, these enclosures are well equipped for most of your "
							"storage needs.";
	IDF_SHIELDEDPIN.desc = IDF_SHIELDEDENCLOSURE.desc = IDF_SHIELDEDEXHIBIT.desc = IDF_SHIELDEDGRANDEXHIBIT.desc =
							"Spacey and near impregnable, there is nothing this can't contain; however, it's energy "
							"requirements will put a major strain on your park's power grid.";
	IDF_JUMPGATENODE.desc =  "";
	IDF_JUMPGATESTABILIZER.desc = "Jumpgates are subspace tunnels formed by impacting centrigon particles under controlled circumstances.  "
								"Stabilizers improve the accuracy of this impact allowing for controlled destinations.";
	IDF_TRANSPHASITESTABILIZER.desc = "Trans-phasite stabilizers unilaterally bend subspace allowing longer jumps than possible "
									"than with conventional warpgate technology.";
	IDF_MULTIPHASITESTABILIZER.desc = "Multi-phasite stabilizers coil subspace into multiple extraneous dimensions "
									"permitting the jumpgate to remain open for longer.";
	IDF_OMNIPHASITESTABILIZER.desc = "Unlike most stabilizers, Omni-phasite stabilizers have a string/plate sequencer ring which can open a "
									"jumpgate to any asteroid in scanner range, not just ones with radio markers.";
	
	IDF_RAIDERGUNTOWER.desc = "Raider gun towers provides extra security at most raider camps";
	IDF_RAIDERDGTOWER.desc = "The raider DG Tower is a standard DG Tower painted red to match the whole raider motif.";
	IDF_RAIDERGALTIXTOWER.desc = "Galtix lasers are the most powerful machine guns on the market, and raiders like to deploy them when possible.";
	IDF_RAIDERXCANNONTOWER.desc = "It is uncommon, but not unheard of, for raiders to posses the X-Cannon towers.  These things pack a lot of punch so it is usually best to avoid these.";
	
	
	// IDF_ACATOOSPIT.desc = IDF_VRXPSYATTACK.desc = IDF_YZTPSYATTACK.desc = IDF_FIREBALL.desc = "";
	
	IDF_MIXEDVEGETABLES.desc = "The can's label says: I'm a delightful blend of assorted vegetables to make you happy."; 
	IDF_CANNEDFURBIT.desc = "Ingrediants: mechanically separated furbit, water, less than 2% of salt, sugar, ascorbic acid (to protect color), citric acid, artificial and natural flavors"; IDF_STEAMEDMUBARK.desc = "This can has a picture of a cartoon smiling moobark on the label with the caption: You'll love our steamed moobark, chunked and formed!";
	//IDF_UNLABELEDCAN.desc = "This can of food has lost it's label (that's why it's cheep), but it could really be anything!";
	IDF_FRIEDCHICKEN.desc = "This stuff is their best ultra tasty super crunchy recipe.";
	IDF_EYESTALKSHOOTS.desc = "Freeze dried eyestalks are tasty to quite a number of animals.";
	
	IDF_MARJACOLA.desc     = "The can says \"Marja Cola.  Making fun times since 2046.\"";
	
	
	IDF_IHB77.desc         = IDF_VOJODUST.desc =
	IDF_ZOZIN.desc         = IDF_LYKEDOFAN.desc = IDF_ROZOLIN.desc =
	IDF_GLOWINGLIQUID.desc = IDF_SUPERSTIM.desc = IDF_JR19AZF.desc =
	IDF_AISORMBA.desc      = IDF_HX12.desc      = IDF_MORODULB.desc =
	IDF_PROJECT17.desc = "???";
	
	IDF_DETOX.desc = "Detox reduces poisoning caused by dart and kills off some kinds of infections.";
	IDF_NANOBOTS.desc = "Nanobots, once injected, are designed to quickly repair damaged tissue and close open wounds.";
	
	IDF_REDROCK.desc = IDF_YELLOWROCK.desc = IDF_PURPLEROCK.desc = "Some sort of mineral found in the asteroid belt";
	IDF_GREENCRYSTALS.desc = "These are pretty green crystals.";
	IDF_BIOCITE.desc = "Biocite, a rare alloy found only on the asteroid belt, is used in the manufacture of some of the most technological advanced devices known to man.";
	IDF_MONEY.desc = "The astrobuck is the most commonly used currency in the belt.";
	for (int c = 0; c < NUM_INV_REPAIR; c++) ivalu_list[FIRST_INV_REPAIR_IDX + c].desc = "Full repairs on your cage.";
	IDF_REPAIR99.desc = "Completely repair your hull and all cages.";
	IDF_REPAIR98.desc = "Completely repair your hull.";
	IDF_POWERCORE.desc = "This amazingly small device outputs a lot of power for its size.  It appears to be rather unstable though.";
	
	mb_powercore.flags = AF_INORGANIC;
	mb_redrock.fame = 10;
	mb_yellowrock.fame = 10;
	mb_purplerock.fame = 25;
	mb_greencrystals.fame = 85;
	mb_biocite.fame = 120;
	auto_fix_desc();
	dump_docs();
}

void monst_t::issue_weaps(int mutate) {
      	int r = irand(100);
     	if (base == &mb_ackatoo) { give_weap(&IDF_ACKATOOSPIT); }
   	if (base == &mb_vaerix) { give_weap(&IDF_VRXPSYATTACK); }
   	if (base == &mb_yazzert) { give_weap(&IDF_YZTPSYATTACK); }
   	if (base == &mb_dragoonigon) { give_weap(&IDF_FIREBALL); }
   	if (base == &mb_crysillian) { give_weap(&IDF_POWERCORE); }
   	if (base == &mb_meeker)  { give_weap(&IDF_MEEKERBLAST); }
        if (base == &mb_pyroshroom) give_weap(&IDF_PYROSPORE);
        if (base == &mb_poisonshroom) give_weap(&IDF_POISONSPORE);
        if (base == &mb_psychoshroom) give_weap(&IDF_PSYCHOSPORE);
	if (mutate)  return;
   	if (base == &mb_raider) {
                if (irand(4) == 0) { give_weap(&IDF_MONEY); armed()->count = 10 + irand(100); }
          	give_weap(
          		r < 80 ? NULL :
           		r < 86 ? &IDF_SUPERSTIM :
           		r < 90 ? &IDF_MORODULB : &IDF_NANOBOTS
           	);
      		give_weap((irand(1000) - hud->karma < -200 ? &IDF_MORNINGSTAR : &IDF_RUMBLER) );
   	}
   	if (base == &mb_raidercamp) {
                for (int c = irand(10); c; c--) give_weap(&IDF_SUPERSTIM);
           	for (int c = irand(8); c; c--)	give_weap(&IDF_MORODULB);
           	for (int c = irand(4); c; c--)	give_weap(&IDF_NANOBOTS);
           	for (int c = irand(4); c; c--)	give_weap(&IDF_MARJACOLA);
           	give_weap(&IDF_MONEY); 	armed()->count = 1000 + irand(5000);
        }
        /*
        NosaJimiki:  I think public building should drop relivent carg when destroyed:
        BFC>food products,
        J-mart>anything jmart sells,
        arenas>any claws,launchers, ropes, or cages, etc
        */
   	if (base == &mb_raidship) { give_weap(&IDF_PULSAR); }
   	if (base == &mb_hunter) {
                if (irand(4) == 0) { give_weap(&IDF_MONEY); armed()->count = 10 + irand(500); }
                give_weap(r < 80 ? NULL : r < 90 ? &IDF_DETOX : &IDF_NANOBOTS );
      		give_weap(&IDF_LANCER);
   	}
   	if (base == &mb_netship) { if (arg_cheater) give_weap(&IDF_LANCER); else give_weap(&IDF_RATCHET); give_weap(&IDF_EZSLEEPDARTS); }
   	if (base == &mb_merchant) { give_weap(&IDF_RAILGUN); }
   	if (base == &mb_trainer) { give_weap(&IDF_STORMHAND); }
   	if (base == &mb_crashsite || base == &mb_crysillianruins) {
   		for (int c = irand(6); c; c--) give_weap(&IDF_BIOCITE);
	}
        if (base == &mb_hoveler ) {
          give_weap(r < 60 ? NULL :
                   r < 88 ? &IDF_EZSLEEPDARTS :
                   r < 97 ? &IDF_TRANKQUILDARTS: &IDF_PACIFYERDARTS
           );
        }
        if (base == &mb_cheech ) {
          int r = irand(100);
          give_weap(r < 60 ? NULL :
                   r < 80 ? &IDF_SUPERSTIM :
                   r < 97 ? &IDF_MORODULB : &IDF_PROJECT17
           );
        }
		if (base == &mb_worker && ((aiu1 >> 24) == 'S')) {
			if (worker_skill <= 3) give_weap(&IDF_RATCHET);  // 85% suck.
			else if (worker_skill == 4) give_weap(&IDF_RUMBLER); // elites are 10% of applicants
			else switch (irand(10)) { // master elites are 5% of your applicants
			case 0:
			case 1:
			case 2:
			case 3: give_weap(&IDF_RUMBLER); break;
            case 4: 
            case 5: give_weap(&IDF_MORNINGSTAR);break;
            case 6:
            case 7: give_weap(&IDF_LANCER);break;
            case 8: give_weap(&IDF_RAILGUN);break;
            case 9: give_weap(&IDF_STORMHAND);break;            
			}
		}
		
        if (base == &mb_orick) {
        	int civ_lev = MID(0, ms_count / (45 * 60000), 7);
        	static struct { int spr, rch, rum, mor, lan, rai, stm, sun; } civ_lr[8] = {
                              {      10,   1,   0,   0,   0,   0,   0,   0  }, // 0 11 : 1
                              {      20,   5,   1,   0,   0,   0,   0,   0  }, // 1 26 : 6
                              {      30,  10,   5,   2,   2,   0,   0,   0  }, // 2 49 : 19
                              {      40,  20,  10,   4,  10,   1,   0,   0  }, // 3 85 : 45
                              {      23,  25,  20,  10,  15,   5,   1,   1  }, // 4 100: 77
                              {      16,  13,  28,  12,  18,   7,   4,   2  }, // 5 100: 84
                              {       9,  10,  30,  16,  10,  15,   8,   2  }, // 6 100: 91
                              {       0,   5,  15,  30,   5,  30,  10,   5  }, // 7 100:100  5.25 hrs
                };
                int u = 0;
                if      (r < (u += civ_lr[civ_lev].spr)) give_weap(&IDF_SPEAR);
                else if (r < (u += civ_lr[civ_lev].rch)) give_weap(&IDF_RATCHET);
                else if (r < (u += civ_lr[civ_lev].rum)) give_weap(&IDF_RUMBLER);
                else if (r < (u += civ_lr[civ_lev].mor)) give_weap(&IDF_MORNINGSTAR);
                else if (r < (u += civ_lr[civ_lev].lan)) give_weap(&IDF_LANCER);
                else if (r < (u += civ_lr[civ_lev].rai)) give_weap(&IDF_RAILGUN);
                else if (r < (u += civ_lr[civ_lev].stm)) give_weap(&IDF_STORMHAND);
                else if (r < (u += civ_lr[civ_lev].sun)) give_weap(&IDF_SUNSTRIKER);
        }
        if (base == &mb_guntower) give_weap(&IDF_GUNTURRET);
        if (base == &mb_dgtower) give_weap(&IDF_DGTURRET);
        if (base == &mb_xcannontower) give_weap(&IDF_XCANNONTURRET);
        if (base == &mb_galtixtower) give_weap(&IDF_GALTIXTURRET);

        if (base == &mb_raiderguntower) give_weap(&IDF_GUNTURRETRAID);
        if (base == &mb_raiderdgtower) give_weap(&IDF_DGTURRETRAID);
        if (base == &mb_raiderxcannontower) give_weap(&IDF_XCANNONTURRETRAID);
        if (base == &mb_raidergaltixtower) give_weap(&IDF_GALTIXTURRETRAID);
        if (base == &mb_jmart) for (int c = 0; c < 10; c++) {
           int i = irand(NUM_IVALU_LIST);
           if (ivalu_list[i].jm) give_weap(&ivalu_list[i]);
        }

   	if (base == &mb_squadship) give_weap(&IDF_SQUADBLASTER);
   	if (base == &mb_watcher) give_weap(&IDF_WATCHERDG);
        if (base == &mb_enforcer) {give_weap(&IDF_ENFORCERXCANNON); give_weap(&IDF_ENFORCERRPG);}
        if (base == &mb_sentry) give_weap(&IDF_LANCER);
        if (base == &mb_sentinel) give_weap(&IDF_STORMHAND);


        if (base == &mb_ufo) give_weap(&IDF_DEATHRAY);

        // if (base == &mb_merchant || base == &mb_trainer ) give_weap(&IDF_MORNINGSTAR);
       
}
