#ifndef _xtrig_h
#define _xtrig_h 1
#define dist(x,y,x2,y2) sqrt(((x)-(x2))*((x)-(x2)) + ((y)-(y2))*((y)-(y2)) )
#define z_dist(x,y) sqrt((x)*(x) + (y)*(y) )
#ifndef M_PI
#  define M_PI    3.14159265358979323846
#endif
float shortest_arc(float a1, float a2);
float afix(float tang);
int quadratic(float A, float B, float C, float &z1, float &z2);
float lpquadratic(float A, float B, float C);

#endif
