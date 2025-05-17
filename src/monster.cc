#include "mmam.h"
#include "rand.h"
#include "bubbles.h"
#include "namelist.h"

////////////////////////////////// monst_t //////////////////////////////////////////////
static int exact_func(int i) {return i;}
char *fem_first[]  = {(char *)"Becky", (char *)"Robin", (char *)"Lisa", (char *)"Jenifer", (char *)"Tara", (char *)"Sarah", (char *)"Laura", (char *)"Amy"};
char *male_first[] = {(char *)"Eric", (char *)"Bryan", (char *)"Jason", (char *)"Nathan", (char *)"Shawn", (char *)"Greg", (char *)"Chris", (char *)"Marcus"};
char *last_names[] = {(char *)"Killeen", (char *)"Waldo", (char *)"Gulley", (char *)"Marchel", (char *)"Reinhardt", (char *)"Bradford", (char *)"Branighan", (char *)"Andrews"};

name_gen name_generator;
            
monst_t::monst_t(monst_base *m, int grown) {
	int (*warp_num)(int) = (m->flags & AF_INORGANIC  ? exact_func : deviate);
	sex = (m->flags & AF_ASEXUAL ? 0 : 1 + irand(2));
	if (m == &mb_worker)  {
		char scrap[256], fn[256];
		sprintf(fn,"%s/custom/humannames.conf", home_dir);
		name_generator.load(fn); // load returns true immediately once data is loaded 
		sprintf(fn,"%s/baseconf.dat#HUMANNAMES_CONF", data_dir);
		if (name_generator.load(fn)) {
			name_generator.get_fullname(scrap);
			sex = name_generator.get_last_gender();
		} else { // fallback name gen.
			if (sex == 2) {
				snprintf(scrap, sizeof scrap, "%s %s", fem_first[irand(8)], last_names[irand(8)]);
			} else {
				snprintf(scrap, sizeof scrap, "%s %s", male_first[irand(8)], last_names[irand(8)]);
			}
		}
		name = strdup(scrap);
	} else {
		name = strdup(m->name);
	}
	flags  = m->flags;
	base = m;
	ai_func = m->ai_func;
	x = y = ax = ay = vx = vy  = 0.;
	ang = ( ai_func == ai_plant || ai_func == ai_building || ai_func == ai_item ? M_PI * 0.5 :
	ai_func == ai_player ? 0. : 0. );
	my_ast = NULL;
	action_idx = action_frame = vis_count = 0;
	krulasite_inf = hunger_level = 0;
	drugged = traveled = 0.;
	frame_ms = attack_time = ms_count;
	momma = daddy = spouse = baby = NULL;
	time_bark = ms_count + (base->soundtab ? irand(abs(base->soundtab[snd_bark_freq])) : 0);
	int life_mature = warp_num(m->mature_age);
	int life_span = warp_num(m->max_age);
	if (life_mature < life_span - 5000) life_span = life_mature + 5000;
	int age = (base->org_idx == org_crysillians ? 2000 * 60000 + ms_count :
		grown ?  (int)(0.8 * frand() * life_span) : 0
	);
	if (flags & AF_INORGANIC || m->mature_age == 0) {
		growth_rate = 0;
		size = m->size;
	} else {
		int full_size = warp_num(m->size);
		int born_size = full_size / 10;
		growth_rate = (full_size - born_size) * 1000. / MAX(life_mature,1);
		size = (int)(born_size + (MIN(age, life_mature) / 1000.) * growth_rate);
	}
	fix_radius();
	time_born = ms_count - age;
	time_die = (m->max_age ? life_span + time_born : 0);
	time_mature = (m->mature_age ? life_mature + time_born : time_born);
	printf((char *)"spawn age(%i): %0.2f of %0.2f\n", grown, age / 60000., life_span / 60000.);
	time_done_pregnant = 0;
	mhp = hp = warp_num(m->mhp);
	spore_pyro_count = spore_psycho_count = spore_poison_count = 0;
	speed  = warp_num(m->speed) / (float)(TURNS_PER_SEC);
	lat_speed  = warp_num(m->lat_speed) / (float)(TURNS_PER_SEC);
	turn_rate = warp_num(m->turn_rate) * (M_PI/180.) / (float)(TURNS_PER_SEC);
	str  = warp_num(m->str);
	attack_rate = warp_num(m->attack_rate);
	stomach = warp_num(m->stomach);
	in_stomach = stomach * 0.8;
	met_rate  = warp_num(m->met_rate);
	nutrish = max_nutrish = warp_num(m->nutrish);
	energy = max_energy = MAX(warp_num(m->max_energy), 200);
	od_thresh  = warp_num(m->od_thresh);
	gestation  = warp_num(m->gestation);
	agression  = warp_num(m->agression);
	fear_dist = warp_num(m->fear_dist);
	extra_hates = NULL;
	extra_scared_of = NULL;
	who_hit_me_last = see_monst = see_buddy = see_hole = home_hole = hole_monst = chase_last = NULL;
	ast_from = -1;
	jump_out_time = 0;
	speed_bonus = energy_bonus = str_bonus = met_rate_bonus = 0;
	healing_bonus = agression_bonus = invisability_bonus = 0;
	is_starving = 0;
	growth_bonus = restlessness = drugged = 0;
	fame_bonus = on_fire = 0;
	on = 1;   pow_on = 1;
	pow_drain = 0;
	target_ang[0] = target_ang[1] = target_ang[2] = target_ang[3] = 0;
	aiu1 = aiu2 = aiu3 = 0;  // usually: anthropy, timer, misc
	name_xp = name_xp_last = 0;
	if (base->inv_link) switch (base->inv_link->typ) {
	case inv_tower :
	case inv_scanner :
	case inv_stabilizer :
	case inv_encl : pow_drain = base->inv_link->tox; break;
	case inv_power : pow_drain = -base->inv_link->tox; break;
	default:;
	}
	org_idx = base->org_idx;
	mutate_time = revert_time = 0; mutate_to = NULL; mutate_growth = 0.;
	trader_ui = new trader_t(0, this);
	if (!trader_ui->link) {
		delete trader_ui; trader_ui = NULL;
	}
	building_squads = (
		base == &mb_raidercamp || base == &mb_policestation || base == &mb_jmart ||
		base == &mb_bfc || base == &mb_arena || base == &mb_orickcamp ? 1 : 0
	);
	encl = NULL;
	if (base->inv_link && base->inv_link->typ == inv_encl) {
		radius = 180;
		size = (int)((radius / 2 - 5) * (radius / 2 - 5));
		encl = new asteroid_t(0, 1, this);
	}
	if (base == &mb_hole) encl = new asteroid_t(0, 1, this); // use it for m list	
	smacked.clear();
	mask.mask = NULL;
	mask.src = NULL;
	mark_pmask(&mask, base->spr_chill ? (BITMAP *)sprites[base->spr_chill[0].idx].dat : NULL, ang - (M_PI / 2.), (int)(radius * 2));
	weap_rounds = (base->inv_link ? base->inv_link->grp : 0);
	for (int c =0; c < NUM_AWARD_TYPES; c++) award[c] = 0;
	
	worker_history = worker_rapsheet = worker_lastboss = NULL;
    if (m == &mb_worker) {
        char scrap[8000]; char *so = scrap;
        char wclasslist[] = "SSSSSMMMMMMKKPRRVVVCCCCCC";
    	char wclass = wclasslist[irand(strlen(wclasslist))];
    	int iwclass = wclass;
		static int worker_skill_dist[20] = {
    	  1,1,1,1,1, 1,2,2,2,2,
    	  2,2,3,3,3, 3,3,4,4,5
    	};
    	static int worker_salary_table[27] = {
    	  450,500,500,
    	  500,525,550,
    	  575,600,625,
    	  650,700,750,
    	  800,850,850,
    	  850,900,925,
    	  950,1000,1000,
    	  1000,1100,1200,
    	  1200,1300,1300    	  
    	};
		aiu1 = (irand(0xffffff) & 0xffffff) | (iwclass << 24); // build qwerks. 
		int security_clean_mask = 0xffffffff ^ (QWERK_PACIFIST);	
    	int pacifist_clean_mask = 0xffffffff ^ (QWERK_VIOLENT | QWERK_VENGEFUL);	
    	int lawful_clean_mask   = 0xffffffff ^ (QWERK_POLITICAL | QWERK_DRUGS);
		if ((aiu1 >> 24) == 'S') aiu1 &= security_clean_mask;
		if (aiu1 & QWERK_PACIFIST) aiu1 &= pacifist_clean_mask;
		if (aiu1 & QWERK_LAWFUL) aiu1 &= lawful_clean_mask;
		if (irand(20) == 0) flags |= AF_PARANOID;
		worker_skill = worker_skill_dist[irand(20)];
    	int job_arrogance = (
    		wclass == 'S' ? 2 : 
    		wclass == 'V' ? 5 : 
    		wclass == 'R' ? 7 :
    		wclass == 'K' ? 5 :
    		wclass == 'P' ? 6 : 0
    	);
    	// possable skill penaltys for drugs and lazyness 
		if (worker_skill > 1 && (aiu1 & QWERK_DRUGS) && irand(3) == 0) worker_skill--;
		if (worker_skill > 1 && ((aiu1 & 0x7) == 0)) worker_skill--;
		if (worker_skill > 1 && ((aiu1 & 0x50) == 0) && irand(3) == 0) worker_skill--;
		worker_salary = worker_salary_table[MID(0,job_arrogance + worker_skill * 3 + irand(7) - 3,26)];
    	int sharpshooter_skill_table[6] = {0, 17, 8, 3, 2, 1 };
    	int sec_extra_hp_table[6] = {0, 10, 16, 21, 26, 30 }; 
    	if (!(aiu1 & QWERK_PACIFIST) && 
    		irand(wclass != 'S' ? 20 : MID(1,sharpshooter_skill_table[worker_skill],5) ) == 0
    	) flags |= AF_ADVTARGET;
		mhp += (wclass != 'S' ? 0 : sec_extra_hp_table[worker_skill] );
		hp = mhp;
		char *fun_time_critters[6] = {
		  (char *)"molber",(char *)"rexrex",(char *)"mubark",
		  (char *)"furbit",(char *)"vaerix",(char *)"hoveler"
		};
		char *drug_mischief[6] = {
		  (char *)"peeing on a molber",(char *)"peeing in a mailbox",(char *)"peeing on a mubark",
		  (char *)"public urination",(char *)"impersonating a vaerix",(char *)"public drunkenes"
		   
		};
		char *political_pacifist_havoc[6]  = {
		  (char *)"burning draft cards",(char *)"refusal to appear in court",(char *)"streaking",
		  (char *)"streaking",(char *)"streaking",(char *)"streaking"
		};
        char *political_violent_havoc[6]  = {
		  (char *)"burning down a postoffice",(char *)"blowing up an IRS building",(char *)"assualting the mayor",
		  (char *)"burning down city hall",(char *)"interfeering with an election",(char *)"destroying court documents"
		};
        char *unlawful_violent_havoc[6]  = {
		  (char *)"armed robbery",(char *)"assault",(char *)"simple battery",
		  (char *)"assaulting a police officer",(char *)"battery",(char *)"agrivated assault"
		};
        char *bullshit_charges[12] = {
        (char *)"recived a ticket for speeding in a school zone",
		(char *)"recived a ticket for failure to yeild",
		(char *)"recived a ticket for an illegal U-turn",
		(char *)"recived a ticket for failure to come to a complete stop at a stop sign",
		(char *)"recieved a ticket for running a red light", 
		(char *)"recieved a citation for an expired licence", 
		(char *)"was arrested for reckless driving", 
		(char *)"recieved a citation for an illegal left turn", 
        (char *)"recieved a citation for an illegal right turn", 
        (char *)"recieved a citation for an illegal lane change", 
        (char *)"recieved a citation for impeding trafic", 
        (char *)"recieved a citation for littering"
        };
		char *dangerous_drugs[6] = {
		(char *) "rozolin",(char *)"glowing green liquid",(char *)"JR-19AZF", 
		(char *)"morwdolb",(char *)"project 17",(char *)"IHB-77"
		};
		char *drug_actions[6] = {
		(char *)"for the possession of",(char *)"for the possession of",(char *)"for the possession of", 
		(char *)"for the possession of with intent to distribute",
		(char *)"for distributing", (char *)"while high on"
		};
		char *months[12] = {
		(char *) "Jan",(char *)"Feb",(char *)"Mar", 
		(char *)"Apr",(char *)"May",(char *)"Jun",
		(char *) "Jul",(char *)"Aug",(char *)"Sep", 
		(char *)"Oct",(char *)"Nov",(char *)"Dec"
		};
		
		if (irand(3)) gestation = 0;
        else if (irand(3) == 0) flags ^= AF_FAMILY;
        
        int cheep_date = 0;  
        for (int c = 0; c < 6; c++) { 
        	char prec[256], while_high_on[256];
        	cheep_date += irand(12 * 28 * 3) + 90;
        	int cheep_yr = 2405 + cheep_date / (12 * 28);
        	int cheep_mo = (cheep_date / 28) % 12;
        	int cheep_day = (cheep_date % 28) + 1;
        	int speed = 3 + irand(70);
        	snprintf(prec, sizeof prec, "On %s %i, %i %s",months[cheep_mo], cheep_day, cheep_yr, name);
        	if (irand(4) == 0 && (aiu1 & QWERK_DRUGS)) {
        		snprintf(while_high_on, sizeof while_high_on, " while high on %s", dangerous_drugs[irand(6)]);  
        	} else while_high_on[0] = 0;
        	switch (irand(27)) {
			case 0: 
			case 1: if (aiu1 & QWERK_DRUGS) so += sprintf(so, "%s was arrested %s %s.  ", prec, drug_actions[irand(6)], dangerous_drugs[irand(6)]); cheep_date += 1000; break;
			case 2: if (aiu1 & QWERK_DRUGS) so += sprintf(so, "%s recived a citation for %s.  ", prec, drug_mischief[irand(6)]); break;
			case 3: if (aiu1 & QWERK_VIOLENT) so += sprintf(so, "%s was arrested for an inproperly registered firearm.  ", prec); break;
			case 4: 
			case 5: if ((aiu1 & QWERK_VIOLENT) && !(aiu1 & QWERK_LAWFUL)) so += sprintf(so,"%s was arrested for %s%s.  ", prec, unlawful_violent_havoc[irand(6)], while_high_on); cheep_date += 1000; break;
			case 6: if (aiu1 & QWERK_VENGEFUL && irand(3)==0) so += sprintf(so, "%s was arrested for arson%s.  ", prec, while_high_on); cheep_date += 1000; break;
			case 7: if (aiu1 & QWERK_POLITICAL) so += sprintf(so, "%s was arrested for disorderly conduct%s.  ", prec, while_high_on); break;
			
			case 8: if (irand(3)) so += sprintf(so, "%s was arrested for back child support.  ", prec); break;
			
			case 9: if (!(aiu1 & QWERK_LAWFUL)) so += sprintf(so, "%s was arrested for an inappropriate relationship with a %s%s.  ", prec, fun_time_critters[irand(6)], while_high_on); cheep_date += 1000; break;
			case 10:
			case 11:
			case 12: if (!(aiu1 & QWERK_LAWFUL) || irand(5)==0) so += sprintf(so, "%s %s.  ", prec, bullshit_charges[irand(12)]); break;
			case 13: 
			case 14: if (!(aiu1 & QWERK_LAWFUL) || irand(5)==0)  so += sprintf(so, "%s  %s for driving %imph over the speed limit.  ", prec, (speed > 30 ? "was arrested" : "recieved a ticket"),speed); break;
			case 15: if (irand(5) == 0) so += sprintf(so, "%s recieved a citation for being black.  ", prec); break;
			case 16: if ((aiu1 & QWERK_PACIFIST)&&(aiu1 & QWERK_POLITICAL)) so += sprintf(so, "%s was arrested for %s.  ", prec, political_pacifist_havoc[irand(6)]); cheep_date += 1000; break;
			case 17: if ((aiu1 & QWERK_VIOLENT)&&(aiu1 & QWERK_POLITICAL)) so += sprintf(so, "%s was arrested for %s.  ", prec, political_violent_havoc[irand(6)]); cheep_date += 1000; break;
			case 18: if (gestation && irand(3)) so += sprintf(so, "%s was arrested for lewd conduct.  ", prec); break;
			case 19: if (gestation && !(flags & AF_FAMILY) && irand(3)) so += sprintf(so, "%s was arrested for %s.  ", prec, sex == 2 ? "selling it on the street corner" : "soliciting an undercover officer"); break;
			}
        }
    	worker_rapsheet = so == scrap ? NULL : strdup(scrap);
		worker_lastboss = strdup("He's extremely lazy.");
    	so = scrap; *so = 0;
		int show_skill = MID(1,worker_skill - irand(1),5);
		int odd_jobs = irand(3);
		static char *odd_jobs_tab[12] = {
			(char *)"Buddy's Fried Chicken", (char *)"J-Mart", (char *)"Bob's Shuttle Wash", 
			(char *)"Ike's Coin Laundry",(char *)"Max Supply",(char *)"Things-N-Stuff",
			(char *)"Bath Bayond",	(char *)"Big Nate's Used Robots", (char *)"Crazy Al's Shuttles", 
			(char *)"Killeen Corp", (char *)"Pet's Pallace", (char *)"Pointy Pencel Company" 
		};
		static char *odd_times[12] = {
			(char *)"a month", (char *)"3 months", (char *)"6 months", 
			(char *)"1 year",(char *)"2 years",(char *)"3 years"
		};
		if (show_skill == 1) {
			so += sprintf(so,"%s is a student at %s.  %s is applying for a job as a %s.  ",
				name, ((wclass == 'M' || wclass == 'C') && irand(2) ? "Asteroid HS" : "Astreoid University"), He(), worker_class(2)
			);
			if (odd_jobs == 0) so += sprintf(so, "This will be %s first job.  ", his());
			else {
				for (int c = 0; c < odd_jobs; c++) {
					so += sprintf(so, "%s used to work at %s for %s.  ", He(), odd_jobs_tab[irand(12)], odd_times[irand(6)]);
				}
			}
		} else  {
			so += sprintf(so, "%s is a graduate from %s.  %s is applying for a job as a %s.  ",
				name, ((wclass == 'M' || wclass == 'C' || wclass == 'S') && irand(2) ? "Asteroid HS" : "Astreoid University"), He(), worker_class(2)
			);
			for (int c = 0; c < odd_jobs; c++) {
					so += sprintf(so, "%s used to work for %s for %s.  ", He(), odd_jobs_tab[irand(12)], odd_times[irand(6)]);
			}
			for (int c = irand(show_skill - 1) + 1; c > 0; c-- )switch (wclass) {
			case 'R': so += sprintf(so, "%s was a manager at %s for %i years.  ",He(), odd_jobs_tab[irand(12)], show_skill + irand(3 * show_skill) ); break;
			case 'P': so += sprintf(so, "%s was in charge of public relations at %s for %i years.  ",He(), odd_jobs_tab[irand(12)], show_skill + irand(3 * show_skill) ); break;
			case 'S': so += sprintf(so, "%s spent %i years in the %s.  ",He(), show_skill + irand(3 * show_skill) , irand(2) ? "Asteroid Police Department" : "milatary");break;
			case 'C': so += sprintf(so, "%s spent %i years working %s.  ",He(), show_skill + irand(3 * show_skill) , irand(2) ? "for the zoo" : "as a vet assistant");break;
			case 'V': so += sprintf(so, "%s spent %i years working %s.  ",He(), show_skill + irand(3 * show_skill) , irand(2) ? "for the zoo" : "in private practice as a vet");break;
			case 'M': so += sprintf(so, "%s spent %i years working %s for %s.  ", He(), show_skill + irand(3 * show_skill) , irand(2) ? "as a handyman" : "jack of all trades", irand(2) ? "a hab dome complex" : "an office tower" );break;
			case 'K': so += sprintf(so, "%s spent %i years %s.  ", He(), show_skill + irand(3 * show_skill) , irand(2) ? "as an army medic": "in private practice as a doctor");break;
			
			}
			
		}
		worker_history = strdup(scrap);			
	}
	reset_horny_time();
	issue_weaps(0);	
}

