#include "config.h"
#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include <adime.h>
#include "albgi.h"
#include "fonts.h"

extern BITMAP *buffer;


int DarkGray,LightGray,Green,Brown,Red,Blue,Cyan,Magenta,
    White,Black,LightCyan,Yellow,LightGreen,
    LightRed,LightBlue,LightMagenta,
    BluishPurple,Purple, GhostGreen;


#define MC(c,r,g,b) (c = makecol(0x ## r,0x ## g,0x ## b))

extern int pcolors[], playerc[];

void albgi_init() {
   MC(DarkGray,40,40,40);
   MC(LightGray,a0,a0,a0);
   MC(Green,1e,af,16);
   MC(Brown,a3,64,16);
   MC(Red,af,19,16);
   MC(Blue,16,3a,af);
   MC(Cyan,16,a5,af);
   MC(White,ff,ff,ff);
   MC(Yellow, e5,ff,21);
   MC(LightCyan,1e,ed,df);
   MC(Magenta,af,16,96);
   MC(LightBlue,29,1e,ed);
   MC(LightMagenta,ed,1e,ca);
   MC(BluishPurple,68,11,db);
   MC(LightRed,ed,1e,1e);
   MC(Purple,a3,23,ff);
   MC(GhostGreen,be,d3,9e);
   MC(LightGreen, 1e,ed,33);
   MC(Black,00,00,00);
#ifdef USE_ADIME
   #define ADC(ac,col) ( (adime_ ## ac ## _rgb.r = getr(col) ) | \
                         (adime_ ## ac ## _rgb.g = getg(col) ) | \
                         (adime_ ## ac ## _rgb.b = getb(col) ) )
   ADC(shadow, Blue);
   ADC(light_highlight, LightBlue);
   ADC(edit_field, GhostGreen);
   ADC(background,BluishPurple);
   ADC(button,BluishPurple);
   ADC(title_background,Purple);
   adime_font = &font_tiny;
   adime_title_font = &font_bold;
   adime_button_font = &font_bold;
#endif
}
