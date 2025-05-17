#include "mmam.h"

char *comma_i(char *ss, int i) {
	char *oo = ss;
	char scrap[255], *ii = scrap;
	int nc = snprintf( scrap, sizeof scrap, "%i", i);
	for (; *ii; ++ii) {
		*oo = *ii; oo++;
		--nc;		
		if (((nc % 3) == 0) && nc) { *oo = ','; oo++; }
	}
	*oo = 0;
	return ss;
}

char *htmlize(char *ss, char *ii) {
	char *oo = ss;
	for (; *ii; ++ii) if ((*ii >= 'A' && *ii <= 'Z') || (*ii >= 'a' && *ii <= 'z') || (*ii >= '0' && *ii <= '9') ) {		
		*oo = *ii; oo++;
	}
	*oo = 0;
	return ss;
}


char *readable_milliseconds(char *ss, int i) {
	if (i < 10000) {
		sprintf( ss, "%ims", i);
	} else {
		int ms = i % 1000;
		i /= 1000;
		int s = i % 60;
		i /= 60;
		int m = i % 60;
		int h = i / 60;
		if (h) {
			sprintf(ss, "%ih, %02im", h,m);		
		} else if (m && ms) {
			sprintf(ss, "%im, %02i.%1is", m,s,ms / 100);		
		} else if (m) {
			sprintf(ss, "%im, %02is", m,s);
		} else {
			sprintf(ss, "%i.%03is", s,ms);		
		}	
	}
	return ss;
}


