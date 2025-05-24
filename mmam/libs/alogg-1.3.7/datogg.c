/* Copyright (C) 2002, 2003 Vincent Penquerc'h.
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

#include <stdio.h>
#include <string.h>

#ifdef ALOGG_USE_TREMOR
#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>
#else
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#endif

#include "allegro.h"
#ifndef ALOGG_USE_TREMOR
#include "allegro/internal/aintern.h"
#endif
#include "../datedit.h"

#include <alogg/aloggint.h>
#include <alogg/alogg.h>



#ifndef ALOGG_USE_TREMOR

/* Settings dialog adapted from Angelo Mottola's JPGAlleg library */

static char quality_string[64] = "Quality: 0.5 (medium)";
static int quality_cb(void*,int);

#define QUALITY_TEXT_INDEX 2
#define QUALITY_VALUE_INDEX 3

static DIALOG settings_dialog[] = {
 /* proc               x    y    w    h    fg   bg   key  flags      d1   d2    dp    dp2   dp3 */
  { d_shadow_box_proc, 0,   0,   292, 90,  0,   0,   0,   0,         0,   0,    NULL, NULL, NULL },
  { d_text_proc,       40,  10,  260, 8,   0,   0,   0,   0,         0,   0,    "Ogg/Vorbis import settings", NULL, NULL },
  { d_text_proc,       16,  30,  260, 8,   0,   0,   0,   0,         0,   0,    quality_string, NULL, NULL },
  { d_slider_proc,     16,  40,  260, 16,  0,   0,   0,   0,         100, 50,   NULL, quality_cb, NULL },
  { d_button_proc,     106, 60,  81,  17,  0,   0,   0,   D_EXIT,    0,   0,    "Ok", NULL, NULL },
  { d_yield_proc,      0,   0,   0,   0,   0,   0,   0,   0,         0,   0,    NULL, NULL, NULL },
  { NULL,              0,   0,   0,   0,   0,   0,   0,   0,         0,   0,    NULL, NULL, NULL }
}; 

static int quality_cb(void *dp3,int d2)
{
   int q=d2;
   snprintf(
     quality_string,sizeof(quality_string),
     "Quality: %01d.%02d (%s)     ",
     q==100?1:0,
     q%100,
     q<20?"very low":q<40?"low":q<65?"medium":q<90?"high":"very high"
   );
   object_message(&settings_dialog[QUALITY_TEXT_INDEX],MSG_DRAW,0);
   return D_O_K;
}

static int datogg_settings_proc()
{
   set_dialog_color(settings_dialog, gui_fg_color, gui_bg_color);
   centre_dialog(settings_dialog);
   popup_dialog(settings_dialog, -1);
   return D_REDRAW;
}

static MENU datogg_settings_menu =
{
   "Ogg/Vorbis import settings",
   datogg_settings_proc,
   NULL,
   0,
   0
};

#endif


/* Used to keep track of Ogg/Vorbis streaming from the grabber interface */
static struct alogg_stream *global_datogg_stream=NULL;

/* updates streaming of the currently playing stream, if any */
static void global_stream_updater()
{
   if (global_datogg_stream) {
      if (!alogg_update_streaming(global_datogg_stream)) {
         alogg_stop_streaming(global_datogg_stream);
         global_datogg_stream=NULL;
      }
   }
}

/* stops streaming - called liberally */
static void stop_streaming()
{
   if (global_datogg_stream) {
      remove_int(&global_stream_updater);
      alogg_stop_streaming(global_datogg_stream);
      global_datogg_stream=NULL;
   }
}



/* creates a new Ogg/Vorbis object */
static void *makenew_ogg(long *size)
{
   datogg_object *data=malloc(sizeof(datogg_object));
   if (data) {
      data->size=0;
      data->position=0;
      data->allocated=0;
      data->data=NULL;
   }
   return data;
}



/* displays an Ogg/Vorbis object in the grabber object view window */
static void plot_ogg(AL_CONST DATAFILE *dat, int x, int y)
{
#if ALLEGRO_VERSION*0x10000+ALLEGRO_SUB_VERSION*0x100+ALLEGRO_WIP_VERSION>=0x040104
   textout_ex(screen, font, "Double-click in the item list to play it", x, y+32, gui_fg_color, gui_bg_color);
#else
   textout(screen, font, "Double-click in the item list to play it", x, y+32, gui_fg_color);
#endif
}



