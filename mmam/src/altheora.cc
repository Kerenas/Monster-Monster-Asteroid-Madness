#ifdef STANDALONE
#   define USE_THEORA
#   ifndef _REENTRANT
#      define _REENTRANT
#   endif
#   include <stdio.h>
#   include <unistd.h>
#   include <stdlib.h>
#   include <string.h>
#   include <sys/time.h>
#   include <sys/types.h>
#   include <sys/stat.h>
#   include <math.h>
#   include <allegro.h>
#else
#   include "mmam.h"
#endif
#include "altheora.h"
#ifndef USE_OGG
#   undef USE_THEORA
#endif
#ifndef USE_THEORA
int play_movie(char *fn, frame_time *tlist, int flags) { return 0; }
#else

#include <theora/theora.h>
#include <vorbis/codec.h>


/* Helper; just grab some more compressed bitstream and sync it for
   page extraction */
static int theora_buffer_data(PACKFILE *in, ogg_sync_state *oy){
	char *buffer=ogg_sync_buffer(oy,4096);
	int bytes=pack_fread(buffer,4096,in);
	ogg_sync_wrote(oy,bytes);
	return(bytes);
}

/* never forget that globals are a one-way ticket to Hell */
/* Ogg and codec state for demux/decode */
static ogg_sync_state   oy;
static ogg_page         og;
static ogg_stream_state vo;
static ogg_stream_state to;
static theora_info      ti;
static theora_comment   tc;
static theora_state     td;
static vorbis_info      vi;
static vorbis_dsp_state vd;
static vorbis_block     vb;
static vorbis_comment   vc;

static int              theora_p=0;
static int              vorbis_p=0;
static int              stateflag=0;

static BITMAP *theora_buffer;
int quit_now = 0;
#ifdef STANDALONE
volatile int ms_count;
#define VOLUME 127
int enable_sound = 1;
#endif

/* single frame video theora_buffering */
static int          videobuf_ready=0;
static ogg_int64_t  videobuf_granulepos=-1;
static double       videobuf_time=0;

/* single audio fragment audio theora_buffering */
static int          audiobuf_fill=0;
static int          audiobuf_ready=0;
static ogg_int16_t *audiobuf;
static ogg_int64_t  audiobuf_granulepos=0; /* time position of last sample */

/* audio / video synchronization tracking:

Since this will make it to Google at some point and lots of people
search for how to do this, a quick rundown of a practical A/V sync
strategy under Linux [the UNIX where Everything Is Hard].  Naturally,
this works on other platforms using OSS for sound as well.

In OSS, we don't have reliable access to any precise information on
the exact current playback position (that, of course would have been
too easy; the kernel folks like to keep us app people working hard
doing simple things that should have been solved once and abstracted
long ago).  Hopefully ALSA solves this a little better; we'll probably
use that once ALSA is the standard in the stable kernel.

We can't use the system clock for a/v sync because audio is hard
synced to its own clock, and both the system and audio clocks suffer
from wobble, drift, and a lack of accuracy that can be guaranteed to
add a reliable percent or so of error.  After ten seconds, that's
100ms.  We can't drift by half a second every minute.

Although OSS can't generally tell us where the audio playback pointer
is, we do know that if we work in complete audio fragments and keep
the kernel theora_buffer full, a blocking select on the audio theora_buffer will
give us a writable fragment immediately after playback finishes with
it.  We assume at that point that we know the exact number of bytes in
the kernel theora_buffer that have not been played (total fragments minus
one) and calculate clock drift between audio and system then (and only
then).  Damp the sync correction fraction, apply, and walla: A
reliable A/V clock that even works if it's interrupted. */

static long         audiofd_totalsize=-1;
static int          audiofd_fragsize;      /* read and write only complete fragments
                                       so that SNDCTL_DSP_GETOSPACE is
                                       accurate immediately after a bank
                                       switch */
static int          audiofd=-1;
static ogg_int64_t  audiofd_timer_calibrate=-1;

static void open_video() {
	theora_buffer = create_bitmap(ti.width, ti.height);
}

