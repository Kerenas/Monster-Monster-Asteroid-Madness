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
#include <string.h>
#include <allegro.h>
#include "alogg.h"
#include "aloggint.h"

#define BLOCK_SIZE 4096

int main(int argc,char **argv)
{
  DATAFILE *dat;
  int play=0;

  if (argc!=4) {
    fprintf(stderr,"alogg_dat {--stream,--play} <datafile> <name>\n");
    exit(1);
  }
  if (!strcmp(argv[1],"--stream")) {
    play=0;
  }
  else if (!strcmp(argv[1],"--play")) {
    play=1;
  }
  else {
    fprintf(stderr,"alogg_dat {--stream,--play} <datafile> <name>\n");
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
  dat=load_datafile_object(argv[2],argv[3]);
  if (!dat || !dat->dat) {
    fprintf(stderr,"Error loading %s#%s\n",argv[2],argv[3]);
    exit(1);
  }
  fprintf(stderr,"%lu bytes\n",dat->size);

  install_timer();
  if (install_sound(DIGI_AUTODETECT,MIDI_NONE,NULL)<0) {
    fprintf(stderr,"Failed to install sound: %s\n",allegro_error);
    alogg_exit();
    exit(1);
  }

  if (play) {
    SAMPLE *sample=alogg_create_sample(dat);
    int voice=allocate_voice(sample);
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
  }
  else {
    struct alogg_stream *stream=alogg_start_streaming_datafile(dat,BLOCK_SIZE);
    if (!stream) {
      fprintf(stderr,"Error streaming %s#%s\n",argv[2],argv[3]);
      alogg_exit();
      exit(1);
    }
    while (1) {
      int ret=alogg_update_streaming(stream);
      if (ret==0) {
        /* end of stream */
        break;
      }
      if (ret<0) {
        fprintf(stderr,"Error reading stream\n");
        alogg_exit();
        exit(1);
      }
    }
    alogg_stop_streaming(stream);
  }

  unload_datafile_object(dat);

  alogg_exit();
  return 0;
}
END_OF_MAIN()