char *monst_t::worker_class(int ver) {
	// 0 - normal   1 - short   2 - full title
	char *class_str = (char *)"???";
	switch (aiu1 >> 24) {
    		case 'S': class_str = (char *)(ver == 2 ? "Security Guard" : "Security");  break;
			case 'M': class_str = (char *)(
							ver == 1 ? "Maint." : ver == 2 ? "Maintance" : sex == 1 ?
							"Maintance Man" : "Maintance Worker");    break;
			case 'K': class_str = (char *)"Medic";     break;
			case 'P': class_str = (char *)(ver == 1 ? "P. R." : ver == 0 ? "Public Relations" : "Public Relations Specialist");     break;
			case 'R': class_str = (char *)"Manager";   break;
			case 'V': class_str = (char *)(ver == 0 ? "Vet." : "Veterinarian");      break;
			case 'C': class_str = (char *)(ver == 2 ? "Monster Caretaker" : "Caretaker"); break;
	}
	return class_str;
}

void monst_t::mutate(monst_base *m, int time) {
        if (m->flags & AF_INORGANIC) return;
        if (my_ast) my_ast->add_jumpgate(x, y, SPR_MUTATE);
        int (*warp_num)(int) = deviate;
        if (m == base) hud->long_news((char *)"The %s looks somehow changed.",name);
        else hud->long_news((char *)"The %s is mutating into a %s.",name,m->name);
        if (strcmp(name, base->name) == 0) {
        	free(name);
        	name = strdup(m->name);
        }
        flags  = m->flags;
        base = m;
        sex = (flags & AF_ASEXUAL ? 0 : 1 + irand(2));
        time_done_pregnant = 0; // trans species pregnancys don't work 
        ai_func = m->ai_func;
        mhp = hp = warp_num(m->mhp);
        speed  = warp_num(m->speed) / (float)(TURNS_PER_SEC);
        lat_speed  = warp_num(m->lat_speed) / (float)(TURNS_PER_SEC);
        turn_rate = warp_num(m->turn_rate) * (M_PI/180.) / (float)(TURNS_PER_SEC);
        str  = warp_num(m->str);
        attack_rate = warp_num(m->attack_rate);
        stomach = warp_num(m->stomach);
        size = warp_num(m->size);
        fix_radius();
        met_rate  = warp_num(m->met_rate);
        nutrish = max_nutrish = warp_num(m->nutrish);
        energy = max_energy = warp_num(m->max_energy);
        od_thresh = warp_num(m->od_thresh);
        gestation = warp_num(m->gestation);
        agression = warp_num(m->agression);
        fear_dist = warp_num(m->fear_dist);
        action_frame = 0;
}