static void close_video() {
	if (theora_buffer) destroy_bitmap(theora_buffer); theora_buffer = NULL;
}

static void open_audio() {
}

static void close_audio() {
}

/* call this only immediately after unblocking from a full kernel
   having a newly empty fragment or at the point of DMA restart */
static void audio_calibrate_timer(int restart) {
  ogg_int64_t current_sample;
  ogg_int64_t new_time = ms_count;
  if(restart){
    current_sample=audiobuf_granulepos-audiobuf_fill/2/vi.channels;
  }else
    current_sample=audiobuf_granulepos-
      (audiobuf_fill+audiofd_totalsize-audiofd_fragsize)/2/vi.channels;
  new_time-=1000*current_sample/vi.rate;
  audiofd_timer_calibrate=new_time;
}

/* get relative time since beginning playback, compensating for A/V
   drift */
double get_time(){
  static ogg_int64_t last=0;  static ogg_int64_t up=0;
  ogg_int64_t now = ms_count;
  if(audiofd_timer_calibrate==-1)audiofd_timer_calibrate=last=now;
  if(audiofd<0){
    // no audio timer to worry about, we can just use the system clock
    // only one complication: If the process is suspended, we should
    //   reset timing to account for the gap in play time.  Do it the
    //   easy/hack way
    if(now-last>1000)audiofd_timer_calibrate+=(now-last);
    last=now;
  }
  if(now-up>200){
/*
    double timebase=(now-audiofd_timer_calibrate)*.001;
    int hundredths=timebase*100-(long)timebase*100;
    int seconds=(long)timebase%60;
    int minutes=((long)timebase/60)%60;
    int hours=(long)timebase/3600;

    printf("   Playing: %d:%02d:%02d.%02d                       \r",
            hours,minutes,seconds,hundredths);
*/
    up=now;
  }
  return (now-audiofd_timer_calibrate)*.001;
}

/* write a fragment to the OSS kernel audio API, but only if we can
   stuff in a whole fragment without blocking */
void audio_write_nonblocking(void){
/*
  if(audiobuf_ready){
    audio_buf_info info;
    long bytes;

    ioctl(audiofd,SNDCTL_DSP_GETOSPACE,&info);
    bytes=info.bytes;
    if(bytes>=audiofd_fragsize){
      if(bytes==audiofd_totalsize)audio_calibrate_timer(1);

      while(1){
        bytes=write(audiofd,audiobuf+(audiofd_fragsize-audiobuf_fill),
                    audiofd_fragsize);

        if(bytes>0){

          if(bytes!=audiobuf_fill){
            // shouldn't actually be possible... but eh
            audiobuf_fill-=bytes;
          }else
            break;
        }
      }

      audiobuf_fill=0;
      audiobuf_ready=0;

    }
  }
*/
}

static int good_tables;
static float Y_Table[256];
static float Cr_Table1[256];
static float Cb_Table1[256];
static float Cr_Table2[256];
static float Cb_Table2[256];
static unsigned char *clip_table;

		/* color allocation:
		 *
		 * translation to hi/true-color RGB is done by color look-up
		 * tables, using the algorithm:
		 *
		 *	R = Clip[(int)(Y_Table[Y] + Cr_Table1[Cr])];
		 *	G = Clip[(int)(Y_Table[Y] - Cb_Table1[Cb] - Cr_Table2[Cr])];
		 *	B = Clip[(int)(Y_Table[Y] + Cb_Table2[Cb])];
		 */
#define GET_R(y, cr)		(clip_table[(int)(Y_Table[(y)] + Cr_Table1[(cr)])])
#define GET_G(y, cb, cr)	(clip_table[(int)(Y_Table[(y)] - Cb_Table1[(cb)] - Cr_Table2[(cr)])])
#define GET_B(y, cb)		(clip_table[(int)(Y_Table[(y)] + Cb_Table2[(cb)])])