/* handles double-clicking on an Ogg/Vorbis object in the grabber */
static int dclick_ogg(DATAFILE *dat)
{
   datogg_object *ovd=(datogg_object*)dat->dat;
   if (!ovd) return D_O_K;
   stop_streaming();
   install_int_ex(&global_stream_updater,BPS_TO_TIMER(20));
   global_datogg_stream=aloggint_start_streaming_datogg(ovd,4096);
   return D_O_K;
}



/* returns an information string describing an Ogg/Vorbis object */
static void get_ogg_desc(AL_CONST DATAFILE *dat, char *s)
{
   int ret;
   int channels,freq;
#ifdef ALOGG_USE_TREMOR
   ogg_int64_t length;
#else
   float length;
#endif
   datogg_object *ovd;

   ASSERT(dat);
   ASSERT(s);
   ASSERT(dat->type==DAT_OGG_VORBIS);
   ovd=(datogg_object*)dat->dat;
   ASSERT(ovd);
   if (ovd->data!=NULL) {
      ret=aloggint_get_info(ovd,&channels,&freq,&length);
      if (ret==0) {
        sprintf(
          s,"Ogg/Vorbis stream, %d channels, %d Hz, %2.2f s",
#ifdef ALOGG_USE_TREMOR
          channels,freq,length/1000.0f
#else
          channels,freq,length
#endif
        );
      }
      else {
         sprintf(s,"Invalid Ogg/Vorbis stream");
      }
   }
   else {
      sprintf(s,"Empty Ogg/Vorbis stream");
   }
}



/* exports an Ogg/Vorbis stream into an external file */
static int export_ogg(AL_CONST DATAFILE *dat, AL_CONST char *filename)
{
   datogg_object *ovd;
   PACKFILE *f;

   ASSERT(dat);
   ASSERT(filename);

   ovd=(datogg_object*)dat->dat;
   ASSERT(ovd);
   f=pack_fopen(filename,F_WRITE);
   if (!f) return 0;
   pack_fwrite(ovd->data,ovd->size,f);
   pack_fclose(f);
   return (errno==0);
}



#ifndef ALOGG_USE_TREMOR

/* encodes a sample to an Ogg/Vorbis stream */
static void grab_ogg_writer(void *data,size_t bytes,datogg_object *ovd)
{
   ASSERT(data);
   ASSERT(ovd);
   if (ovd->size+bytes>ovd->allocated) {
      ovd->allocated=ovd->allocated+bytes+(ovd->allocated)/2;
      ovd->data=realloc(ovd->data,ovd->allocated);
   }
   memcpy(ovd->data+ovd->size,data,bytes);
   ovd->size+=bytes;
}

static AL_CONST char *find_property(AL_CONST DATAFILE_PROPERTY *properties,int type)
{
  while (properties->type!=DAT_END) {
    if (properties->type==type) {
      if (!properties->dat) return empty_string;
      return properties->dat;
    }
    properties++;
  }
  return empty_string;
}

/* finds a suitable encoding quality setting */
static float find_quality(AL_CONST DATAFILE_PROPERTY *prop)
{
   if (prop) {
      AL_CONST char *value=find_property(prop,DAT_OGG_VORBIS_QUALITY);
      if (value && *value) {
         int n=atoi(value);
         n=MID(0,n,100);
         return n/100.0f;
      }
   }
   return settings_dialog[QUALITY_VALUE_INDEX].d2/100.0f;
}

/* encodes an Allegro SAMPLE to Ogg/Vorbis */
static datogg_object *encode_sample(AL_CONST SAMPLE *sample,float quality)
{
   datogg_object *ovd;
   struct alogg_encoding_data *data;
   int ret;

   ASSERT(sample);

   ovd=makenew_ogg(NULL);
   data=alogg_start_encoding(
     sample->stereo?2:1,sample->freq,quality,0,NULL,
     (void(*)(void*,size_t,unsigned long))&grab_ogg_writer,
     (unsigned long)ovd
   );
   if (!data) {
     free(ovd);
     return NULL;
   }
   ret=alogg_update_encoding(
     data,sample->data,sample->len,sample->stereo?2:1,sample->bits
   );
   ret=alogg_stop_encoding(data) && ret;
   if (!ret) {
     free(ovd);
     return NULL;
   }
   return ovd;
}

