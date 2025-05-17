#include <allegro.h>
#include "namelist.h"
#include "rand.h"

using namespace std;


void name_gen::kill() {
	char *last = NULL;	
	for (vector<char *>::iterator i = fnlist.begin(); i != fnlist.end(); ++i) {
		if (*i != last) { free(*i); last = *i; }
	}
	for (vector<char *>::iterator i = lnlist.begin(); i != lnlist.end(); ++i) {
		if (*i != last) { free(*i); last = *i; }
	}
	for (vector<char *>::iterator i = prelist.begin(); i != prelist.end(); ++i) {
		if (*i != last) { free(*i); last = *i; }
	}
	for (vector<char *>::iterator i = postlist.begin(); i != postlist.end(); ++i) {
		if (*i != last) { free(*i); last = *i; }
	}
	fnlist.clear();
	lnlist.clear();
	prelist.clear();
	postlist.clear();
}

void name_gen::gnaw_off_flags(char *buf, int nix) {
	char *s = buf;
	sift_clean = 0;
	gender[nix] = 0;
	ageflags[nix] = 0;
	race[nix] = 0;
	racestrict[nix] = 0;
	for (; *s; ++s) if (*s == ':') {
		*s = 0; s++; break;
	}
	for (; *s; ++s) switch (*s) {
	case 'W': race[nix] |= RS_WHITE;    break;	 
	case 'B': race[nix] |= RS_BLACK;    break;	 
	case 'H': race[nix] |= RS_HISPANIC; break;	 
	case 'A': race[nix] |= RS_ASIAN;    break;	 
	case 'R': race[nix] |= RS_ARABIC;   break;	 
	case 'I': race[nix] |= RS_INDIA;    break;	 
	case 'm': gender[nix] |= RS_MALE;    break;	 
	case 'f': gender[nix] |= RS_FEMALE;    break;	 
	case 's': racestrict[nix] |= RS_STRICT; break;
	case 'y': ageflags[nix] |= RS_YOUNG; break;
	case 'e': ageflags[nix] |= RS_ELDERLY; break;
	}
}

void name_gen::sift_flags() {
	if (sift_clean) return;
	if ( gender[RS_FIRSTNAME] == RS_ANY_GENDER ) {
		gender[RS_FIRSTNAME] = RS_MALE + irand(2);
	}
	race[RS_FIRSTNAME] &= RS_ANY_RACE;
	if (!race[RS_FIRSTNAME]) race[RS_FIRSTNAME] = RS_ANY_RACE;
	while (1) {
		int mask = RS_WHITE << irand(6);
		if (race[RS_FIRSTNAME] & mask) { race[RS_FIRSTNAME] = mask; break; }
	}
	racestrict[RS_FIRSTNAME] = 0;
	age = 18 + irand(73);
	sift_clean = 1; 
	return;
}



int name_gen::load(char *fn, char *species, int append_mode) {
	int state = 0;
	if (append_mode == 0) if (fnlist.size() && lnlist.size()) return 1;
	if (PACKFILE *pf = pack_fopen(fn, "r")) {
		while (!pack_feof(pf)) {
			char scrap[200], *s = scrap;
			pack_fgets(scrap, sizeof scrap, pf);
			while (*s == ' ' || *s == '\t') s++;
			int clones = 0;
			for (char *s2 = s; *s2; s2++) {
				if (*s2 == '#') { 
					*s2 = 0; break;  
				} else if ((*s2 >= '0') && (*s2 <= '9')) {
					clones = *s; clones -= '0'; 
				}
			}
			if (*s == '=') {
				char *spix = strstr(s,"."); if (spix) ++spix;
				if (!spix || strcmp(spix, species ? species : "")) {
					if (strncmp(s,"=firstname",10)     == 0) state = 1;
					else if (strncmp(s,"=lastname",9)  == 0) state = 2;
					else if (strncmp(s,"=prename",8)   == 0) state = 3;
					else if (strncmp(s,"=postname",9)  == 0) state = 4;
					else if (strncmp(s,"=syntax", 7)   == 0) state = 5;
					else if (strncmp(s,"=include", 9) == 0) state = 6;
					else if (strncmp(s,"=done", 5)     == 0) {state = 0; return (fnlist.size() && lnlist.size());}
				} else state = 0;
			} else if (state && *s) {
				if (state <= 4) {
					char *ns = strdup(s);
					
					for (int i = clones + 1; i; --i) {
						switch (state) {
						case 1: fnlist.push_back(ns);   break;
						case 2: lnlist.push_back(ns);   break;
						case 3: prelist.push_back(ns);  break;
						case 4: postlist.push_back(ns); break;
						}
					}
				} if (state == 5);
				else if (state == 6) {
					char ffn[512];
					strcpy(ffn,fn);
					char *oo = strstr(ffn, "#");
					if (oo) {
						for (++oo; *s; ++s, ++oo ) *oo = (*s == '.' ? '_' : (*s >= 'a' && *s <= 'z') ? ('A' + (*s - 'a')) : *s);
						*oo = 0;
					} else strcpy(ffn, s);
					load(ffn, species, append_mode);
				}
			} 
		}
		pack_fclose(pf);
		return (fnlist.size() && lnlist.size() ? 1 : 0);
	}
	return 0;
}


