#include <stdlib.h>
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
#include <unistd.h>
#include <time.h>
#include <allegro.h>
#include "alogg.h"
#include "aloggint.h"

#ifndef ALOGG_USE_TREMOR
int main(int argc,char **argv)
{
  SAMPLE *sample;
  char filename[PATH_MAX]="";
  int n;
  float quality=0.5f;
  char *title=NULL;

  srand(time(NULL));
  if (argc==1) {
    fprintf(stderr,"alogg_encode <arg1> <arg2>...\n");
    fprintf(stderr,"args can be:\n");
    fprintf(stderr," -q <float>   Set encoding quality (0 (low) to 1 (high)\n");
    fprintf(stderr," -t <string>  Set title of the next track(s)\n");
    fprintf(stderr," -o filename  Use this output filename\n");
    fprintf(stderr," filename     Encode this file\n");
    fprintf(stderr,"Encoding uses the last specified quality, left to right\n");
    fprintf(stderr,"Default quality is %.2g\n",quality);
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

  for (n=1;n<argc;++n) {
    if (!strcmp(argv[n],"-q")) {
      ++n;
      if (n==argc) {
        fprintf(stderr,"-q needs a float argument\n");
        exit(1);
      }
      quality=(float)atof(argv[n]);
      continue;
    }
    if (!strcmp(argv[n],"-t")) {
      ++n;
      if (n==argc) {
        fprintf(stderr,"-t needs a string argument\n");
        exit(1);
      }
      if (title) free(title);
      title=malloc(strlen(argv[n])+strlen("TITLE=")+1);
      sprintf(title,"TITLE=%s",argv[n]);
      continue;
    }
    if (!strcmp(argv[n],"-o")) {
      ++n;
      if (n==argc) {
        fprintf(stderr,"-o needs a string argument\n");
        exit(1);
      }
      strcpy(filename,argv[n]);
      continue;
    }

    sample=load_sample(argv[n]);
    if (!sample) {
      fprintf(stderr,"Error loading %s (%d)\n",argv[n],alogg_error_code);
      alogg_exit();
      exit(1);
    }

    if (!filename[0]) {
      replace_extension(filename,argv[n],"ogg",sizeof(filename));
    }

    printf("%s -> %s:\n",argv[n],filename);
    printf("%lu samples, %s\n",sample->len,sample->stereo?"stereo":"mono");
    if (title) printf("title: %s\n",strchr(title,'=')+1);

    if (!alogg_save_ogg_param(filename,sample,quality,title?1:0,&title)) {
      fprintf(stderr,"Failed to save %s\n",filename);
      destroy_sample(sample);
      alogg_exit();
      exit(1);
    }
    strcpy(filename,"");

    destroy_sample(sample);
  }

  if (title) free(title);

  alogg_exit();
  return 0;
}
#else
int main()
{
  printf("alogg_encode is not compiled when building alogg with Tremor\n");
  return 0;
}
#endif

END_OF_MAIN()