void monst_t::building_upgrade() {
	if (!base->inv_link) return;
	ivalu_t *upg_to = base->inv_link;
	upg_to++;
	if (upg_to->typ != base->inv_link->typ) return;
        monst_base *m = upg_to->mb_link;
        int (*warp_num)(int) = exact_func;
        if (strcmp(name, base->name) == 0) {
        	free(name);
        	name = strdup(m->name);
        }
        flags  = m->flags;
        base = m;
        ai_func = m->ai_func;
        mhp = hp = warp_num(m->mhp);
        speed  = warp_num(m->speed) / (float)(TURNS_PER_SEC);
        lat_speed  = warp_num(m->lat_speed) / (float)(TURNS_PER_SEC);
        turn_rate = warp_num(m->turn_rate) * (M_PI/180.) / (float)(TURNS_PER_SEC);
        str  = warp_num(m->str);
        attack_rate = warp_num(m->attack_rate);
        stomach = warp_num(m->stomach);
        size = warp_num(m->size);
        radius = (int)(sqrt(size)/2 + 5);
        met_rate  = warp_num(m->met_rate);
        nutrish = max_nutrish = warp_num(m->nutrish);
        energy = max_energy = warp_num(m->max_energy);
        od_thresh  = warp_num(m->od_thresh);
        gestation  = warp_num(m->gestation);
        time_done_pregnant = 0;
        agression  = warp_num(m->agression);
        fear_dist = warp_num(m->fear_dist);
	for (list<weap_t *>::iterator wp = weap.begin(); wp != weap.end(); ) {
                delete (*wp);
        	wp = weap.erase(wp);
   	}
	issue_weaps(0);
}



monst_t::~monst_t() {
	free(name);
	if (base == &mb_golgithan && alive()) {
		event.set(event_golgithan, 60 * (60 + irand(20))); // there is only one golgathan
	}
	if (trader_ui) {
		trader_ui->link = NULL;
		if (trader_ui == trader) {
			trader_ui->scroll_out();
		} else delete trader_ui;
	}
	delete encl;
	smacked.clear();
	if (hud) hud->forget(this);
	if (my_ast) my_ast->_forget_monst(this);
	if (trader) trader->forget(this);
}

void monst_t::sqwak(int noise_idx) {
	if (noise_idx < 0 || !base->soundtab || (base->soundtab[noise_idx] <= 0)) return;
	if (noise_idx == snd_bark) {
   		if (ms_count < time_bark) return;
		do_sound(base->soundtab[noise_idx], (int)(x), (int)(y), 255);
        time_bark = ms_count + base->soundtab[snd_bark_freq];
   	} else {
		do_sound(base->soundtab[noise_idx], (int)(x), (int)(y), 255);
   	}
}

void monst_t::feel(emotion e) {
        want_emote = e;
}


int monst_t::drop_weap(int force) {
	weap_t *w = armed();
	if (w) {
        	if (w->t->wtyp & WP_INTERNAL && !force) return 0;
        	if ( w->t->mb_link && ((w->t->wtyp & WP_INTERNAL) == 0 || w->t->typ != inv_proj) && my_ast) {
           		monst_t *gw = new monst_t(w->t->mb_link);
           		gw->weap_rounds = w->count;
                	my_ast->add_monst_by(gw,x,y);
           	}
           	weap.erase(weap.begin());
                delete w;
                return 1;
   	}
   	return 0;
}

int monst_t::take_weap(monst_t *m, builder_table *bt) {
	if (!m || !m->my_ast || dist(x, y, m->x, m->y) > radius + m->radius) return 0;
	if (bt) for (int c = 0; bt[c].fr; c++) if (bt[c].fr == m->base) {
		printf((char *)"%s converts %s to %s.\n", name, bt[c].fr->name, bt[c].to->name);
		ivalu_t *itm = bt[c].to->inv_link;
		int count = (itm ? itm->grp : 0);
		do_act(ACT_BUILD);
		attack_time = ms_count + bt[c].time;
		give_weap(itm, count);
		m->my_ast->unhook_monst(m);
		delete m;
		return 1;
	}
	if (m->my_ast && m->base->inv_link && m->alive()) {
		int count = m->weap_rounds;
		ivalu_t *itm = m->base->inv_link;
		printf((char *)"%s picks up %i %s.\n", name, count, itm->nam);
		if (this == ship && m->base->inv_link->typ == inv_money) {
			hud->money += count;
		} else give_weap(itm, count);
		m->my_ast->unhook_monst(m);
		delete m;
		return 1;
   	}
   	return 0;
}


void monst_t::do_act(int action) {
  	if (action_idx == ACT_DIE) return;
	if (
		(action == ACT_WALK  || action == ACT_CHILL) &&
		(action_idx == ACT_EAT || action_idx == ACT_ATTACK || action_idx == ACT_BUILD) &&
		(attack_time > ms_count)
	) return;
	if (action_idx == action) return;
	action_idx = action;
	action_frame = 0;
	aspath *ap = (
		action_idx == ACT_CHILL  ? base->spr_chill :
		action_idx == ACT_WALK   ? base->spr_walk :
		action_idx == ACT_EAT    ? base->spr_eat :
		action_idx == ACT_ATTACK || action_idx == ACT_BUILD ? base->spr_attack :
		action_idx == ACT_DIE || action_idx == ACT_KO   ? base->spr_die : NULL
	);
	if (action_idx == ACT_DIE || action_idx == ACT_KO ) while (drop_weap(1));
	frame_ms = (ap ? ms_count + ap[0].ms : ms_count + 100);
}

int monst_t::stress(float stress_factor) {
	energy -= (stress_factor / size) * 0.05;
	if (energy < 0.) energy = 0.;
	return tired();
}


int monst_t::hit_ex(int dam, monst_t *who, int die_noise, int pain_noise) {
	if (this == ship && sackless_mode) return 0;
	if (dead()) return 1;
	if (this == ship) safty_pause();
	hp -=dam;
	if (who) {
		if (base != who->base) who_hit_me_last = who;
		if (who->org_idx == org_ast0) alignment.hit(this, dam);
		if (who->base == &mb_slymer && od_thresh) {
			int tox = 70; int drg = 30;
			drugged += tox;
			met_rate_bonus -= 1000. * drg / od_thresh;
		}
		if (who->base == &mb_drexler && od_thresh) {
			int tox = 12; int drg = 6; // drexlers stings are mildly toxic
			drugged += tox;
			met_rate_bonus -= 1000. * drg / od_thresh;
		}
	}
	if (this == ship && trader) trader->scroll_out();
	if (hp <= 0) {
		hp = 0;
		decay_time = ms_count + (this == ship ? 4000 : 20000);
		if (my_ast) {
			if (this == ship && base == &mb_netship) {
				my_ast->add_monst(thehuman); thehuman->x = x; thehuman->y = y;
				float rang = frand() * 2. * M_PI;
				thehuman->vx = vx + cos(rang) * (600. / TURNS_PER_SEC);
				thehuman->vy = vy + sin(rang) * (600. / TURNS_PER_SEC);
				ship = thehuman;
			}
			if (
				((base == &mb_yazzert || base == &mb_vaerix) && irand(5) == 0) ||
				((base == &mb_psychoshroom || base == &mb_poisonshroom) && 
				(ms_count > time_mature && !on_fire) ) ||
				((base == &mb_pyroshroom) && (ms_count > time_mature && !drugged) )

			) {
				decay_time = ms_count + 500;
				weap_t *w = armed();
				if (w) for (int c = 0; c < 60; c++) my_ast->add_bullet(this, frand() * 2. * M_PI, w->t);
			}
			if (base == &mb_pyroshroom && my_ast) my_ast->explode(x,y, 150, 60, NULL, this, WP_FIRE);
			int xdam = 0;
			int xrad = 350;
			if (base->inv_link) {
				ivalu_t *t = base->inv_link;
				switch (t->typ) {
				case inv_proj :
					if (t->wtyp & WP_EXPL && t->drg && my_ast) {
						decay_time = ms_count + 500;
						xrad = t->drg; xdam = t->sv;
					} else if (irand(3) == 0) {
						decay_time = ms_count + 500;
						int rnds = MIN(irand(60), irand(weap_rounds));
						for (int c = 0; c < rnds; c++){
							my_ast->add_bullet(this, frand() * 2. * M_PI, t);
						}
					}
					break;
				case inv_power:	xdam = t->sv; break;
				default:;
				} // switch
			} else if (flags & AF_BOMB) {
				xdam = 4500;
			}
			if (xdam) my_ast->explode(x,y,xrad,xdam,who,this);
			if (xdam) hud->flash_time = ms_count + 1200;
		}
		if (krulasite_inf) {
			if (my_ast) {
				// if host monster dies new krulasites jump out
				// only 1/2 Krulasites if not ready to hatch.
				int n = (size < mb_shruver.size     ? 2 :
					size < mb_throbid.size     ? 3 :
					size < mb_mamothquito.size ? 4 : 5
				);
				if (krulasite_inf < ms_count) n /= 2;
				for (int c = 0; c < n; c++ ) {
					monst_t *nmonst = new monst_t(&mb_krulasite);
					my_ast->add_monst_by(nmonst, this->x, this->y);
				}
			}
			krulasite_inf = 0;
		}
		int t_spore = spore_pyro_count + spore_psycho_count + spore_poison_count;
		if (my_ast && (t_spore > 20 || t_spore && irand(4) == 0) ) {
			monst_t *n_shroom = new monst_t(
				spore_pyro_count > spore_psycho_count ?
				(spore_pyro_count > spore_poison_count ? &mb_pyroshroom : &mb_poisonshroom) :
				(spore_psycho_count > spore_poison_count ? &mb_psychoshroom : &mb_poisonshroom), 0
			);
			my_ast->add_monst(n_shroom, x, y);
		}
		do_act(ACT_DIE);
		sqwak(die_noise);
		if (who && who->org_idx == org_ast0) alignment.kill(this);
		if (who) {
			hud->long_news((char *)"%s killed by %s.", name, who->name);
			if (my_ast && my_ast->arena_glyph) who->name_xp++;
		} else  hud->long_news((char *)"%s died.", name);
		if (base->blood_color && my_ast) my_ast->spatter_blood(this, 0., 0., -1);
		if (my_ast) {
			// unhook then add_monst to fix the sort height. 
			asteroid_t *remember = my_ast;
			remember->unhook_monst(this);
			remember->add_monst(this); 
		}
	} else {
		sqwak(pain_noise);
		if (who && who->org_idx == org_ast0) alignment.hit(this, dam);
		if (who) printf((char *)"%s hit by %s for %i hp.\n", name, who->name, dam);
		else  printf((char *)"%s hit for %i hp.\n", name, dam);
		if (base->blood_color && who && my_ast) my_ast->spatter_blood(this, 0., 0., dam);
		if (
			(irand(15) == 0) && isa_shroom() &&
			(ms_count  > time_mature && !on_fire) // maturity test later.
		) {
			weap_t *w = armed();
			if (w) for (int c = 0; c < 8; c++) my_ast->add_bullet(this, frand() * 2. * M_PI, w->t);
		}
		
	}
   	if (this == ship) hud->pin_time = 0;
   	return (hp == 0);
}

