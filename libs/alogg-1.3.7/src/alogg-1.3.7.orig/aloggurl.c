/* Copyright (C) 2002, 2003 Vincent Penquerc'h.
   This file is part of the alogg library.
   Written by Vincent Penquerc'h <lyrian -at- kezako -dot- net>.

   The alogg library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The alogg library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the alogg Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.

   As a special exception, if you link this library statically with a
   program, you do not have to distribute the object files for this
   program.
   This exception does not however invalidate any other reasons why
   you would have to distribute the object files for this program.  */


#include <string.h>
#include <allegro/debug.h>
#include <allegro/unicode.h>
#ifdef ALOGG_USE_TREMOR
#include <tremor/ivorbisfile.h>
#else
#include <vorbis/vorbisfile.h>
#endif
#include <curl/curl.h>
#include "aloggint.h"
#include "alogg.h"

typedef struct alogg_curl_data {
  CURL *handle;
  CURLM *multi;
  char *url;
  void *block;
  size_t size;
  size_t read_pos;
  size_t write_pos;
  size_t read_bytes;
} alogg_curl_data;

#define BLOCK_SIZE_MULTIPLIER 3


#ifdef DEBUG
static void check_write(AL_CONST alogg_curl_data *data,size_t bytes)
{
  size_t n;
  size_t write_pos=data->write_pos;
  for (n=0;n<bytes;++n) {
    ++write_pos;
    if (write_pos==data->size) write_pos=0;
    ASSERT(write_pos!=data->read_pos);
  }
}

static void check_read(AL_CONST alogg_curl_data *data,size_t bytes)
{
  size_t n;
  size_t read_pos=data->read_pos;
  for (n=0;n<bytes;++n) {
    ASSERT(read_pos!=data->write_pos);
    ++read_pos;
    if (read_pos==data->size) read_pos=0;
  }
}
#endif

static size_t get_available_bytes(AL_CONST alogg_curl_data *data)
{
  int available;
  ASSERT(data);
  available=data->write_pos-data->read_pos;
  if (data->read_pos>data->write_pos) available+=data->size;
  ASSERT(available>=0 && available<data->size);
  return available;
}

static int refill_url_buffer(void *datasource)
{
  int count;
  alogg_curl_data *data=datasource;
  ASSERT(data);

  /* Read only if we won't overflow the buffer */
  if (get_available_bytes(data)<data->size*2/BLOCK_SIZE_MULTIPLIER) {
    curl_multi_perform(data->multi,&count);
  }

  return count;
}

static int update_url_streaming(struct alogg_stream *stream,void *datasource)
{
  refill_url_buffer(datasource);
  return aloggint_basic_streamer_update(stream,datasource);
}

/* Callbacks to stream OGG/Vorbis data from an URL */
static size_t curl_writer(void *ptr,size_t size,size_t nmemb,void *stream)
{
  size_t bytes=size*nmemb,available;
  alogg_curl_data *data=(alogg_curl_data*)stream;
  ASSERT(data);
  TRACE("curl_writer: %u bytes read\n",bytes);
  available=get_available_bytes(data);
  ASSERT(available+bytes<data->size);
  while (bytes) {
    size_t left=data->size-data->write_pos;
    size_t write=MIN(bytes,left);
#ifdef DEBUG
    check_write(data,write);
#endif
    memcpy((char*)data->block+data->write_pos,ptr,write);
    bytes-=write;
    ptr=((char*)ptr+write);
    data->write_pos+=write;
    data->read_bytes+=write;
    if (data->write_pos==data->size) data->write_pos=0;
    ASSERT(get_available_bytes(data)+bytes<data->size);
  }

  return size*nmemb;
}

static size_t url_read(void *ptr,size_t size,size_t nmemb,void *datasource)
{
  size_t bytes=size*nmemb;
  int count;
  alogg_curl_data *data=(alogg_curl_data*)datasource;
  ASSERT(data);

  TRACE(
    "url_read: %u bytes requested, %u bytes available\n",
    bytes,get_available_bytes(data)
  );

#if 1
  /* At startup, make sure we read a bufferfull before doing anything else
     so libvorbis can detect Vorbisness properly */
  while (data->read_bytes<data->size/2) {
    TRACE("Loopstart, available %u\n",get_available_bytes(data));
    count=refill_url_buffer(data);
    /* This is to make sure we don't loop forever if the stream is shorter
       than a bufferfull */
    if (count==0) break;
    TRACE("Loopend: available %u\n",get_available_bytes(data));
  }
#endif

  /* read bytes */
  while (bytes) {
    size_t left=data->size-data->read_pos;
    size_t read=MIN(bytes,left);
    size_t available=get_available_bytes(data);
    if (available==0) break;
    if (read>available) read=available;
#ifdef DEBUG
    check_read(data,read);
#endif
    memcpy(ptr,(char*)data->block+data->read_pos,read);
    bytes-=read;
    ptr=(char*)ptr+read;
    data->read_pos+=read;
    if (data->read_pos==data->size) data->read_pos=0;
  }

  TRACE("url_read ends: %d bytes still available\n",get_available_bytes(data));

  return size*nmemb-bytes;

  /* If there was not enough data, fill with silence (unsigned) */
#ifdef DEBUG
  if (bytes) {
    TRACE("Not enough data for read, zeroing %u bytes\n",bytes);
    ASSERT(data->read_pos==data->write_pos);
  }
#endif
  while (bytes) {
    size_t left=data->size-data->read_pos;
    size_t write=MIN(bytes,left);
    memset(ptr,0,write);
    bytes-=write;
    data->read_pos+=write;
  }

  return size*nmemb;
}