void dump_critter_doc(FILE *ff) {
	monst_base **mb = mb_list_all;
	int n = 1;
	fprintf(ff,"<style type='text/css'>\n"
		".monst-attr { font-size : 80%; font-weight: bold; white-space: nowrap }\n"
		".monst-attr2 { font-size : 80%; font-weight: bold;  }\n"
		".monst-attr2 a {font-weight: normal; color: #444444; }\n"
		".ma-val {font-weight: normal; color: #444444; }\n"
		"</style>\n"
	);
	fprintf(ff,"<h2>The Critters</h2>\n\n");
	while (*mb) {
		char mm1[64], mm2[64], mm3[64], mm4[64], mm5[64];
         fprintf(ff, (char *)
         "<div style='margin: 8px 8px 8px 16px;'><!--img src='img/%s.jpg' -->"
         "    <a name='mon%s'></a>\n"
         "    <div style='text-size: 110%%; font-weight:bold;'>%02i: %s</div>\n"
         "    <div style='margin-top:4px; color: 222250;'>\n%s\n</div>\n"
         "    <span class='monst-attr'>ai_func: <span class='ma-val'>%'i</span></span>\n"
         "    <span class='monst-attr'>mhp: <span class='ma-val'>%'i</span></span>\n"
         "    <span class='monst-attr'>max_age: <span class='ma-val'>%s</span></span>\n"
         "    <span class='monst-attr'>mature_age: <span class='ma-val'>%s</span></span>\n"
         "    <span class='monst-attr'>speed: <span class='ma-val'>%'i</span></span>\n"
         "    <span class='monst-attr'>lat_speed: <span class='ma-val'>%'i</span></span>\n"
         "    <span class='monst-attr'>turn_rate: <span class='ma-val'>%'i</span></span>\n"
         "    <span class='monst-attr'>str: <span class='ma-val'>%'i</span></span>\n"
         "    <span class='monst-attr'>attack_rate: <span class='ma-val'>%'i</span></span>\n"
         "    <span class='monst-attr'>stomach: <span class='ma-val'>%'i</span></span>\n"
         "    <span class='monst-attr'>size: <span class='ma-val'>%'i</span></span>\n"
         "    <span class='monst-attr'>met_rate: <span class='ma-val'>%'i</span></span>\n"
         "    <span class='monst-attr'>nutrish: <span class='ma-val'>%i</span></span>\n"
         "    <span class='monst-attr'>max_energy: <span class='ma-val'>%i</span></span>\n"
         "    <span class='monst-attr'>od_thresh: <span class='ma-val'>%i</span></span>\n"
         "    <span class='monst-attr'>gestation: <span class='ma-val'>%s</span></span>\n"
         "    <span class='monst-attr'>agression: <span class='ma-val'>%i</span></span>\n"
         "    <span class='monst-attr'>flags: <span class='ma-val'>0x%08x</span></span>\n"
         "    <span class='monst-attr'>fear_dist: <span class='ma-val'>%i</span></span>\n"
         "    <span class='monst-attr'>herd_size: <span class='ma-val'>%i</span></span>\n"
         "    <span class='monst-attr'>fame: <span class='ma-val'>%i</span></span>\n"
         "    <span class='monst-attr'>bfc_price: <span class='ma-val'>%i</span></span>\n"
         "    <span class='monst-attr'>combat_price: <span class='ma-val'>%i</span></span>\n"
         "    <span class='monst-attr'>common: <span class='ma-val'>%i%%</span></span>\n"
         "    <span class='monst-attr'>org_idx: <span class='ma-val'>%i-%s</span></span>\n"
         "    <span class='monst-attr'>spr_big: <span class='ma-val'>%i</span></span>\n"
         , 
		htmlize(mm4,(*mb)->name), htmlize(mm5,(*mb)->name), n,	(*mb)->name, (*mb)->desc,
		(*mb)->ai_func,
		(*mb)->mhp, 
		readable_milliseconds(mm1, (*mb)->max_age), readable_milliseconds(mm2, (*mb)->mature_age), 
		(*mb)->speed, (*mb)->lat_speed, (*mb)->turn_rate, 
		(*mb)->str, (*mb)->attack_rate, (*mb)->stomach, (*mb)->size, (*mb)->met_rate,
		(*mb)->nutrish, (*mb)->max_energy, (*mb)->od_thresh, 
		readable_milliseconds(mm3, (*mb)->gestation), (*mb)->agression, (*mb)->flags,
		(*mb)->fear_dist,
		
		(*mb)->herd_size, (*mb)->fame, (*mb)->bfc_price, (*mb)->combat_price, (*mb)->common,
		(*mb)->org_idx,
		((*mb)->org_idx < 0 || (*mb)->org_idx >= org_last ? (char *)"INVALID ORG" : org_list[(*mb)->org_idx].nam), 
		((*mb)->spr_big ? *((*mb)->spr_big) : -1)
			
		
			/*
			int (*mb)->monster_idx
			(*mb)->subtype,
			int *blood_color;
			float fear_factor;
			aspath *spr_chill;
			int *walk_span;
			int *soundtab;
			aspath *spr_walk, *spr_eat, *spr_attack, *spr_die;
			struct ivalu_t* inv_link; int monster_idx, stat_caught,
				stat_inpark, stat_killed, stat_didscan, stat_won_medals,
				stat_valid_target, ;
			*/
		);
		
		if ((*mb)->likes_to_eat) {
			fprintf(ff, "<br /><span class='monst-attr2'>likes_to_eat: \n");
			for (monst_base **xb = (*mb)->likes_to_eat; *xb; xb++) { 
				fprintf(ff, "      <a href='#mon%s'>%s</a>\n", htmlize(mm4,(*xb)->name), (*xb)->name);
			}
			fprintf(ff,"</span>\n");
		}
		if ((*mb)->will_eat) {
			fprintf(ff, "<br /><span class='monst-attr2'>will_eat: \n");
			for (monst_base **xb = (*mb)->will_eat; *xb; xb++) { 
				fprintf(ff, "      <a href='#mon%s'>%s</a>\n", htmlize(mm4,(*xb)->name), (*xb)->name);
			}
			fprintf(ff,"</span>\n");
		}
		if ((*mb)->hates) {
			fprintf(ff, "<br /><span class='monst-attr2'>hates: \n");
			for (monst_base **xb = (*mb)->hates; *xb; xb++) { 
				fprintf(ff, "      <a href='#mon%s'>%s</a>\n", htmlize(mm4,(*xb)->name), (*xb)->name);
			}
			fprintf(ff, "</span>\n");
		}
		if ((*mb)->scared_of) {
			fprintf(ff, "<br /><span class='monst-attr2'>scared_of: \n");
			for (monst_base **xb = (*mb)->scared_of; *xb; xb++) { 
				fprintf(ff, "      <a href='#mon%s'>%s</a>\n", htmlize(mm4,(*xb)->name), (*xb)->name);
			}
			fprintf(ff, "</span>\n");
		}		
		fprintf(ff, "</div>\n\n");
       	if (*mb == &mb_golgithan) {
    		fprintf(ff,"<h2>Misc. Non-Critter monst_base Entrys</h2>\n\n");
	   	
       	};
       	if (*mb == &mb_meteor) {
    		fprintf(ff,"<h2>Item monst_base Entrys</h2>\n\n");
	   	
       	};
       	n++;   mb++;
   	}
   	
}