int monst_t::hit(int dam, monst_t *who, int die_noise, int pain_noise) {
   	return hit_ex(dam, who, die_noise, pain_noise);
}

int monst_t::hit(int dam, int die_noise, int pain_noise) {
   	return hit_ex(dam, NULL, die_noise, pain_noise);
}


int monst_t::attack(monst_t *who) {
	if (!who || !attack_rate || !str) return 0;
	vx *= 0.5; vy *= 0.5;
	if (base == &mb_krulasite) { printf((char *)"krulasite attack!\n"); }
	if ( ms_count > attack_time && !who->dead()) {
		if (base == &mb_krulasite) {
			printf((char *)"%s is infected!\n", who->name);
			hit(hp);
			if (who->krulasite_inf == 0) who->krulasite_inf = ms_count + 1000 * (180 + irand(330));
		}
		who->hit(irand(str), this);
		do_act(ACT_ATTACK);
		//sqwak(snd_attacking);
		calc_can_attack_next();
	}
	return (who->dead());
}


	

void monst_t::reset_horny_time() {
	int horny_freq = 30000; // 30 secs is normal (males tend to be sluts)
	if (base == &mb_frogert) {
		int frogert_year = 60000 * 22; // 22 minutes
		int frogert_sl = 60000  * 2; // 2 min
		int frogert_season = (ms_count) % frogert_year;
		if (frogert_season < frogert_sl) horny_time = 4000;
		else horny_time = ((ms_count / frogert_year) + 1) * frogert_year;
		return;
	}
	if (ms_count < time_mature) {
		horny_time = time_mature;
		return; 
	}
	if (sex == 2 && time_done_pregnant) horny_time = time_done_pregnant + horny_freq;
	else {
		horny_time = ms_count + horny_freq;
	}
}



int  monst_t::fire_at(monst_t *who) { 
	 if (!who) return 0;
	 float a; int ok = 1;
	 if (flags & AF_ADVTARGET) {
    		ok = shot_lead(who, a);
	 } else a = atan2(who->y - y, who->x - x);
	 return (ok ? fire(a) : 0);
}

int  monst_t::fire(float ang) {
	if (asleep()) return 0;
	wang = ang;
	weap_t *w = armed();
	if (!w) return 0;
	if (ms_count < w->rl_time ) return 0;
	int inj_what = -1, eat_what = -1;
	switch (w->t->typ) {
	case inv_encl :
	case inv_stabilizer :
	case inv_tower :
	case inv_scanner :
	case inv_power : {
       		if (!my_ast ) return 0;
                float r = sqrt(w->t->mb_link->size);
                if (! my_ast->monst_near(input_map.target_x, input_map.target_y, r)) {
					monst_t *m = new monst_t(w->t->mb_link);
					my_ast->add_monst_by(m, input_map.target_x, input_map.target_y);
					w->rl_time = 1500 + ms_count;
					SHIP_NOISE(BUILD);
					ast->add_jumpgate(m->x, m->y, SPR_BUILD);
					input_map.click_ok = ms_count + 300;
                } else {
                	return 0;
                }
        }
        break;
  	case inv_proj: {
       			if (!my_ast ) return 0;
                	my_ast->add_bullet(this, ang, w->t);
                	w->rl_time = w->t->relms + ms_count;
                	w->a_time  = MID(50, w->t->relms - 50, 150) + ms_count;
                	if (w->t->ws1 == SPR_NO_LIL_PIC) {
                   		do_act(ACT_ATTACK);
                        	attack_time = w->a_time;
            		}
            		do_sound(w->t->noise, (int)(x), (int)(y), 255);
        }
        break;
        case inv_injector: printf((char *)"%s injects self with %s.\n",name,w->t->nam); feel(emot_inject); inject_idx = w->t->ws1; inj_what = w->t->sv;  w->rl_time = 18000 + ms_count; break;
        case inv_food: printf((char *)"%s eats a %s.\n",name,w->t->nam); feel(emot_inject); inject_idx = w->t->ws1; eat_what = w->t->sv;  w->rl_time = 18000 + ms_count; break;
        default : return 0;
   	}
   	if ((--w->count) <= 0) {
   		int rlt = w->rl_time;
   		delete w;
   		weap.pop_front();
   		if ( (w = armed()) ) {
                	w->rl_time = rlt;
       		}
	}
	// some injectables cause to drop all items. this makes sure the item is used first.
	if (eat_what >= 0) feed(eat_what);
	if (inj_what >= 0) inject(inj_what);
   	return 1;
}


void monst_t::give_weap(struct ivalu_t *t, int n, int rl) {
    if (!t) return;
  	if (n == -1) n = t->grp;
	for(list<weap_t *>::iterator bl = weap.begin(); bl != weap.end(); bl++) if ((*bl)->t == t) {
   		(*bl)->count += n;
   		return;
	}
    weap_t *w = new weap_t;
    w->t = t;   w->count = n;  w->a_time = w->rl_time = ms_count + rl;
	weap.push_front(w);
}


int monst_t::eat(monst_t *who) {
	if (!who) return 0;
	if (who->alive() && !who->isa_plant() && ((flags & AF_BLOODSUCKER) == 0)) { attack(who); }
	else if ((in_stomach < stomach) && (ms_count > attack_time) && (who->nutrish) ) {
   		vx *= 0.5; vy *= 0.5;
        	int at = 0;
        	for (int c = 0; (base->spr_eat[c].idx != -1); c++ ) at += base->spr_eat[c].ms;
   		int bite_size = MAX(1, stomach / (at < 400 ? 20 : at < 1200 ? 10 : 6) );
        	int take = MID(0, bite_size, who->nutrish);
        	in_stomach += take;
        	who->nutrish -= take;
		if (base == &mb_slymer) {
			size += take; radius = sqrt(size);
			if (my_ast && size > 2 * mb_slymer.size) {
				monst_t *twin = new monst_t(base);
				twin->size = size = size / 4;
				twin->fix_radius();   fix_radius();
				my_ast->add_monst_by(twin, x, y);
			}
		}
		if (who->nutrish <= 0) who->hit(who->hp, -1, -1); // completely eaten
		do_act(ACT_EAT);
		feel(emot_eating);
		sqwak(snd_eat);
		//calc_can_attack_next();
		attack_time = ms_count + MAX(at, 100);
		if (who->dead() && (who->nutrish <= 0)) {
			who->decay_time = ms_count; // decay now!
			eat_affect(who);
		}	
	}
	return (who->nutrish <= 0);
}

static monst_base* humanoids[] = {
	&mb_orick, &mb_cheech,	&mb_hoveler, &mb_raider, &mb_hunter,  NULL
};

int  monst_t::adopt_new_parent(monst_t *parent) {
	if (!momma && !daddy) {
		if (parent->sex == 1) {
			daddy = parent; momma = daddy->spouse;
		} else {
			momma = parent; daddy = momma->spouse;
		}
		char t1[255], t2[255];
		if (daddy) daddy->baby = this;
		if (momma) momma->baby = this;
		if (my_ast != home_ast) {
			//say nothing
		} else if (parent->base != base) {
			hud->long_news((char *)"%s just adopted %s", parent->A_name(t1,1), a_name(t2));
		} else if (parent->spouse) {
			hud->long_news((char *)"%s was just adopted by new parents.", a_name(t1));			
		} else 	hud->long_news((char *)"%s was just adopted by a new %s.", a_name(t1), sex==1 ? "dad" : "mother");		
	}
}



