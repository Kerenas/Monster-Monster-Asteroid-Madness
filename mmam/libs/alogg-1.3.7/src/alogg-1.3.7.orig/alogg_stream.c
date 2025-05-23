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
#include <ctype.h>
#include <allegro.h>
#ifdef ALOGG_USE_TREMOR
#include <tremor/ivorbisfile.h>
#else
#include <vorbis/vorbisfile.h>
#endif
#ifdef ALOGG_USE_CURL
#include "aloggurl.h"
#endif
#include "alogg.h"
#include "aloggint.h"

#define BLOCK_SIZE 40960
#define ECHO_SIZE 20480
#define SHORT_BLOCK_SIZE (BLOCK_SIZE/2)
#define SHORT_ECHO_SIZE (ECHO_SIZE/2)

static char data[BLOCK_SIZE+ECHO_SIZE];
static unsigned short *sdata=(unsigned short*)data;
static unsigned short *sread_data=(unsigned short*)(data+ECHO_SIZE);

static void usage()
{
  fprintf(stderr,"alogg_stream [--echo] <oggfile>\n");
  fprintf(stderr,"  Try alogg_stream foo.dat#oggstream\n");
#ifdef ALOGG_USE_CURL
  fprintf(
    stderr,"  For HTTP streaming, try alogg_stream http://foo.net/bar.ogg\n"
  );
#endif
}

static void display_info(struct alogg_stream *stream)
{
  if (stream) {
    OggVorbis_File *ovf=alogg_get_vorbis_file(stream);
    if (ovf) {
      /* Get title and bitstream, and display it */
      int bitstream=ovf->current_link;
      static int current_bitstream=-1;
      if (bitstream!=current_bitstream) {
        vorbis_comment *comment=ov_comment(ovf,bitstream);
        current_bitstream=bitstream;
        if (comment) {
          int n;
          for (n=0;n<comment->comments;++n) {
            char *text=comment->user_comments[n];
            if (text) {
              int found=1,c,count=strlen("title=");
              for (c=0;c<count;++c) {
                if (tolower(text[c])!="title="[c]) {
                  found=0;
                  break;
                }
              }
              if (found) {
                printf("%s\n",strchr(text,'=')+1);
                break;
              }
            }
          }
        }
      }
    }
  }
}

int main(int argc,char **argv)
{
  struct alogg_stream *stream;
  int ret,n;
  int filename_index=1;
  int do_echo=0;
  int eos=0;

  if (argc==1) {
    usage();
    exit(1);
  }

  if (argc>2) {
    for (n=1;n<argc-1;++n) {
      if (!strcmp(argv[n],"--echo")) {
        do_echo=1;
        filename_index=n+1;
        /* fill the start of the echo buffer with silence */
        for (n=0;n<SHORT_ECHO_SIZE;++n) sdata[n]=0x8000;
      }
      else {
        usage();
        exit(1);
      }
    }
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

  if (strstr(argv[filename_index],"://")) {
#ifdef ALOGG_USE_CURL
    stream=alogg_start_streaming_url(argv[filename_index],NULL,BLOCK_SIZE);
#else
    fprintf(stderr,"alogg was not compiled with libcurl support\n");
    stream=NULL;
#endif
  }
  else {
    stream=alogg_start_streaming(argv[filename_index],BLOCK_SIZE);
  }
  if (!stream) {
    fprintf(stderr,"Error opening %s\n",argv[filename_index]);
    alogg_exit();
    exit(1);
  }

  /* stream data */
  do {
    ret=1;
    if (do_echo) {
      /* the manual way */
      AUDIOSTREAM *audio_stream=alogg_get_audio_stream(stream);
      void *block=get_audio_stream_buffer(audio_stream);
      if (block) {
        /* read a chunk of data, add echo, then play it */
        unsigned short *sblock=(unsigned short*)block;
        ret=alogg_read_stream_data(stream,sread_data,BLOCK_SIZE);
        if (eos) break;
        if (ret==0) eos=1;
        for (n=0;n<SHORT_BLOCK_SIZE;++n) {
          int echo=sdata[n]/4;
          sblock[n]=MID(0,sread_data[n]+echo,65535);
        }
        free_audio_stream_buffer(audio_stream);
        /* move data (we could use a ring buffer to avoid this) */
        memmove(sdata,sread_data,BLOCK_SIZE);
      }
    }
    else {
      /* the simple automagic way */
      ret=alogg_update_streaming(stream);
      if (ret==0) {
        /* end of stream */
        break;
      }
    }
    if (ret<0) {
      fprintf(stderr,"Error reading stream\n");
      alogg_exit();
      exit(1);
    }
    display_info(stream);
  } while (1);

  alogg_stop_streaming(stream);

  alogg_exit();
  return 0;
}
END_OF_MAIN()
