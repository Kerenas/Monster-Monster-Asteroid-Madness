#ifndef _namelist_h
#define _namelist_h 1
#include <vector>

const int 
	RS_MALE = 0x0001, 
	RS_FEMALE = 0x0002, 
	RS_WHITE = 0x0004, 
	RS_BLACK = 0x0008, 
	RS_ASIAN = 0x0010,
	RS_ARABIC = 0x0020,
	RS_INDIA = 0x0040,
	RS_HISPANIC = 0x0080,
	RS_YOUNG = 0x0100,
	RS_ELDERLY = 0x0200,	
	RS_STRICT = 0x0400,
	RS_ANY_RACE = 0x00fc,	
	RS_ANY_GENDER = 0x0003,
	RS_FIRSTNAME = 0,
	RS_LASTNAME = 1;	


class name_gen {
private:
	std::vector<char *> fnlist;
	std::vector<char *> lnlist;
	std::vector<char *> prelist;
	std::vector<char *> postlist;
	int age; 
	int use_syntax_rules;
	int race[2], gender[2],	ageflags[2], racestrict[2];
	int sift_clean;
	void gnaw_off_flags(char *buf, int nix);
	void sift_flags();
public:
	int load(char *fn, char *species = NULL, int append_mode = 0);
	void kill();
	char *get_firstname(char *buf);
	char *get_lastname(char *buf);
	char *get_fullname(char *buf);
	char *level_up_name(char *buf, int lvl, int gender);	
	int get_last_gender();
	int get_last_race();
	int get_last_age();
	char *get_last_race_str();
	char *get_last_gender_str();
	char *get_fullname_for(char *buf, int want_gender, int want_race, int want_age);
};
#endif