int  monst_t::try_to_give_birth() {
	if (time_done_pregnant && ms_count > time_done_pregnant ) {
		time_done_pregnant = 0;
		char ts[255];			
		if (base == &mb_worker) {
			int quit = (irand(flags & AF_FAMILY ? 4 : 10) == 0);
			hud->long_news((char *)"%s gave birth to a baby %s.%s",
				A_name(ts,1), (irand(2) == 0 ? "boy" : "girl"), 
				quit ? 
			  "  She quit to go home and take care of her baby." : 
			  "  Her family will take care of her baby while she is at work."
			);
			energy = MIN(30, energy);			
			if (quit) jump_out_time = ms_count + 8000;
			return 1;
		}
		if (my_ast) {
			int lay_eggs = flags & AF_EGGS;
			int number_of_puppies = 1;
			char *puppy_name = (char *)"little one";
			char *litter_name = (char *)"litter";
			if (base == &mb_roachy) {
				number_of_puppies = 3 + irand(4); puppy_name = (char *) "roachlet";
				litter_name = (char *) "swarm";
			} else if (base == &mb_quitofly) {
				number_of_puppies = 3 + irand(4);  puppy_name = (char *)  "quito larva";
				litter_name = (char *) "swarm";
			} else if (base == &mb_furbit) {
				number_of_puppies = 1 + irand(4);  puppy_name = (char *) "furb";
				energy = MIN(30, energy);			
			} else if (base == &mb_radrunner) {
				puppy_name = (char *) "radchick";
			} else if (base == &mb_squeeker) {
				puppy_name = (char *) "squeekie";
			} else if (base == &mb_meeker) {
				puppy_name = (char *) "meekie";
			} else if (base == &mb_sneeker) {
				puppy_name = (char *) "sneekie";
			} else if (base == &mb_cheech) {
				energy = MIN(30, energy);			
			} else if (base == &mb_rexrex) {
				puppy_name = (char *) "larex";
				number_of_puppies =  1 + irand(3);			
				energy = MIN(30, energy);			
			} else if (base == &mb_uberworm) {
				puppy_name = (char *) "small worm";
			} else if (base == &mb_toothworm) {
				puppy_name = (char *) "small worm";
				litter_name = (char *) "tangle";
				number_of_puppies =  1+ irand(5);
			} else if (base == &mb_molber || base == &mb_mubark) {
				puppy_name = (char *) "calf";
				energy = MIN(30, energy);			
			}
			monst_t *puppy = NULL;
			for (int c = 0; c < number_of_puppies; c++) {
				puppy = new monst_t(base, 0);
				puppy->x = x; puppy->y = y;				
				puppy->momma = this;
				puppy->daddy = spouse;
				baby = puppy;
				if (spouse) spouse->baby = puppy;
				my_ast->add_monst(puppy);
			}
			if (lay_eggs && number_of_puppies == 1) {
				hud->long_news((char *)"%s layed an egg.",
					A_name(ts,1), puppy_name
				);
			} else if (lay_eggs) {
				hud->long_news((char *)"%s layed its eggs.",
					A_name(ts,1)
				);
			} else if (base == &mb_cheech) {
				puppy_name = (char *) "chichi";
				puppy_name = (char *) "cheena";
				hud->long_news((char *)"%s gave birth to a %s.",
					A_name(ts,1), (puppy->sex == 1 ? "chichi" : "cheena" )
				);
			} else if (number_of_puppies == 1) {
				hud->long_news((char *)"%s gave birth to a %s%s.",
					A_name(ts,1), (puppy->sex == 1 ? "male " : puppy->sex == 2 ? "female " : ""),puppy_name
				);
			} else {
				hud->long_news((char *)"%s gave birth to a %s of %i %ss.",
					A_name(ts,1), litter_name, number_of_puppies, puppy_name
				);
			}
			return number_of_puppies;
		}
	}
	return 0;
}


int  monst_t::inject(int inject_idx, int flags) {
        char *drug = (char *)"???";
        if (inject_idx >= 0 && inject_idx < 10) drug = ivalu_list[inject_idx + FIRST_INV_INJECTOR_IDX].nam;
        while (inject_idx == 3) inject_idx = 1 + irand(10);
        switch (inject_idx) {
	case  0 : //  "Detox" cures poison (usually makes ansey)
                if (drugged > 0) {
       				drugged -= 1000;
       				hud->long_news((char *)"The %s starts looking better.", name);
              	}
       		if (drugged < 0) drugged = 0;
       		met_rate_bonus += 90 + irand(280);
       		if (krulasite_inf) {
	        	krulasite_inf = 0;
        	       	if (krulasite_sym() && irand(10) < 6) {
               		  hit(hp,snd_die,-1);  // usually has bad reaction if symptoms have set in
               		} else hud->long_news((char *)"The %s wiped out the %s's infection.", drug, name);
       		}
        break;
	case  1 : //  "Rozolin"  Minor healing and Improves Max Health (Paranioa sometimes)
            if (mhp) {
               drugged += 50;
               int omhp = mhp; int ohp = hp;
	       float hpr = (float)(hp)/(mhp);
	       if (hpr > 0.9 && irand(1)) mhp += 10;
	       hp += mhp / 10; if (hp > mhp) hp = mhp;
	       if (mhp != omhp) hud->long_news((char *)"The %s looks much healthier.", name);
	       else if (hp != ohp) hud->long_news((char *)"The %s looks healthier.", name);
	       else hud->long_news((char *)"The %s has no effect on the %s.", drug, name);
            }
        break;
	// case  2 :  "Glowing Green Liquid" Random Effect [greencrystal:cheech]
	case  3 : //  "Super-Stim" Temporary energy 8xMax (Animals Somtimes Explode on this stuff) [redrock:cheech]
	   drugged +=  (irand(12)==0 ? 3000. : 100.);
           speed_bonus += (speed / 5);
           energy_bonus += 8.0;
           met_rate_bonus += 2 + irand(12);
	   hud->long_news((char *)"The %s starts moving faster.", name);
        break;
	case  4 : //  "JR-19AZF" nothing yet
		hud->long_news((char *)"The %s does not effect the %s.", drug, name);
        break;
	case  5 : //  "Ais Ormba"  // Sets Metrate to 0 and life expacantcy x50 (rarely blindness)
           if (irand(20) == 0) fear_dist = 0;
           time_die = ms_count + 9999999;
           flags |= AF_HUNGERLESS;
           mhp += mhp / 8;
           hp = mhp;
           drugged = 0;
	   hud->long_news((char *)"The %s looks satisfyed.", name);
        break;
	case  6 : //  "Nanno Bots"  Temp 5x Healing Rate (not technically a drug)
	   healing_bonus = 5.0;
           flags &=~ AF_PARANOID;
        break;
	case  7 : /* Causes Rapid Growth */
	   drugged +=  (irand(9)==0 ? 300. : 40.);
	   growth_bonus += (irand(4)==0 ? 16. : 4.);
	   if (growth_bonus >= 20.) {
      		drugged += 400.;
      		growth_bonus *= 0.8;
      	}
	   met_rate_bonus += irand(60);
	   hud->long_news((char *)"The %s looks bigger.", name);
        break;
	case  8 : // "Morwdolb" /* Increases Strength (known to make violent rarely canabolistic) [wormparts:cheech]*/
		drugged +=  350.;
		str += 2 * (irand(3)==0);
		str_bonus +=  8.0;
		agression += 30;
		agression_bonus = (irand(6) == 0 ? irand(2000) : 70 );
		if (agression_bonus > 1000.) flags |= AF_BESERK;
		hud->long_news((char *)"The %s looks stronger.", name);
		if (base == &mb_quitofly && irand(8) == 0) mutate(&mb_mamothquito);
        break;
	case 9 : // "Project 17", Causes (sometimes Intence Trans-spicies) Mutations [roachyparts:cheech]
		energy_bonus -= 50;
		if (irand(3) == 0) {
		drugged /= 2.;
		int r = irand(100);
		if (check_mb_list( humanoids) ) {
			while (drop_weap(1));
			mutate(
				(r < 70 ? &mb_throbid :
				r < 80 ?  &mb_toothworm :
				r < 92 ?  &mb_vaerix :
				r < 96 ?  &mb_bigasaurus : &mb_uberworm
				), 30 + irand(260)
			);
   		} else if (base == &mb_slymer) mutate(&mb_biocite);
		else if (base == &mb_mubark) mutate(r < 50 ? &mb_molber : &mb_rexrex, 60 + irand(600));
		else if (base == &mb_roachy) mutate(r < 70 ? &mb_pygment : &mb_krulasite);
		else if (base == &mb_pygment) mutate(&mb_drexler);
		else if (base == &mb_throbid) mutate(&mb_bigasaurus);
		else if (base == &mb_furbit) mutate(&mb_malcumbit);
		else if (base == &mb_bervul) mutate(&mb_rexrex);
		else if (base == &mb_shruver) mutate(&mb_ackatoo);
		else if (base == &mb_vaerix) mutate(&mb_yazzert);
		else if (base == &mb_rexrex) mutate(&mb_bervul);
		else if (base == &mb_ackatoo) mutate(&mb_shruver);
		else if (base == &mb_yazzert) mutate(&mb_vaerix);
		else if (base == &mb_squeeker) mutate(&mb_meeker);
		else if (base == &mb_meeker) mutate(&mb_sneeker);
		else if (base == &mb_sneeker) mutate(&mb_squeeker);
		else hud->long_news((char *)"The %s does not change the %s.", drug, name);
           } else {
           	drugged +=  450.;
                mutate(base);
           }
        break;
        default : hud->long_news((char *)"The %s has no effects on the %s.", drug, name);
        }
	return 0;
}

int  monst_t::eat_affect(monst_t *eatm) {
        if (!eatm) return 0;
	int ret = 4;
        monst_base *eat;
	if (eatm) {
		speed_bonus += (0.33 * eatm->speed_bonus);
		energy_bonus  += (0.33 * eatm->energy_bonus);
		str_bonus  += (0.33 * eatm->str_bonus);
		if (can_poison()) met_rate_bonus  += (0.10 * eatm->met_rate_bonus);
		healing_bonus  += (0.33 * eatm->healing_bonus);
		agression_bonus += (0.33 * eatm->agression_bonus);
		invisability_bonus  += (0.33 * eatm->invisability_bonus);
		growth_bonus  += (0.33 * eatm->growth_bonus);
		restlessness  += (0.33 * eatm->restlessness);
		if (can_poison()) drugged  += ( 0.10 * eatm->drugged);
   	}
        if (eat == &mb_quiver) {
	} else if (eat == &mb_poisonshroom && can_shroom_poison()) {
   		ret = 0;
	} else if (eat == &mb_psychoshroom && can_shroom_poison()) {
   		ret = 0;
	} else if (eat == &mb_pyroshroom && flammable()) {
   		hit(50,snd_die,-1);
   		ret = 0;
	} else if (eat == &mb_krulasite) {
   		// immunity
	} else if (eat == &mb_frogert) {
   		speed_bonus += 100.;
	} else if (eat == &mb_squeeker) {
   		agression += 4000;
   		 // voilent
	} else if (eat == &mb_meeker) {
        	
	} else if (eat == &mb_sneeker) {
   		invisability_bonus += 300.;
	} else if (eat == &mb_vaerix) {
   		mhp += 30;
	} else if (eat == &mb_yazzert) {
   		mhp += 50;
	} else if (eat == &mb_squarger) {
	} else if (eat == &mb_slymer && can_poison()) {
   		drugged += 800.;
		ret = 0;
	} else if (eat == &mb_toothworm) {
   		str_bonus += 100.;
	} else if (eat == &mb_cheech) {
	} else if (eat == &mb_stalker) {
   		invisability_bonus += 400.;
	} else if (eat == &mb_drexler) {
	} else if (eat == &mb_krakin) {
	} else if (eat == &mb_mamothquito) {
	} else if (eat == &mb_uberworm) {
   		str_bonus += 500.;
	} else if (eat == &mb_bigasaurus) {
	} else if (eat == &mb_raider) {
   		agression += 20;
	} else if (eat == &mb_hippie) {
   		agression -= 70;
	} else if (eat->inv_link && eat->inv_link->typ == inv_proj) {
   		// slymers may digest weapons sometimes
   	}
	return ret;
}