#endif

/* imports an Ogg/Vorbis stream from an external file */
#if ALLEGRO_VERSION*0x10000+ALLEGRO_SUB_VERSION*0x100+ALLEGRO_WIP_VERSION>=0x04010d
static DATAFILE *grab_ogg(int type, AL_CONST char *filename, DATAFILE_PROPERTY **prop, int depth)
#else
static void *grab_ogg(AL_CONST char *filename, long *size, int x, int y, int w, int h, int depth)
#endif
{
   datogg_object *ovd;
   PACKFILE *f;
   AL_CONST char *extension;

   ASSERT(filename);

   extension=get_extension(filename);
   if (extension && !stricmp(extension,"ogg")) {
     f=pack_fopen(filename,"r");
     if (!f) return NULL;
     ovd=makenew_ogg(NULL);
#if ALLEGRO_VERSION*0x10000+ALLEGRO_SUB_VERSION*0x100+ALLEGRO_WIP_VERSION>=0x040112
     long todo=f->normal.todo;
#else
     long todo=f->todo;
#endif
     ovd->size=todo;
     ovd->data=malloc(todo);
     if (!ovd->data) {
       pack_fclose(f);
       free(ovd);
       return NULL;
     }
     pack_fread(ovd->data,ovd->size,f);
     pack_fclose(f);
#if ALLEGRO_VERSION*0x10000+ALLEGRO_SUB_VERSION*0x100+ALLEGRO_WIP_VERSION>=0x04010d
     return datedit_construct(type,ovd,0,prop);
#else
     return ovd;
#endif
   }

#ifndef ALOGG_USE_TREMOR
   else {
      /* Try a non Ogg/Vorbis stream format */
      SAMPLE *sample=load_sample(filename);
      if (sample) {
#if ALLEGRO_VERSION*0x10000+ALLEGRO_SUB_VERSION*0x100+ALLEGRO_WIP_VERSION>=0x04010d
         DATAFILE *dat;
         float quality=find_quality(*prop);
         char qstr[8];
         ovd=encode_sample(sample,quality);
         destroy_sample(sample);
         dat=datedit_construct(type,ovd,0,prop);
         if (!dat) return NULL;
         snprintf(qstr,sizeof(qstr),"%d",(int)(quality*100.0f+0.5f));
         datedit_set_property(dat,DAT_OGG_VORBIS_QUALITY,qstr);
         return dat;
#else
         float quality=settings_dialog[QUALITY_VALUE_INDEX].d2/100.0f;
         ovd=encode_sample(sample,quality);
         destroy_sample(sample);
         return ovd;
#endif
      }
   }
#endif

   /* Could not load */
   return NULL;
}



/* saves an Ogg/Vorbis stream into the datafile format */
static
#if ALLEGRO_VERSION*0x10000+ALLEGRO_SUB_VERSION*0x100+ALLEGRO_WIP_VERSION>=0x040104
int save_ogg(DATAFILE *dat, AL_CONST int *fixed_prop, int pack, int pack_kids, int strip, int sort, int verbose, int extra, PACKFILE *f)
#else
void save_ogg(DATAFILE *dat, int packed, int packkids, int strip, int *keeplist, int sort, int verbose, int extra, PACKFILE *f)
#endif
{
   datogg_object *ovd;
   ASSERT(dat);
   ovd=(datogg_object*)dat->dat;
   ASSERT(ovd);
   pack_fwrite(ovd->data,ovd->size,f);
#if ALLEGRO_VERSION*0x10000+ALLEGRO_SUB_VERSION*0x100+ALLEGRO_WIP_VERSION>=0x040104
#if ALLEGRO_VERSION*0x10000+ALLEGRO_SUB_VERSION*0x100+ALLEGRO_WIP_VERSION>=0x040106
   return TRUE;
#else
   return 0;
#endif
#endif
}



