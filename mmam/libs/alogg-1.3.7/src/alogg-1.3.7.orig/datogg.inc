/* Copyright (C) 2002 Vincent Penquerc'h.
   This file is part of the alogg library.
   Written by Vincent Penquerc'h <lyrian -at- kezako -dot- net>.

   Allegro and the alogg grabber plugin are gift-ware. They were created by
   a number of people working in cooperation, and are given to you freely as
   a gift. You may use, modify, redistribute, and generally hack them about
   in any way you like, and you do not have to give us anything in return.
   However, if you like these products you are encouraged to thank us by
   making a return gift to the Allegro/alogg community. This could be by
   writing an add-on package, providing a useful bug report, making an
   improvement to the library and/or the plugin, or perhaps just releasing
   the sources of your program so that other people can learn from them.
   If you redistribute parts of this code or make a game using it, it
   would be nice if you mentioned Allegro and alogg somewhere in the
   credits, but you are not required to do this. We trust you not to abuse
   our generosity. */

/* registration code for setting up the Ogg/Vorbis object plugin */
{
   extern DATEDIT_OBJECT_INFO datogg_info;
   extern DATEDIT_GRABBER_INFO datogg_grabber;
#ifndef ALOGG_USE_TREMOR
   extern DATEDIT_MENU_INFO datogg_settings_menu_info;
   extern DATEDIT_MENU_INFO datogg_encode_menu_info;
#endif

   extern void datogg_init();
   datogg_init();

   datedit_register_object(&datogg_info);
   datedit_register_grabber(&datogg_grabber);
#ifndef ALOGG_USE_TREMOR
   datedit_register_menu(&datogg_settings_menu_info);
   datedit_register_menu(&datogg_encode_menu_info);
#endif
}