#define NUM_UNLABELED_FLAVORS 41
static monst_base *unlabeled_flavors[NUM_UNLABELED_FLAVORS] = {
  	&mb_fern,       &mb_shrub, 	&mb_stalkplant, &mb_happytree,	&mb_poisonshroom,
  	&mb_psychoshroom,&mb_pyroshroom,	&mb_roachy, 	&mb_krulasite,	&mb_quitofly,
	&mb_pygment,	&mb_frogert,	&mb_furbit,	&mb_shruver,	&mb_radrunner,
	&mb_mubark,	&mb_molber,	&mb_ackatoo,	&mb_squeeker,	&mb_meeker,
	&mb_sneeker,	&mb_vaerix,	&mb_yazzert,	&mb_squarger,	&mb_slymer,
	&mb_bervul,	&mb_rexrex,	&mb_throbid,	&mb_toothworm,	&mb_orick,
	&mb_cheech,	&mb_hoveler,	&mb_stalker,	&mb_drexler,	&mb_krakin,
	&mb_dragoonigon,&mb_mamothquito,&mb_uberworm,	&mb_bigasaurus,	&mb_titurtilion, &mb_quiver
};

static char *unlabeled_desc[] = { 
	(char *)"Canned", (char *)"Steamed", (char *)"Ground", 
	(char *)"Minced", (char *)"Boiled", (char *)"Stir-Fried"
};

int  monst_t::feed(int food_idx) {
	char fn_buff[100] = "some unknown food";
	char *food_name = (food_idx != 4 && food_idx >= 0 && food_idx < 10 ?
		ivalu_list[food_idx + FIRST_INV_FOOD_IDX].nam :
		fn_buff
	);
	static monst_base *norm_food[5] = {
  		NULL, &mb_stalkplant, &mb_furbit, &mb_mubark, NULL
	};
	monst_t *eat = new monst_t(&mb_fern); // junk
	int rating = (base == &mb_cheech && food_idx > 4 ? 2 : 1);
	switch (food_idx) {
        case 0: { // mixed veg
        	static monst_base *in_mixed_vegitables[] = {
  			&mb_fern, &mb_shrub, &mb_happytree
		};
        	for (int c = 0; c<3 && rating != 4; c++) {
                        eat->base = in_mixed_vegitables[c];
        		if (will_eat(eat)) rating = 3;
                	if (likes_to_eat(eat)) rating = 4;
         	}
        	in_stomach += 600;
	}
        break;
        case 4: { // unlabeled_can
        	int ulidx = irand(NUM_UNLABELED_FLAVORS);
                snprintf(fn_buff, sizeof fn_buff, "%s %s", unlabeled_desc[irand(6)], unlabeled_flavors[ulidx]->name);
                norm_food[4] = unlabeled_flavors[ulidx];
        }
        // no break;
        case 1:
        case 2:
        case 3: { // normal foods
                eat->base = norm_food[food_idx];
        	if (will_eat(eat)) rating = 3;
                if (likes_to_eat(eat)) rating = 4;
                if (base == eat->base) rating = 0;
                int ea = eat_affect(eat);
                if (ea < rating) rating = ea;
                in_stomach += 600;
 			}
        break;
	case 5: { // Marja-cola
        	drugged += 80;
                speed_bonus -= 200.;
                met_rate_bonus += 200.;
	}
	break;
        case 6: { // 	IHB-77 /* mellows animal and saps energy (ocasional death) [eyestalk:cheech]*/
        	drugged += (irand(9) == 0 ? 10000 : 500);
                met_rate_bonus -= 80.;
                energy *= 0.70;
                agression -= 500;
 	}
 	break;
        case 7: { // Vojo Dust Invis (Rarely Permant) [stalkerparts:stalker]
  		drugged += 80;
  		if (irand(15) == 0) flags |= AF_CLOAKING; else invisability_bonus += 1000.;
        }
        break;
	case 8: { // Zozin  mellows animal and increases appatite (rare death) [happytreeparts:cheech]
        	drugged += (irand(17) == 0 ? 10000 : 500);
                met_rate_bonus += 300.;
                agression -= 500;
 	}
 	break;
	case 9: { // Lykedofan Increases vision (Paroinoa sometimes)
        	if (irand(12) == 0) flags |= AF_PARANOID;
 	}
 	break;
 	}
 	static char *eat_str[] = {(char *)"gags while eating",(char *)"reluctantly eats", (char *)"eats", (char *)"quickly eats", (char *)"wolfs down"};
	static int agr_chg[] = {1500, 100, 0, -100, -250};
 	agression += agr_chg[rating];
 	hud->long_news((char *)"The %s %s %s.", name, ( food_idx == 5 ? (char *)"drinks" : eat_str[rating]), food_name);
	return 1;
}

void monst_t::decay_bonus(float &f) {
      if (f > 0) {
         f -= (met_rate + (met_rate_bonus > 0 ? met_rate_bonus / 4. : 0));
         if (f < 0) f = 0;
      } else if (f < 0) {
         f += (met_rate + (met_rate_bonus > 0 ? met_rate_bonus / 4. : 0));
         if (f > 0) f = 0;
      }
}

int monst_t::flammable() {
	return (
		( org_idx != org_crysillians &&
		base != &mb_dragoonigon &&
		base != &mb_hole &&
		base != &mb_meteor &&
		base != &mb_pyroshroom &&
		!(base->inv_link && base->inv_link->typ == inv_mineral))
	);
}

int monst_t::can_poison() {
	return (
		((flags & AF_INORGANIC) == 0) &&
		base != &mb_slymer &&  base != &mb_psychoshroom && base != &mb_poisonshroom
	);
}

int monst_t::can_shroom_poison() {
	return (
		((flags & AF_INORGANIC) == 0) &&
		base != &mb_slymer && base != &mb_mubark && base != &mb_cheech &&
		!isa_shroom()
	);
}


int monst_t::can_sleep() {
	return (!isa_plant() && ((flags & AF_INORGANIC) == 0));
}

int monst_t::can_pickup_spores() {
	return ( (flags & AF_FLYING) == 0 && !inanimate());
}



void monst_t::poll() {
 	// == pointer safety / number cleanup ==
 	if (lev_editor && ai_func != ai_player) return;
   	if (who_hit_me_last) if (who_hit_me_last->dead() || !my_ast) who_hit_me_last = NULL;
        if (see_monst) if (!my_ast) see_monst = NULL;
        ang = afix(ang);
        // == AI Actions ==
        if (alive() && (my_ast != NULL)) {
            // living ai actions on asteroid
			switch (ai_func) {
			case ai_animal: animal_ai();break;
			case ai_plant: plant_ai();break;
			case ai_player: player_ai();break; // implemented in mmam.cc
			case ai_building: building_ai();break;
			case ai_hunter: hunter_ai();break;
			case ai_crysillian: crysillian_ai();break;
			case ai_item: item_ai(); break;
			case ai_trainer : trainer_ai(); break;
			case ai_merchant : merchant_ai(); break;
			case ai_tourist : tourist_ai(); break;
			case ai_dragoonigon : dragoonigon_ai(); break;
			case ai_beholder : beholder_ai(); break;
			case ai_turret : turret_ai(); break;
			case ai_tank : tank_ai(); break;
			case ai_soldier : soldier_ai(); break;
			case ai_krulasite : krulasite_ai(); break;
			case ai_meeker : meeker_ai(); break;
			case ai_ackatoo : ackatoo_ai(); break;
			case ai_stalker : stalker_ai(); break;
			case ai_cheech  : cheech_ai(); break;
			case ai_hoveler : hoveler_ai(); break;
			case ai_slymer : slymer_ai(); break;
			case ai_meteor: meteor_ai(); break;
			case ai_worker: worker_ai(); break;
			}
			sqwak(snd_bark); /* sqwak takes care of timings */
		}
   	// == life processes ==
        if (alive() && (game_turn % TURNS_PER_SEC == 0)) {
                if (base == &mb_squeeker && my_ast) {
			// squeekers make other animals happy
			for(list<monst_t *>::iterator mon = my_ast->m.begin(); mon != my_ast->m.end(); mon++) {
				monst_t *m = *mon;
				if (
					m != this &&
					dist(x,y,m->x,m->y) < 300 &&
					(m->flags & AF_INORGANIC) == 0 &&
					m->agression > 100
				) m->agression -= 10;
		        }
		}
		if (stomach) {
			if (in_stomach) {
    				is_starving = 0;
        			in_stomach -= (met_rate + met_rate_bonus / 4.);
                                if (in_stomach < 0) in_stomach = 0;
        		} else {
           			if (!is_starving) hud->long_news((char *)"A %s is starving!", name);
           			is_starving = 1;
                        	if (hit(MID(1, mhp / 50, 25), snd_die_starving, -1)) {
                           		if (my_ast != remote_ast) hud->karma -= 50;
                           		hud->long_news((char *)"A %s starved to death!", name);
   		               	}
           		}
   		}
   		if (agression < 10000 && krulasite_sym()) agression += 15;
                if (drugged && od_thresh) {
                	if (drugged > od_thresh) {
				if (hit((int)(MID(1, mhp / 30, 50) * (drugged / od_thresh > 2. ? 1.5 : 1.))), snd_die_od, -1) {
					if (my_ast != remote_ast) hud->karma -= 280;
      					hud->long_news((char *)"A %s died from a drug overdose!", name);
                        	}
                   	}
                	if ((drugged -= (met_rate / 4 + (met_rate_bonus > 0 ? met_rate_bonus / 4. : 0))) <= 0) {
                                od_thresh = od_thresh * 105 / 100 + 1;
                                drugged = 0;
                        }
          	}
          	energy += (is_starving ? -5 : MID(1, met_rate / 4., 10)) + met_rate_bonus / 3.;
          	if (energy < 0) energy = 0;
                if (energy > max_energy) energy = max_energy;
          	if (nutrish < max_nutrish) nutrish += (int)(MID(1,  energy / 50, 20));
                if ((flags & AF_INORGANIC) == 0 && alive() && hp  < mhp && (drugged <= od_thresh)) { hp += (int)(MID(1, energy / 100, 5) + healing_bonus); if (hp > mhp) hp = mhp; }
                if (on_fire > ms_count) {
                	if (ai_func == ai_item) {
                   		if (hit(50),snd_die_burn,-1) hud->long_news((char *)"A %s burned up!", name);
                   	} else {
                                krulasite_inf = 0;
                        	if (hit(6),snd_die_burn,-1) hud->long_news((char *)"A %s burned to death!", name);

                    	}
               		if (base != &mb_mubark) spore_psycho_count = spore_poison_count = 0;
        	} else on_fire = 0;
                if ((ms_count < time_mature || growth_bonus > 0) ) {
                	float gr = growth_rate * ( 1. + growth_bonus);
                        size += (int)(gr);
                        fix_radius();
                        growth_bonus -= 0.1; if (growth_bonus < 0.) growth_bonus = 0.;                        
                }
                decay_bonus(healing_bonus);
  	        decay_bonus(speed_bonus);
                decay_bonus(energy_bonus);
                decay_bonus(str_bonus);
                decay_bonus(met_rate_bonus);
	        decay_bonus(agression_bonus);
                decay_bonus(invisability_bonus);
       	}
       	if (alive()) {
           if (asleep()) { feel(emot_sleep); do_act(ACT_KO); }
           else if (tired()) feel(emot_tired);
           if (od_thresh && drugged > od_thresh)  feel(emot_sick);
           if (!pow_on) feel(emot_nopower);
           if (!on) feel(emot_off);
        } else feel(emot_dead);
       	// == handle emotion bubbles ==
       	if ( ((flags & AF_CLOAKING) == 0 || arg_cheater) && !isa_plant() && want_emote && (emote_icon != want_emote || ms_count - emote_count > 5000)) {
  		emote_icon = want_emote;  emote_count = ms_count;
        }
        want_emote = 0;
}