void dump_docs() {
    FILE *ff = fopen("mmam-pricesheet.html", "wb");
    if (ff) {
    	const int num_mclass = 9;
    	monst_t *mclass[num_mclass];
    	mclass[0] = new monst_t(&mb_astbase); 
    	mclass[1] = new monst_t(&mb_jmart); 
    	mclass[2] = new monst_t(&mb_arena); 
    	mclass[3] = new monst_t(&mb_fisec); 
    	mclass[4] = new monst_t(&mb_merchant);
		mclass[5] = new monst_t(&mb_merchant);
		mclass[6] = new monst_t(&mb_merchant);
		mclass[7] = new monst_t(&mb_orickcamp);
		mclass[8] = new monst_t(&mb_orickcamp);
		
		mclass[4]->trader_ui->price_mult = 2.5; 
		mclass[5]->trader_ui->price_mult = 1.2; 
		mclass[6]->trader_ui->price_mult = 5.0;
		free(mclass[7]->name); mclass[7]->name = strdup("Standard Sell Rate");
		mclass[7]->trader_ui->price_mult = 0.333;
		free(mclass[8]->name); mclass[8]->name = strdup("Orick Sell Rate");
		mclass[8]->trader_ui->price_mult = 0.5;
		char *notes = (char *) "This price sheet is generated directly from the game "
		"source code (v " VERSION ") from its internal tables.  The colors are advisory \n"
		"and mean that a seller could or could not sell an item according to the internal tables. \n"
		"Green means an organization could get that item to sell red means they \n"
		"should never have one in stock to sell you. Not listed are BFC and Trainers which deal \n"
		"in monsters and use their own calculations.  Merchants do not all sell at the same rate; the \n"
		"median high and low rates are listed.  Whether an merchant has an item or not is affected by \n"
		"how common it is, but all other sellers will typicality gave green items in stock. \n";
		
		fprintf(ff, "<h1>MMaM Price Sheets:</h1>\n\n%s\n<table border='1'>\n<tr>\n  <th>Name</th>\n  <th>Common</th>\n", notes);
		for (int i = 0; i < num_mclass; i++) {
			fprintf(ff, "  <th>%s<br /><span style='font-size: 60%%'>(x %0.2f)</span></th>\n", mclass[i]->name, mclass[i]->trader_ui->price_mult);
			
		}
		fprintf(ff,"</tr>\n");
		for (int L = 0; L < NUM_IVALU_LIST; L++ ) {
			fprintf(ff, "  <tr>\n    <td>%s</td>\n    <td style='text-align: right'>%i%%</td>\n", ivalu_list[L].nam, ivalu_list[L].common);
			
			for (int i = 0; i < num_mclass; i++) {
			    char *color = (char *)"#880000";
			    int can_have = (
					mclass[i]->base == &mb_jmart ? ivalu_list[L].jm & 1 :
					mclass[i]->base == &mb_astbase ? ivalu_list[L].jm & 2  :
					mclass[i]->base == &mb_arena  ? ivalu_list[L].jm & 4 :
					mclass[i]->base == &mb_fisec  ? ivalu_list[L].jm & 8 :
				    mclass[i]->base == &mb_merchant ? ivalu_list[L].common : 0xffff
				);
				if (can_have == 0xffff) color = "#aaaaaa"; 
				else  if (can_have) color = "#008800";  
				char scrap[255];
				int price = (int)(ivalu_list[L].cost * mclass[i]->trader_ui->price_mult);
				
				fprintf(ff, "    <td style='text-align: right; color:%s'>%s</td>\n", 
					color, comma_i(scrap, price) 
				);
			}
			fprintf(ff,"</tr>\n");
		}
		for (int i = 0; i < num_mclass; i++) {
			delete mclass[i];		
		}
		fprintf(ff, "</table>\n<div style='font-size:50%; color:#888888'> Generated by MMaM v " VERSION "	</div>\n");
		dump_critter_doc(ff);		
		fclose(ff);		    
    }
}  
