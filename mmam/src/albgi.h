#ifndef _albgi_h
#define _albgi_h 1

extern int DarkGray,LightGray,Green,Brown,Red,Blue,Cyan,Magenta,
           White,Black,LightCyan,Yellow,LightGreen,
           LightRed,LightBlue,LightMagenta,
           BluishPurple,Purple, GhostGreen;


#define dist(x,y,x2,y2) sqrt(((x)-(x2))*((x)-(x2)) + ((y)-(y2))*((y)-(y2)) )
#define z_dist(x,y) sqrt((x)*(x) + (y)*(y) )

void albgi_init();

#endif