void monst_t::caged_poll(cage_t *jail) {
	poll();
	if (agression <= 50 && alive()) {
       	feel(emot_halo);
   	} else if (jail && ms_count > attack_time && agression > 50 && alive() && energy > 2) {
		jail->hit(irand(str));
		calc_can_attack_next();
       	if (agression < 100) attack_time += 1000;
       	if (agression < 90) attack_time += 1000;
       	if (agression < 80) attack_time += 1000;
       	if (agression < 70) attack_time += 1000;
       	if (agression < 60) attack_time += 1000;
       	feel(emot_pissed);
        agression -= 1;
    }  	
}

void monst_t::calc_can_attack_next() {
        // line [100, 1.0] [0, 6.0] (attack takes 6x as long when near passed out)
       	attack_time = ms_count + (int)(attack_rate * (energy > 100 ? 1.0 : 6. + (-0.05 * energy)  ));
}


void monst_t::rename(char *s) {
        free(name); name = strdup(s);
}

void turret_rsw(BITMAP *dest, BITMAP *weap, int x, int y, int y_ofs, float ang, int new_w) {
	if (new_w <= 0) return;
	pivot_scaled_sprite(dest, weap,
		x, y, weap->w / 2 + 1, y_ofs + 1,
		ftofix(0x80 * ang / M_PI), (new_w << 16)/weap->w
	);

}

BITMAP *monst_t::emote_bitmap() {
	BITMAP *e_bmp = NULL;
	switch (emote_icon) {
	case emot_none: break;
	case emot_happy:      e_bmp = (BITMAP *)sprites[SPR_HAPPY].dat; break;
	case emot_confused:   e_bmp = (BITMAP *)sprites[SPR_CONFUSED].dat; break;
	case emot_bored:      e_bmp = (BITMAP *)sprites[SPR_BORED].dat; break;
	case emot_off:        e_bmp = (BITMAP *)(!on ? sprites[SPR_POWEROFF].dat : NULL); break;
	case emot_nopower:    e_bmp = (BITMAP *)(!pow_on ? sprites[SPR_NOPOWER].dat : NULL); break;
	case emot_surprized: e_bmp = (BITMAP *)sprites[SPR_EXCLAM].dat; break;
	case emot_halo: e_bmp = (BITMAP *)sprites[SPR_HALO].dat;   break;
	case emot_hungry: e_bmp = (BITMAP *)sprites[SPR_HUNGRY].dat; break;
	case emot_pissed: e_bmp = (BITMAP *)sprites[SPR_PISSED].dat; break;
	case emot_dead: e_bmp = (BITMAP *)sprites[SPR_DEAD].dat;   break;
	case emot_sick: e_bmp = (BITMAP *)sprites[SPR_SICK].dat;   break;
	case emot_eating: e_bmp = (BITMAP *)sprites[SPR_EATING].dat;   break;
	case emot_sleep:  e_bmp = (BITMAP *)sprites[SPR_ASLEEP].dat;   break;
	case emot_tired: e_bmp = (BITMAP *)sprites[SPR_SWEAT].dat;   break;
	case emot_inject: e_bmp = (BITMAP *)sprites[inject_idx].dat;   break;
	}
	return e_bmp;
}

extern aspath orick_spear_spr_chill[],  orick_gun_spr_chill[];
extern aspath orick_spear_spr_eat[],    orick_gun_spr_eat[];
extern aspath orick_spear_spr_walk[],   orick_gun_spr_walk[];
extern aspath orick_spear_spr_attack[], orick_gun_spr_attack[];

extern char *no_sex_reason;

