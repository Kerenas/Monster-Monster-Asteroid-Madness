#define NUM_RAINDROPS 200
#define JG_MID_TIME 1000
#define JG_MAX_SIZE 120
#define JG_RAD_PER_MS 0.0080

enum {wth_none, wth_rain, wth_meteorids};

typedef class weather_t {
	private:
	struct {int x; int y;} rain[NUM_RAINDROPS];
	void init_rain();
	void step_rain();
	public:
	int weather_idx, severity, rain_color;
	float wind_x, wind_y;
	class asteroid_t *my_ast;
	weather_t(asteroid_t *aa, int wx = wth_none, int ws = 5);
	void poll();
	void draw();
};

typedef class jg_particle_t {
	public:
	  float x,y, a;
	  int s_time, size, typ;
	  class asteroid_t *my_ast;
	  jg_particle_t(asteroid_t *aa, float sx, float sy, int styp);
	  void poll();
      void draw();
};


typedef class killer_particle_t {
    public:
	float x,y, vx,vy, a; int dry_time, str, size; BITMAP *pic; struct ivalu_t *t; PMASK mask;
	class asteroid_t *my_ast;  class monst_t *who_fired;
	killer_particle_t(asteroid_t *smy_ast, monst_t *swho_fired, float sx, float sy, float sa, ivalu_t *w);
	void spore_check();
    void spore_freeze();
	void poll();
	void draw();
};