static void init_conv_tables() {
	// matrix coefficients
	if (good_tables) return;
	float crv = 104597.0f/65536.0f;
	float cbu = 132201.0f/65536.0f;
	float cgu = 25675.0f/65536.0f;
	float cgv = 53279.0f/65536.0f;
	static unsigned char Clp[1024];
	int i;
	clip_table = Clp + 384;
	for(i = -384;i < 640;i++) clip_table[i] = MID(0, i, 255);
	for(i = 0;i < 256;i++) {
		Y_Table[i] = ((i - 16) * 255.0 / 219.0) + 0.5;
		Cb_Table1[i] = (i - 128) * cgu;
		Cb_Table2[i] = (i - 128) * cbu;
		Cr_Table1[i] = (i - 128) * crv;
		Cr_Table2[i] = (i - 128) * cgv;
	}
	good_tables = 1;
}


#undef RENDER
#define RENDERER(BITS, TYP, SURFACE, CLEANUP) \
	case BITS: {                                                                                    \
  		for(int y=0; y<SURFACE->h; y++) {                                                       \
  			TYP *put = (TYP *)SURFACE->line[y];                                             \
  			unsigned char *Y = (unsigned char *)yuv.y + crop_offset+yuv.y_stride*y;         \
  			unsigned char *U = (unsigned char *)yuv.u + crop_offset2+yuv.uv_stride*(y / 2); \
  			unsigned char *V = (unsigned char *)yuv.v + crop_offset2+yuv.uv_stride*(y / 2); \
  			for (int x=0; x < SURFACE->w; x++) {                                                \
				int r = GET_R(*Y, *V);                                                  \
				int g = GET_G(*Y, *U, *V);                                              \
				int b = GET_B(*Y, *U);                                              \
				TYP o = (TYP)  makecol ## BITS(r, g, b);                                \
        			*put = o;  put++; Y++;                                                  \
                		if (x % 2) { U++;  V++; } CLEANUP;                                       \
     			}                                                                               \
		}                                                                                       \
        	break;                                                                                  \
	}
/* END of RENDER */

static void video_write() {
	yuv_buffer yuv;
  	int crop_offset, crop_offset2;
  	theora_decode_YUVout(&td,&yuv);
  	crop_offset=ti.offset_x+yuv.y_stride*ti.offset_y;
  	crop_offset2=(ti.offset_x / 2)+(yuv.uv_stride)*(ti.offset_y /  2);
  	switch (bitmap_color_depth(theora_buffer)) {
        RENDERER(16,unsigned short, theora_buffer,)
        RENDERER(32,unsigned int, theora_buffer,)
        RENDERER(15,unsigned short, theora_buffer,)
        RENDERER(24,unsigned int, theora_buffer, (char *)(put)--)
  	}
	stretch_blit(theora_buffer, screen, 0,0,theora_buffer->w, theora_buffer->h, 0,0, screen->w, screen->h);
}
#undef RENDER

/* dump the theora (or vorbis) comment header */
static int dump_comments(theora_comment *tc){
  int i, len;
  char *value;

  printf("Encoded by %s\n",tc->vendor);
  if(tc->comments){
    printf( "theora comment header:\n");
    for(i=0;i<tc->comments;i++){
      if(tc->user_comments[i]){
        len=tc->comment_lengths[i];
        value=(char *)malloc(len+1);
        memcpy(value,tc->user_comments[i],len);
        value[len]='\0';
        printf( "\t%s\n", value);
        free(value);
      }
    }
  }
  return(0);
}

/* Report the encoder-specified colorspace for the video, if any.
   We don't actually make use of the information in this example;
   a real player should attempt to perform color correction for
   whatever display device it supports. */
static void report_colorspace(theora_info *ti)
{
    switch(ti->colorspace){
      case OC_CS_UNSPECIFIED:
        /* nothing to report */
        break;;
      case OC_CS_ITU_REC_470M:
        printf("  encoder specified ITU Rec 470M (NTSC) color.\n");
        break;;
      case OC_CS_ITU_REC_470BG:
        printf("  encoder specified ITU Rec 470BG (PAL) color.\n");
        break;;
      default:
        printf("warning: encoder specified unknown colorspace (%d).\n",
            ti->colorspace);
        break;;
    }
}