void monst_t::draw() {
	if (!base) {
		allegro_message("Monster w/o base error");
		exit(0);
	}
	if (!base) return;
	aspath *ap;
	weap_t *w = armed();
	if (
		(action_idx == ACT_EAT || action_idx == ACT_ATTACK || action_idx == ACT_BUILD) && 
		attack_time < ms_count
	) {
		do_act(alive() ? ( asleep() ? ACT_KO : ACT_CHILL) : ACT_DIE);
	}
	ap = ( // patch for orick multi weapon weirdnesses
		w &&  base == &mb_orick ? (
			action_idx == ACT_CHILL ? 
				( w->t == &IDF_SPEAR ? orick_spear_spr_chill : orick_gun_spr_chill) :
			action_idx == ACT_WALK  ? 
				( w->t == &IDF_SPEAR ? orick_spear_spr_walk : orick_gun_spr_walk) :
			action_idx == ACT_ATTACK ?
			( w->t == &IDF_SPEAR ? orick_spear_spr_attack :	orick_gun_spr_attack) :
			NULL
	    ) : NULL
	);
	if (!ap) ap = (
		action_idx == ACT_CHILL  ? base->spr_chill :
		action_idx == ACT_WALK   ? base->spr_walk :
		action_idx == ACT_EAT    ? base->spr_eat :
		action_idx == ACT_ATTACK || action_idx == ACT_BUILD ? base->spr_attack :
		action_idx == ACT_DIE || action_idx == ACT_KO  ? base->spr_die : NULL
	);
	int xx = (int)(x - (my_ast ? my_ast->camera_x : 0));
	int yy = (int)(y - (my_ast ? my_ast->camera_y : 0));
	int rxx = (int)(run_x - (my_ast ? my_ast->camera_x : 0));
	int ryy = (int)(run_y - (my_ast ? my_ast->camera_y : 0));
	
	if (!ap) {
		allegro_message("Unhandled frame (%i:%i) for %s!\n", action_idx, action_frame, name);
		exit(1);
	} else {	
		if (ap[0].idx == -1 || ap[action_frame].idx == -1) {
			allegro_message("Unhandled frame (%i:%i) for %s!\n", action_idx, action_frame, name);
			exit(1);
		}
		int frame_no = -1;
		if (action_idx == ACT_WALK) {
			int wsc=0; for (;ap[wsc].idx != -1;wsc++);
			if (wsc) {
				action_frame =((int)(traveled / base->walk_span[0])) % wsc;
				frame_no = ap[action_frame].idx;
			}
		} else if (base == &mb_epsilonhiscanner && !pow_on) {
			frame_no = ap[0].idx;   frame_ms = ms_count;
		} else if (!pow_on) {
			frame_no = ap[action_frame].idx;   frame_ms = ms_count;
		} else if (base == &mb_hole && !hole_monst) {
			frame_no = ap[1].idx; 	frame_ms = ms_count;
		} else {
			if (ap[action_frame].ms > 0) while (frame_ms < ms_count) {
				action_frame++;
				if (ap[action_frame].idx == -1) {
					action_frame = 0;
				}
				if (ap[action_frame].ms <= 0) goto locked_frame;
				frame_ms += ap[action_frame].ms;
			}
			locked_frame: frame_no = ap[action_frame].idx;
		}
		if (frame_no < 0) {
			allegro_message("Invalid frame for %s1\n", name);
			exit(1);
		} else {
			BITMAP *spr = (BITMAP *)(sprites[frame_no].dat);
			if (encl && base != &mb_hole) {
				int encl_grp = 4; /* SPR_ENCL1_DEAD    SPR_ENCL1_FLOOR    SPR_ENCL1_INV    SPR_ENCL1 */
				int ofs = (base->inv_link ? base->inv_link->lvl - 1 : 0) * encl_grp;
				BITMAP *floor_bmp = create_sub_bitmap(
					(BITMAP *)sprites[(dead() ? SPR_ENCL1_DEAD : SPR_ENCL1_FLOOR) + ofs].dat,41,41,42,42
				);
				BITMAP *cage_bmp =  (
					dead() ? NULL :
					create_sub_bitmap((BITMAP *)sprites[SPR_ENCL1 + ofs].dat,41,41,42,42)
				);
				encl->encl_draw(
					xx - encl->pw / 2, yy - encl->ph / 2,
					xx + encl->pw / 2, yy + encl->ph / 2,
					floor_bmp, cage_bmp
				);
				if (floor_bmp) destroy_bitmap(floor_bmp);
				if (cage_bmp)  destroy_bitmap(cage_bmp);
			} else {
				if (flags & AF_FLYING && alive()) shadow_rsw(active_page, frame_no,
					xx + 4, yy + 4, ang - (M_PI / 2.), (int)(radius * 2)					
				); 
				sprite_rsw(active_page, spr, xx, yy, ang - (M_PI / 2.), (int)(radius * 2) ,
					base->flags & AF_CLOAKING && action_idx == ACT_WALK  ? 
					  MID(10, 150 - (int)dist(x,y,ship->x,ship->y), 90) : 255
				);
			}
			mark_pmask(&mask, spr, ang - (M_PI / 2.), (int)(radius * 2));
			BITMAP *gun = NULL;
			int gun_cy = 15;
			int gun_siz = 25;
			if (ap[action_frame].wx >= 0 && alive()) {
					float dx = ap[action_frame].wx - spr->w / 2.;
					float dy = ap[action_frame].wy - spr->h / 2.;
					float d =  radius * z_dist(dy,dx) / (spr->w / 2.);
					float a = atan2(dy,dx) + ang - (M_PI / 2.);
					int   gx = (int)( xx + d * cos(a));
					int   gy = (int)( yy + d * sin(a));
					if (this == ship && base == &mb_netship) {
						gun = (BITMAP *)sprites[SPR_HRV_GUN].dat;
						wang = (input_map.target_valid ? atan2( input_map.target_y - ship->y, input_map.target_x - ship->x) : ang );
					}
					weap_t *w = armed();
					if (w && w->t->typ == inv_proj) {
						int sn = (w->a_time < ms_count ? w->t->ws1 : w->t->ws2);
					if (sn > 0 && sn != SPR_NO_LIL_PIC) {
							gun = (BITMAP *)(sprites[sn].dat);
							gun_cy = w->t->hy;
							gun_siz = gun->w * 3 / 4;
					}
				}
				if (gun) turret_rsw(active_page, gun, gx, gy, gun_cy, wang - (M_PI / 2.), gun_siz);
			}
			if (arg_xinfo && hud->track_monst ) {
				monst_t *t = hud->track_monst;
				if (this == t->see_monst) circle(active_page, xx,yy, 12, White);
				if (this == t->see_buddy) circle(active_page, xx,yy, 12, Yellow);
				if (this == t->who_hit_me_last) circle(active_page, xx,yy, 14, LightRed);
				if (this == t->chase_last) circle(active_page, xx,yy, 16, LightGreen);
				if (this == t->see_hole) circle(active_page, xx,yy, 10, Blue);
			}
			if (arg_xinfo && (this == hud->track_monst || hud->track_monst == NULL)) {
			    int tyy = yy - (int)radius - 8;
			    int tystep = text_height(&font_tiny);
			    int txx = xx + (int)radius + 4;
			    /*
			    textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
						"M: %i  V: %1.5f I: %0.3f", size, z_dist(vx, vy), size * z_dist(vx, vy)
		        );tyy -= tystep;
		        */
				char *wants_it = (char *)(buddy_wants_to_get_laid() ? "YES!" : "no");
				/*
				textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
						"sex: %s  (%s) H:%is P:%is buddywtgl: %s %s", 
						(sex == 1 ? "Male" : sex == 2 ? "Female" : "Other"), 
						(time_done_pregnant ? "Pregnant!" : ms_count > horny_time ? "Horny!" : "normal"),
						(horny_time - ms_count) / 1000, 
						(time_done_pregnant ? (time_done_pregnant - ms_count) / 1000 : 0),
		        		wants_it, 
		        		
		        );tyy -= tystep;
		        */
		        if (aiu3) {
			    	textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
						"aiu3: %08x %10i", aiu3, aiu3
		        	);tyy -= tystep;
		        }
			    if (aiu2) {
			    	textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
						"aiu2: %08x %10i", aiu2, aiu2
		        	);tyy -= tystep;
		        }
			    if (aiu1) {
			    	textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
						"aiu1: %08x %10i", aiu1, aiu1
		        	);tyy -= tystep;
		        }
			    if (see_monst) textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
					"see_monst: %s", see_monst->name
		        );tyy -= tystep;
			    if (see_buddy) textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
					"see_buddy: [%i,%i] %s %s", sex, see_buddy->sex ,see_buddy->name, no_sex_reason ? no_sex_reason : ""
		        );tyy -= tystep;
			    if (who_hit_me_last) textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
					"who_hit_me_last: %s", who_hit_me_last->name
		        );tyy -= tystep;
			    if (chase_last) textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
					"chase_last: %s", chase_last->name
		        );tyy -= tystep;
			    if (see_hole) textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
					"see_hole: %s", see_hole->name
		        );tyy -= tystep;
			    if (hunger_level) textprintf_ex(active_page, &font_tiny, txx, tyy, White, -1 ,
					"hunger_level: %i", hunger_level
		        );tyy -= tystep;			  
			}
			/*
			if (rxx && ryy) {
				textprintf_ex(active_page, &font_tiny, rxx+6, ryy-10, White, -1 ,
					"%s", name
		        );
			    circle(active_page,rxx,ryy,6, LightGreen);
			}
			*/
			if (base == &mb_netship && this == ship) {
				// draw turret, claw, etc.
				BITMAP *claw = (BITMAP *)(
					hud->fire_claw_dir == 0 ? NULL :
					sprites[hud->spr_claw[hud->hooked_monst || hud->fire_claw_dir == 1 ? 0 : 1]].dat
				);
				if (claw) {
					int cvx = (int)(hud->claw_x - (my_ast ? my_ast->camera_x : 0));
					int cvy = (int)(hud->claw_y - (my_ast ? my_ast->camera_y : 0));
					float ad = (hud->hooked_monst ? dist(hud->hooked_monst->x, hud->hooked_monst->y, ship->x, ship->y) : 0.);
					line(active_page,xx,yy,cvx,cvy,
						! hud->hooked_monst ? White :
						hud->claw_line_out > ad ? Cyan : hud->claw_line_out < ad + 1. ? White :Red
					);
					sprite_rsw(active_page, claw, cvx, cvy, hud->claw_ang - (M_PI / 2.), (int)(claw->w/2));
				}
			}
			if (this == mainbase && dead() ) {
				if (dist(x,y,hud->claw_x,hud->claw_y) < radius && hud->money < REBUILD_COST) {
					textprintf_centre_ex(active_page, &font_bold, xx, yy, LightRed, -1 ,
						"INSUFFICIANT FUNDS!"
					);
				} else {
					textprintf_centre_ex(active_page, &font_tiny, xx, yy, White, -1 ,
						"Rebuild Base: $%i", REBUILD_COST
					);
				}
			}
	
			BITMAP *e_bmp = emote_bitmap();
			if (e_bmp && base != &mb_meteor) {
				int ec = ms_count - emote_count;
				if (ec < 16000) {                               // 2 .. 12               12 .. 17
					int oy = yy - (int)(radius/2) - (ec < 400 ? 2 + ec / 40 : 12 + abs(ec % 400 - 200) / 40 );
					if (emote_icon == emot_inject) {
						BITMAP *ss = (BITMAP *)sprites[SPR_PISSED].dat;
						stretch_sprite(active_page,e_bmp,xx,oy, ss->w, ss->h);
					} else draw_sprite(active_page,e_bmp,xx,oy );
				}
			}
			if (on_fire > ms_count) {
				BITMAP *fire_spr = (BITMAP *)sprites[SPR_CAMPFIRE1 + ((on_fire - ms_count) % 600) / 200].dat;
				sprite_rsw(active_page, fire_spr, xx + irand(2) - 1, yy + irand(2) - 1, 0., 
					(int)(radius * 2.5),
					MAX(140, 255 * (on_fire - ms_count) / 2000)
				);
			}
			if (this->base == &mb_worker) {
				textprintf_centre_ex(active_page, &font_tiny, xx, yy-14, White, -1 ,
					"%s (%s)", name, worker_class()
				);
			}
		}
	} 	
}



void monst_t::player_ai() {
	/* wrapper for user interface funcs */
	hunger_level = 0;
	if (ship->base == &mb_netship) {
	}
	if (ship == this) {
		if (trader ) walk(0.,  0., 0.);
		else walk(input_map.vt * turn_rate / 100.,  input_map.vy / 100., input_map.vx / 100.);
		if (my_ast->arena_glyph >= 0) feel(emot_confused);
	} else {
		if (aiu1 == 911) { //vehicle button for auto pickup
			if (input_map.vehicle_btn || ship != thehuman) { 
				aiu1 = 0;
				hud->long_news((char *)"Pickup canceled for %s.", name);
			}
			if (!alive()) { 
				aiu1 = 0;
				hud->long_news((char *)"Pickup canceled.  %s destroyed.", name);
			}
			if (thehuman->my_ast == my_ast) {
				if (chase(thehuman, 0.60, 0.08, 90, 100)) aiu1 = 0;
			} else if (thehuman->my_ast == home_ast || thehuman->my_ast == remote_ast) { 
				if (ms_count > aiu2) {
					my_ast->add_jumpgate(x,y);
					my_ast->unhook_monst(this);
					thehuman->my_ast->add_monst(this);
					x = irand(my_ast->pw);
					y = irand(my_ast->ph);
					my_ast->add_jumpgate(x,y);
				}				
			} else aiu1 = 0;
		} else if (input_map.vehicle_btn && ship == thehuman && alive() &&
			(thehuman->my_ast == home_ast || thehuman->my_ast == remote_ast) 
		) {
			aiu1 = 911; aiu2 = ms_count + 10000;
			if (mdist(thehuman) > 100 || thehuman->my_ast != my_ast) {
				hud->long_news((char *)"%s enroute for pickup.", name);
			}
		} else walk(0.,  0., 0.);
	} 	
}

void monst_t::forget(monst_t *monst) {
	if (monst == who_hit_me_last) who_hit_me_last = NULL;
	if (monst == see_monst) see_monst = NULL;
	if (monst == see_buddy) see_buddy = NULL;
	if (monst == see_hole) see_hole = NULL;
	if (monst == home_hole) home_hole = NULL;
	if (monst == chase_last) chase_last = NULL;
	if (monst == momma) momma = NULL;
	if (monst == daddy) daddy = NULL;
	if (monst == spouse) spouse = NULL;
	if (monst == baby) baby = NULL;
	if (this == ship) hud->forget(monst);
	if (trader_ui) trader_ui->forget(monst);
}

void monst_t::forget_all() {
	who_hit_me_last = see_monst = see_buddy = see_hole = chase_last = NULL;
	momma = daddy = spouse = baby = home_hole = NULL;
	if (this == ship) hud->forget_all();
    if (trader_ui) trader_ui->forget_all();
}


char *monst_t::He() { return (char *)(sex == 1 ? "He" : sex == 2 ? "She" : "It"); }
char *monst_t::he() { return (char *)(sex == 1 ? "he" : sex == 2 ? "she" : "it"); }
char *monst_t::him() { return (char *)(sex == 1 ? "him" : sex == 2 ? "her" : "it"); }
char *monst_t::His() { return (char *)(sex == 1 ? "His" : sex == 2 ? "Her" : "Its"); }
char *monst_t::his() { return (char *)(sex == 1 ? "his" : sex == 2 ? "her" : "its"); }

char *monst_t::a_name(char *scrap, int long_worker) { 
	if (long_worker && base == &mb_worker) sprintf(scrap, "%s the %s", name, worker_class(2));
	else if (base == &mb_worker) sprintf(scrap, "%s", name);
	else if (name[0] == 'A' || name[0] == 'E' || name[0] == 'I' || name[0] == 'O' || name[0] == 'U') sprintf(scrap, "an %s", name);  
	else sprintf(scrap, "a %s", name); 
	return scrap;
}

char *monst_t::A_name(char *scrap, int long_worker) { 
	a_name(scrap, long_worker); if (scrap[0] == 'a') scrap[0] = 'A'; 
	return scrap;
}	
