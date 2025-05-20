/*
 *	ÇPROJECTNAMEÈ
 *
 *	Created by ÇFULLUSERNAMEÈ on ÇDATEÈ.
 *	Copyright (c) ÇYEARÈ ÇORGANIZATIONNAMEÈ. All rights reserved.
 */

#include <Allegro/allegro.h>
#include <Adime/adime.h>

int buttonrow_callback(DIALOG *d, int n)
{
	return n;
}

int main(int argc, const char *argv[])
{
	allegro_init();
	install_keyboard();
	install_mouse();
	
	if (set_gfx_mode(GFX_AUTODETECT, 320, 200, 0, 0)) {
		allegro_message("Error setting 320x200x8 gfx mode:\n%s\n", allegro_error);
		return -1;
	}
	
	adime_init();
	
	adime_lowlevel_dialogf("Message:", ADIME_ALIGN_CENTRE, ADIME_ALIGN_CENTRE, 100, 
			"Hello, world!%nothing[]%line[]%buttonrow[OK;CTRL+O;ENTER]", 
			buttonrow_callback);
	return 0;
}
END_OF_MAIN();