static int url_seek(void *datasource,ogg_int64_t offset,int whence)
{
  return -1;
}

static int url_close(void *datasource)
{
  alogg_curl_data *data=(alogg_curl_data*)datasource;
  ASSERT(data);
  curl_multi_cleanup(data->multi);
  curl_easy_cleanup(data->handle);
  free(data->block);
  free(data->url);
  return 0;
}

static long url_tell(void *datasource)
{
  return 0;
}

static ov_callbacks url_callbacks={
  &url_read,
  &url_seek,
  &url_close,
  &url_tell
};


#ifdef DEBUG
static int aloggint_curl_log(CURL *c,curl_infotype t,char *msg,size_t s,void *d)
{
  static char *types[]={
    "TEXT",
    "HEADER_IN",
    "HEADER_OUT",
    "DATA_IN",
    "DATA_OUT",
  };
  TRACE("%s: %s\n",types[t],msg);
  return 0;
}
#endif

static AL_CONST char alogg_user_agent[]=ALOGG_ID " (libcurl "LIBCURL_VERSION")";
char alogg_curl_error[CURL_ERROR_SIZE];

struct alogg_stream *alogg_start_streaming_url(
  AL_CONST char *url,int (*configurator)(CURL*),size_t block_size
)
{
  struct alogg_stream *stream=NULL;
  alogg_curl_data *data=NULL;
  CURLcode code;

  /* Make sure the user doesn't get obsolete info */
  alogg_curl_error[0]=0;

  ASSERT(url);
  if (!url) return NULL;

  /* Allocate a structure to keep track of URL specific data */
  data=malloc(sizeof(alogg_curl_data));
  if (!data) return NULL;
  data->handle=NULL;
  data->multi=NULL;
  data->url=NULL;
  data->size=block_size*BLOCK_SIZE_MULTIPLIER;
  data->block=NULL;
  data->write_pos=0;
  data->read_pos=0;
  data->read_bytes=0;

  /* Allocate the memory we need in this structure */
  data->url=ustrdup(url);
  if (!data->url) goto alogg_start_streaming_url_error;
  data->block=malloc(data->size);
  if (!data->block) goto alogg_start_streaming_url_error;

  /* Initialize cURL */
  data->handle=curl_easy_init();
  if (!data->handle) goto alogg_start_streaming_url_error;

  /* Configure the access to the specified URL */
  code=curl_easy_setopt(data->handle,CURLOPT_URL,data->url);
  if (code!=CURLE_OK) goto alogg_start_streaming_url_error;
  code=curl_easy_setopt(data->handle,CURLOPT_NOPROGRESS,1);
  if (code!=CURLE_OK) goto alogg_start_streaming_url_error;
  code=curl_easy_setopt(data->handle,CURLOPT_FAILONERROR,1);
  if (code!=CURLE_OK) goto alogg_start_streaming_url_error;
  code=curl_easy_setopt(data->handle,CURLOPT_WRITEFUNCTION,&curl_writer);
  if (code!=CURLE_OK) goto alogg_start_streaming_url_error;
  code=curl_easy_setopt(data->handle,CURLOPT_WRITEDATA,data);
  if (code!=CURLE_OK) goto alogg_start_streaming_url_error;
  code=curl_easy_setopt(data->handle,CURLOPT_BUFFERSIZE,block_size);
  if (code!=CURLE_OK) goto alogg_start_streaming_url_error;
  code=curl_easy_setopt(data->handle,CURLOPT_USERAGENT,alogg_user_agent);
  if (code!=CURLE_OK) goto alogg_start_streaming_url_error;

#ifdef DEBUG
  code=curl_easy_setopt(data->handle,CURLOPT_VERBOSE,1);
  if (code!=CURLE_OK) goto alogg_start_streaming_url_error;
  code=curl_easy_setopt(data->handle,CURLOPT_DEBUGFUNCTION,&aloggint_curl_log);
  if (code!=CURLE_OK) goto alogg_start_streaming_url_error;
  code=curl_easy_setopt(data->handle,CURLOPT_ERRORBUFFER,alogg_curl_error);
  if (code!=CURLE_OK) goto alogg_start_streaming_url_error;
#endif

  /* Allow the user to configure cURL options */
  if (configurator) {
    if ((*configurator)(data->handle)) goto alogg_start_streaming_url_error;
  }

  data->multi=curl_multi_init();
  if (!data->multi) goto alogg_start_streaming_url_error;
  curl_multi_add_handle(data->multi,data->handle);

  /* Start streaming */
  stream=alogg_start_streaming_callbacks(
    data,(struct ov_callbacks*)&url_callbacks,block_size,&update_url_streaming
  );
  if (!stream) goto alogg_start_streaming_url_error;
  return stream;

alogg_start_streaming_url_error:
  /* In case of an error, make sure we don't leak */
  if (data) {
    if (data->multi) curl_multi_cleanup(data->multi);
    if (data->handle) curl_easy_cleanup(data->handle);
    free(data->block);
    free(data->url);
    free(data);
  }
  TRACE("alogg_start_streaming_url failed: %s\n",alogg_curl_error);
  return NULL;
}

