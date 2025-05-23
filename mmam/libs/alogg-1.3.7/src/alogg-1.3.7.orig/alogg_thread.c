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
#include <string.h>
#include <sys/time.h>
#include <allegro.h>
#ifdef ALOGG_USE_CURL
#include "aloggurl.h"
#endif
#include "aloggpth.h"
#include "aloggint.h"

#define BLOCK_SIZE 40960

static void usage()
{
  fprintf(stderr,"alogg_thread <oggfile1> [<oggfile2>]...\n");
  fprintf(stderr,"  Try alogg_thread foo.dat#oggstream\n");
#ifdef ALOGG_USE_CURL
  fprintf(stderr,"  For HTTP streaming, try alogg_thread http://foo.net\n");
#endif
}

static struct alogg_thread *create_streaming_thread(char *filename)
{
  struct alogg_stream *stream;

  if (strstr(filename,"://")) {
#ifdef ALOGG_USE_CURL
    stream=alogg_start_streaming_url(filename,NULL,BLOCK_SIZE);
#else
    fprintf(stderr,"alogg was not compiled with libcurl support\n");
    stream=NULL;
#endif
  }
  else {
    stream=alogg_start_streaming(filename,BLOCK_SIZE);
  }
  if (!stream) {
    fprintf(stderr,"Error opening %s\n",filename);
    alogg_exit();
    exit(1);
  }

  return alogg_create_thread(stream);
}

int main(int argc,char **argv)
{
  struct alogg_thread **threads;
  int filename_index;

  if (argc==1) {
    usage();
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

  install_timer();
  if (install_sound(DIGI_AUTODETECT,MIDI_NONE,NULL)<0) {
    fprintf(stderr,"Failed to install sound: %s\n",allegro_error);
    alogg_exit();
    exit(1);
  }
  if (install_keyboard()<0) {
    fprintf(stderr,"Failed to install keyboard handler\n");
    alogg_exit();
    exit(1);
  }

  threads=malloc(sizeof(struct alogg_thread*)*(argc-1));
  for (filename_index=1;filename_index<argc;++filename_index) {
    /* start a thread to handle streaming */
    threads[filename_index-1]=create_streaming_thread(argv[filename_index]);
  }

  while (1) {
    if (keypressed()) {
      int c=readkey()&0xff;
      if (c==27) break;
      if (c>='1' && c<='9') {
        int index=c-'0';
        if (index>=1 && index<argc) {
          if (threads[index-1]) {
            struct alogg_thread *thread=threads[index-1];
            alogg_stop_thread(thread);
            while (alogg_is_thread_alive(thread));
            threads[index-1]=NULL;
          }
          else {
            threads[index-1]=create_streaming_thread(argv[index]);
          }
        }
      }
    }
  }

  for (filename_index=1;filename_index<argc;filename_index++) {
    if (threads[filename_index-1]) {
      struct alogg_thread *thread=threads[filename_index-1];
      alogg_stop_thread(thread);
      while (alogg_is_thread_alive(thread));
    }
  }
  free(threads);

  alogg_exit();
  return 0;
}
END_OF_MAIN()
