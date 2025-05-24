#ifndef _cage_h
#define _cage_h 1
typedef class cage_t {
  private:
	int x, y;
	int rattle_x, rattle_y, unrattle_time;
	float rattle_tilt;
	int spr_ok, spr_broken;
	float hp_rf;
	int  cryo_time, has_cryo, has_regen;
  public:
  	class monst_t *m;
  	int lvl, cage_num, hp, mhp;
        cage_t(int x, int y, int idx, struct ivalu_t *t);
	void upgrade(struct ivalu_t *t);
        void pack(monst_t *monst);
        monst_t *dump(int onto_ast =1);
        int  hit(int h);
        int  poll();
	void draw();
	int repair_cost() { return (int)(destroyed() ? 0 : ceil((mhp-hp) * hp_rf));};
        int destroyed() { return (hp <= 0);};
	int vacent() {return (m == NULL && hp > 0);};
	void repair() { if (!destroyed()) hp = mhp; }
	int frozen() { return (cryo_time &&  ms_count > cryo_time);}
	void save(PACKFILE *pf, int flags);
	int  parse(char *cmd, char *next, int line);	
};

#endif
