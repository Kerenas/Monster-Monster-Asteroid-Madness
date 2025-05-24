extern BITMAP *blood_pool[6];
void init_blood_pool();

typedef class blood_t {
	public:
	  int x,y, *color,vx,vy, dry_time, shape; BITMAP *pool; int pool_size; asteroid_t *my_ast;
	  blood_t(asteroid_t *a, monst_t *m, float ang, float vel, int amt, int alt_fluid = 0);
	  void poll();
          void draw();
};