/* helper: push a page into the appropriate steam */
/* this can be done blindly; a stream won't accept a page
                that doesn't belong to it */
static int queue_page(ogg_page *page){
  if(theora_p)ogg_stream_pagein(&to,&og);
  if(vorbis_p)ogg_stream_pagein(&vo,&og);
  return 0;
}

#ifdef STANDALONE

static void usage(void){
  printf(
          "Usage: altheora file.ogg\n"
          "  altheora uses allegro's packfile system so it can also read\n"
          "  from datafiles.   EX: altheora datafile.dat#SOME_OGG\n\n"
  );
}

void ms_counter() {
	if (!quit_now) {
		ms_count += 10;
	}
}

int create_window() {
	allegro_init();
	set_color_depth(16);
	if ( set_gfx_mode(  GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0 ) < 0 ) {
		allegro_message( "Could not set video mode %ix%i. Reason: %s\n", 800, 600, allegro_error );
		return 0;
	}
	set_window_title("allegro theora player example");
	set_color_conversion(COLORCONV_TOTAL);
	install_timer(); install_keyboard(); install_mouse();
	if ( install_sound(DIGI_AUTODETECT,  MIDI_NONE, "") == 0 ) {
		printf("Sound Detected...\n");
        	set_volume(VOLUME,0);
	} else {
 		allegro_message("Error initialising sound system\n%s\n", allegro_error);
		enable_sound = 0;
	}
	install_int(ms_counter, 10);
	return 1;
}
#endif

