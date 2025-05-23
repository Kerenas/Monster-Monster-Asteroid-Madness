/* Copyright (C) 2002, 2003 Vincent Penquerc'h.
   This file is part of the alogg library.
   Written by Vincent Penquerc'h <lyrian -at- kezako -dot- net>.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   - Neither the name of Vincent Penquerc'h nor alogg nor Xiphophorus
   nor the names of their contributors may be used to endorse or promote
   products derived from this software without specific prior written
   permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#define USE_CONSOLE /* or win32 wouldn't have a console */
#define ALLEGRO_USE_CONSOLE /* newer version of this define */

#include <stdio.h>
#include <unistd.h>
#include <allegro.h>
#include "alogg.h"
#include "aloggint.h"

int main(int argc,char **argv)
{
  SAMPLE *sample;
  int voice;

  if (argc==1) {
    fprintf(stderr,"alogg_play <oggfile>\n");
    exit(1);
  }

#if ALLEGRO_IS_VERSION_OR_NEWER(4,1,0)
  if (allegro_init()) {
    fprintf(stderr,"Failed to install Allegro: %s\n",allegro_error);
    exit(1);
  }
#else
  allegro_init();
#endif
  set_display_switch_mode(SWITCH_BACKAMNESIA);
  alogg_init();
  sample=alogg_load_ogg(argv[1]);
  if (!sample) {
    fprintf(stderr,"Error loading %s (%d)\n",argv[1],alogg_error_code);
    alogg_exit();
    exit(1);
  }

  printf("%s:\n",argv[1]);
  printf("%lu samples, %s\n",sample->len,sample->stereo?"stereo":"mono");

  install_timer();
  if (install_sound(DIGI_AUTODETECT,MIDI_NONE,NULL)<0) {
    fprintf(stderr,"Failed to install sound: %s\n",allegro_error);
    alogg_exit();
    exit(1);
  }

  voice=allocate_voice(sample);
  if (voice==-1) {
    fprintf(stderr,"Unable to allocate a voice\n");
    alogg_exit();
    exit(1);
  }

  voice_start(voice);
  release_voice(voice);
  while (voice_check(voice)==sample);
  deallocate_voice(voice);

  destroy_sample(sample);

  alogg_exit();
  return 0;
}
END_OF_MAIN()
