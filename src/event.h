#ifndef _event_h
#define _event_h 1
typedef enum event {
  event_hippies, event_police, event_pit_fight,
  event_ufo, event_stalker_clan, event_weather_change, event_weather_change_r,
  event_merchant, event_tourists, event_raiders, event_hunters,
  event_meteor_storm,  event_golgithan, event_autoshot, event_last
};

typedef struct eventsced_t { int enabled, time, severity; };

typedef class event_t {
   private:
     eventsced_t event_list[event_last];
     int timealign(int time);
     int next;
     void calc_next();
   public:
     event_t() { clear(); };
     void clear();
     void set(int e, int in_seconds, int severity = 0);
     void set_sooner(int e, int in_seconds, int severity = 0);
     void unset(int e);
     void pop();
     void save(PACKFILE *pf, int flags);
     int  parse(char *cmd, char *next, int line);
};

extern event_t event;

#endif