char *name_gen::get_firstname(char *buf) {
	strcpy(buf, fnlist.size() ? fnlist[irand(fnlist.size())] : "Default: WBAIRfm");
	gnaw_off_flags(buf, RS_FIRSTNAME);
	return buf;
}


char *name_gen::get_lastname(char *buf) {
	strcpy(buf, lnlist.size() ? lnlist[irand(lnlist.size())] : "Defaultson:");
	gnaw_off_flags(buf, RS_LASTNAME);
	return buf;
}


char *name_gen::get_fullname(char *buf) {
	while (1) {
		get_firstname(buf);
		char *s = buf + strlen(buf);
		*s = ' '; ++s;
		get_lastname(s);
		/*
		if ((!racestrict[RS_FIRSTNAME]) && (!race[RS_LASTNAME]) && (irand(40) == 0)) {
			race[RS_FIRSTNAME] = RS_ANY_RACE;
		}
		*/
		if (!race[RS_LASTNAME]) break;
		if (!racestrict[RS_FIRSTNAME]) {
			if (irand(3) == 0) race[RS_FIRSTNAME] |= race[RS_LASTNAME];
			else race[RS_FIRSTNAME] = race[RS_LASTNAME];
			break;
		}
		if ( race[RS_LASTNAME] & race[RS_FIRSTNAME]) {
			race[RS_FIRSTNAME] &= race[RS_LASTNAME];
			break;
		}
	}
	return buf;
}


int name_gen::get_last_gender() {
	sift_flags();
	return gender[RS_FIRSTNAME];
}


int name_gen::get_last_race() {
	sift_flags();
	return race[RS_FIRSTNAME];
}


int name_gen::get_last_age() {
	sift_flags();
	return age;
}


char *name_gen::get_last_race_str() {
	switch (get_last_race()) {
	case RS_WHITE: return (char *)"white";	
	case RS_BLACK: return (char *)"black";	
	case RS_ASIAN: return (char *)"asian";	
	case RS_HISPANIC: return (char *)"hispanic";	
	case RS_INDIA: return (char *)"indian";	
	case RS_ARABIC: return (char *)"middle eastern";	
	}
	return (char *)"race unknown";
}

char *name_gen::get_last_gender_str() {
	switch (get_last_gender()) {
	case RS_MALE: return (char *)"male";	
	case RS_FEMALE: return (char *)"female";	
	}
	return (char *)"sex unknown";
}

char *name_gen::get_fullname_for(char *buf, int want_gender, int want_race, int want_age) {
	if (want_gender == 0) want_gender = RS_ANY_GENDER;
	if (want_race == 0) want_race = RS_ANY_GENDER;
	while (1) {
		get_fullname(buf);
		gender[RS_FIRSTNAME] &= want_gender;
		race[RS_FIRSTNAME] &= want_race;
		if (gender[RS_FIRSTNAME] & race[RS_FIRSTNAME]) {
			sift_flags();
			age = want_age;
			break;
		}
	}
	return buf;
}

char *name_gen::level_up_name(char *buf, int lvl, int req_gender) {
	char hs[512];
	if (lvl == 1) {  
		strcpy(buf, lnlist.size() ? lnlist[irand(lnlist.size())] : "Default: WBAIRfm");
		gnaw_off_flags(buf, RS_FIRSTNAME);	
	} else if (lvl == 2) {
		strcpy(hs, buf);		  
		strcpy(buf, fnlist.size() ? fnlist[irand(fnlist.size())] : "Default: WBAIRfm");
		gnaw_off_flags(buf, RS_FIRSTNAME);	
		strcat(buf, " ");
		strcat(buf, hs);			
	} else if (lvl == 4) {
		strcat(buf, " ");
		strcat(buf, postlist.size() ? postlist[irand(postlist.size())] : "Default: WBAIRfm");
		gnaw_off_flags(buf, RS_FIRSTNAME);	
	} else {
		strcpy(hs, buf);		  
		strcpy(buf, prelist.size() ? prelist[irand(prelist.size())] : "Default: WBAIRfm");
		gnaw_off_flags(buf, RS_FIRSTNAME);	
		strcat(buf, " ");
		strcat(buf, hs);
	} 	
	return buf;
}

#ifdef STANDALONE
int main(int argc, char *argv[]) {
	allegro_init();
	do_randomize();
	name_gen ng;
	ng.load((char *)"../data/humannames.conf");
	for (int c = 0; c < 200; c++) {
		char scrap[200];
		printf("%s is a ", ng.get_fullname(scrap));
		printf ("%iyr old %s %s.\n",			 
			ng.get_last_age(), 
			ng.get_last_race_str(),
			ng.get_last_gender_str()
		); 
	} 
}
END_OF_MAIN();
#endif