int play_movie(char *fn, frame_time *tlist, int flags) {
  int i,j;
  ogg_packet op;
  int tlist_idx = 0;
  int frame_idx = 0;
  PACKFILE *infile = pack_fopen(fn, "r");
  if (tlist) videobuf_time = 0.;//tlist[tlist_idx].ms / 1000.;
  
  if(infile==NULL) return 0;

  /*setup YUV => RGB conversion tables */
  init_conv_tables();

  /* start up Ogg stream synchronization layer */
  ogg_sync_init(&oy);

  /* init supporting Vorbis structures needed in header parsing */
  vorbis_info_init(&vi);
  vorbis_comment_init(&vc);

  /* init supporting Theora structures needed in header parsing */
  theora_comment_init(&tc);
  theora_info_init(&ti);

  /* Ogg PACKFILE open; parse the headers */
  /* Only interested in Vorbis/Theora streams */
  while(!stateflag){
    int ret=theora_buffer_data(infile,&oy);
    if(ret==0)break;
    while(ogg_sync_pageout(&oy,&og)>0){
      ogg_stream_state test;

      /* is this a mandated initial header? If not, stop parsing */
      if(!ogg_page_bos(&og)){
        /* don't leak the page; get it into the appropriate stream */
        queue_page(&og);
        stateflag=1;
        break;
      }

      ogg_stream_init(&test,ogg_page_serialno(&og));
      ogg_stream_pagein(&test,&og);
      ogg_stream_packetout(&test,&op);

      /* identify the codec: try theora */
      if(!theora_p && theora_decode_header(&ti,&tc,&op)>=0){
        /* it is theora */
        memcpy(&to,&test,sizeof(test));
        theora_p=1;
      }else if(!vorbis_p && vorbis_synthesis_headerin(&vi,&vc,&op)>=0){
        /* it is vorbis */
        memcpy(&vo,&test,sizeof(test));
        vorbis_p=1;
      }else{
        /* whatever it is, we don't care about it */
        ogg_stream_clear(&test);
      }
    }
    /* fall through to non-bos page parsing */
  }

  /* we're expecting more header packets. */
  while((theora_p && theora_p<3) || (vorbis_p && vorbis_p<3)){
    int ret;

    /* look for further theora headers */
    while(theora_p && (theora_p<3) && (ret=ogg_stream_packetout(&to,&op))){
      if(ret<0){
        printf("Error parsing Theora stream headers; corrupt stream?\n");
        exit(1);
      }
      if(theora_decode_header(&ti,&tc,&op)){
        printf("Error parsing Theora stream headers; corrupt stream?\n");
        exit(1);
      }
      theora_p++;
      if(theora_p==3)break;
    }

    /* look for more vorbis header packets */
    while(vorbis_p && (vorbis_p<3) && (ret=ogg_stream_packetout(&vo,&op))){
      if(ret<0){
        printf("Error parsing Vorbis stream headers; corrupt stream?\n");
        exit(1);
      }
      if(vorbis_synthesis_headerin(&vi,&vc,&op)){
        printf("Error parsing Vorbis stream headers; corrupt stream?\n");
        exit(1);
      }
      vorbis_p++;
      if(vorbis_p==3)break;
    }

    /* The header pages/packets will arrive before anything else we
       care about, or the stream is not obeying spec */

    if(ogg_sync_pageout(&oy,&og)>0){
      queue_page(&og); /* demux into the appropriate stream */
    }else{
      int ret=theora_buffer_data(infile,&oy); /* someone needs more data */
      if(ret==0){
        printf("End of file while searching for codec headers.\n");
        return 0;
      }
    }
  }

  /* and now we have it all.  initialize decoders */
  if(theora_p){
    theora_decode_init(&td,&ti);
    printf("Ogg logical stream %x is Theora %ix%i %.02f fps video\n",
           (int)to.serialno,(int)ti.width,(int)ti.height,
           (double)ti.fps_numerator/ti.fps_denominator);
    if(ti.width!=ti.frame_width || ti.height!=ti.frame_height)
      printf("  Frame content is %ix%i with offset (%i,%i).\n",
           ti.frame_width, ti.frame_height, ti.offset_x, ti.offset_y);
    report_colorspace(&ti);
    dump_comments(&tc);
  }else{
    /* tear down the partial theora setup */
    theora_info_clear(&ti);
    theora_comment_clear(&tc);
  }
  if(vorbis_p){
    vorbis_synthesis_init(&vd,&vi);
    vorbis_block_init(&vd,&vb);
    printf("Ogg logical stream %x is Vorbis %i channel %i Hz audio.\n",
            (int)vo.serialno,(int)vi.channels,(int)vi.rate);
  }else{
    /* tear down the partial vorbis setup */
    vorbis_info_clear(&vi);
    vorbis_comment_clear(&vc);
  }

  /* open audio */
  if(vorbis_p)open_audio();

  /* open video */
  if(theora_p)open_video();


  /* on to the main decode loop.  We assume in this example that audio
     and video start roughly together, and don't begin playback until
     we have a start frame for both.  This is not necessarily a valid
     assumption in Ogg A/V streams! It will always be true of the
     example_encoder (and most streams) though. */

  stateflag=0; /* playback has not begun */
  while(!quit_now){

    /* we want a video and audio frame ready to go at all times.  If
       we have to theora_buffer incoming, theora_buffer the compressed data (ie, let
       ogg do the theora_buffering) */
    while(vorbis_p && !audiobuf_ready){
      int ret;
      float **pcm;

      /* if there's pending, decoded audio, grab it */
      if((ret=vorbis_synthesis_pcmout(&vd,&pcm))>0){
        int count=audiobuf_fill/2;
        int maxsamples=(audiofd_fragsize-audiobuf_fill)/2/vi.channels;
        for(i=0;i<ret && i<maxsamples;i++)
          for(j=0;j<vi.channels;j++){
            int val=(int)(rint(pcm[j][i]*32767.f));
            if(val>32767)val=32767;
            if(val<-32768)val=-32768;
            audiobuf[count++]=val;
          }
        vorbis_synthesis_read(&vd,i);
        audiobuf_fill+=i*vi.channels*2;
        if(audiobuf_fill==audiofd_fragsize)audiobuf_ready=1;
        if(vd.granulepos>=0)
          audiobuf_granulepos=vd.granulepos-ret+i;
        else
          audiobuf_granulepos+=i;

      }else{

        /* no pending audio; is there a pending packet to decode? */
        if(ogg_stream_packetout(&vo,&op)>0){
          if(vorbis_synthesis(&vb,&op)==0) /* test for success! */
            vorbis_synthesis_blockin(&vd,&vb);
        }else   /* we need more data; break out to suck in another page */
          break;
      }
    }

    while(theora_p && !videobuf_ready){
      /* theora is one in, one out... */
      if(ogg_stream_packetout(&to,&op)>0){

        theora_decode_packetin(&td,&op);
        videobuf_granulepos=td.granulepos;

        if (tlist ) {
		frame_idx++;
		if (tlist[tlist_idx + 1].first == frame_idx) tlist_idx++;
		videobuf_time += (tlist[tlist_idx].ms / 1000.);
	} else {
		videobuf_time = theora_granule_time(&td,videobuf_granulepos);		
        }
        if(videobuf_time>=get_time()) videobuf_ready=1;
      } else break;
    }

    if(!videobuf_ready && !audiobuf_ready && pack_feof(infile))break;

    if(!videobuf_ready || !audiobuf_ready) {
      /* no data yet for somebody.  Grab another page */
      int ret=theora_buffer_data(infile,&oy);
      (void) ret;
      while (ogg_sync_pageout(&oy,&og)>0) {
        queue_page(&og);
      }
    }

    /* If playback has begun, top audio theora_buffer off immediately. */
    if (stateflag) audio_write_nonblocking();

    /* are we at or past time for this video frame? */
    if(stateflag && videobuf_ready && videobuf_time<=get_time()){
      video_write();
      videobuf_ready=0;
    }

    if(stateflag &&
       (audiobuf_ready || !vorbis_p) &&
       (videobuf_ready || !theora_p) &&
       !quit_now){
      /* we have an audio frame ready (which means the audio theora_buffer is
         full), it's not time to play video, so wait until one of the
         audio theora_buffer is ready or it's near time to play video */

      /* set up select wait on the audiotheora_buffer and a timeout for video */
#if 0
      struct timeval timeout;
      fd_set writefs;
      fd_set empty;
      int n=0;

      FD_ZERO(&writefs);
      FD_ZERO(&empty);
      if(audiofd>=0){
        FD_SET(audiofd,&writefs);
        n=audiofd+1;
      }

      if(theora_p){
        long milliseconds=(videobuf_time-get_time())*1000-5;
        if(milliseconds>500)milliseconds=500;
        if(milliseconds>0){
          timeout.tv_sec=milliseconds/1000;
          timeout.tv_usec=(milliseconds%1000)*1000;

          n=select(n,&empty,&writefs,&empty,&timeout);
          if(n)audio_calibrate_timer(0);
        }
      }else{
        select(n,&empty,&writefs,&empty,NULL);
      }
#endif
    }

    /* if our theora_buffers either don't exist or are ready to go,
       we can begin playback */
    if((!theora_p || videobuf_ready) &&
       (!vorbis_p || audiobuf_ready))stateflag=1;
    /* same if we've run out of input */
    if(pack_feof(infile))stateflag=1;
    if (key[KEY_ESC]) quit_now = 1;
  }

  /* tear it all down */

  close_audio();
  close_video();
  
  if(vorbis_p){
    ogg_stream_clear(&vo);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);
  }
  if(theora_p){
    ogg_stream_clear(&to);
    theora_clear(&td);
    theora_comment_clear(&tc);
    theora_info_clear(&ti);
  }
  ogg_sync_clear(&oy);

  pack_fclose(infile);

  printf("Done.\n");
  return 1;
}

#ifdef STANDALONE
int main(int argc,char *argv[]){
  if(argc==2){
    create_window();
    if (!play_movie(argv[1],0)) {
      printf("Unable to open '%s' for playback.\n", argv[1]);
      exit(1);
    }
  } else {
      usage();
      exit(1);
  }
  return(0);
}

END_OF_MAIN();
#endif

#endif // NO_THEORA