static void *load_oggvorvis_object(PACKFILE *f,long size)
{
   datogg_object *ovd;

   ASSERT(f);

   ovd=malloc(sizeof(datogg_object));
   if (!ovd) return NULL;

   ovd->size=size;
   ovd->position=0;
   ovd->allocated=0;
   ovd->data=NULL;
   if (ovd->size) {
      ovd->data=malloc(ovd->size);
      pack_fread(ovd->data,ovd->size,f);
   }
   return ovd;
}

static void destroy_oggvorbis_object(void *data)
{
   datogg_object *ovd=(datogg_object*)data;
   if (ovd) {
      stop_streaming();
      if (ovd->data) free(ovd->data);
      free(ovd);
   }
}

#ifndef ALOGG_USE_TREMOR

static int encode_objects_worker(DATAFILE *dat, int *param, int param2)
{
   datogg_object *ovd;
   DATAFILE_PROPERTY *prop;

   ASSERT(dat);
   ASSERT(param);
   ASSERT(param2);

   if (dat->type == DAT_SAMPLE) {
      float quality=find_quality(dat->prop);
      ovd = encode_sample(dat->dat,quality);
      if (!ovd) {
         alert(
           "Error encoding sample:",get_datafile_property(dat,DAT_NAME),"",
           "OK",NULL,13,27
         );
         ++*(int*)param2;
         return 1;
      }
      prop = dat->prop;
      dat->prop = NULL;
      _unload_datafile_object(dat);
      dat->dat = ovd;
      dat->size = 0;
      dat->type = DAT_OGG_VORBIS;
      dat->prop = prop;
      ++*param;
   }

   return 0;
}

static int encode_objects()
{
   int count=0,param=0,errors=0;
   char msg1[64];
   char msg2[64];
   char msg3[64];

   grabber_foreach_selection(&encode_objects_worker,&count,&param,(int)&errors);
   grabber_rebuild_list(NULL,FALSE);
   snprintf(msg1,sizeof(msg1),"%d samples encoded",param);
   snprintf(msg2,sizeof(msg2),"%d errors",errors);
   snprintf(msg3,sizeof(msg3),"%d objects ignored",count-param-errors);
   alert(msg1,msg2,msg3,"OK",NULL,27,13);
   if (!param) return D_O_K;
#if ALLEGRO_VERSION*0x10000+ALLEGRO_SUB_VERSION*0x100+ALLEGRO_WIP_VERSION>=0x040101
   grabber_modified(TRUE);
#endif
   return D_REDRAW;
}

#endif

void datogg_init()
{
  register_datafile_object(
    DAT_OGG_VORBIS,&load_oggvorvis_object,&destroy_oggvorbis_object
  );
}


/* plugin interface header */
DATEDIT_OBJECT_INFO datogg_info =
{
   DAT_OGG_VORBIS,
   "Ogg/Vorbis stream", 
   get_ogg_desc,
   makenew_ogg,
   save_ogg,
   plot_ogg,
   dclick_ogg,
   NULL
};

DATEDIT_GRABBER_INFO datogg_grabber =
{
   DAT_OGG_VORBIS,
   "ogg;wav;voc",
   "ogg",
   grab_ogg,
   export_ogg,
#if ALLEGRO_VERSION*0x10000+ALLEGRO_SUB_VERSION*0x100+ALLEGRO_WIP_VERSION>=0x04010d
   "OGGQ",
#endif
};



#ifndef ALOGG_USE_TREMOR

DATEDIT_MENU_INFO datogg_settings_menu_info =
{
   &datogg_settings_menu,
   NULL,
   DATEDIT_MENU_FILE,
   0,
#if ALLEGRO_VERSION*0x10000+ALLEGRO_SUB_VERSION*0x100+ALLEGRO_WIP_VERSION>=0x04010d
   NULL,
#endif
};

static MENU datogg_encode_menu=
{
   "Encode to Ogg/Vorbis",
   &encode_objects,
   NULL,
   0,
   0
};

DATEDIT_MENU_INFO datogg_encode_menu_info =
{
   &datogg_encode_menu,
   NULL,
   DATEDIT_MENU_OBJECT,
   0,
#if ALLEGRO_VERSION*0x10000+ALLEGRO_SUB_VERSION*0x100+ALLEGRO_WIP_VERSION>=0x04010d
   NULL,
#endif
};

#endif

