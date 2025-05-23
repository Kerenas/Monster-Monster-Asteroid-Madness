
# Select your system if not autodetected by uncommenting the appropriate line:

#SYSTEM=unix
#SYSTEM=dos
#SYSTEM=windows

ifeq ($(SYSTEM),)
  ifneq (,$(findstring linux,$(OSTYPE)))
    SYSTEM=unix
  endif
endif

ifeq ($(SYSTEM),)
  ifneq (,$(findstring Linux,$(shell uname -s)))
    SYSTEM=unix
  endif
endif

ifeq ($(SYSTEM),)
  ifdef DJDIR
    SYSTEM=dos
    ifneq (,$(findstring bash,$(SHELL)))
      UNIX_TOOLS = 1
    endif
  endif
endif

ifeq ($(SYSTEM),)
  ifdef MINGDIR
    SYSTEM=windows
    ifneq (,$(findstring /sh.exe,$(SHELL)))
      UNIX_TOOLS = 1
    endif
  endif
endif

ifeq ($(SYSTEM),)
  autodetect_failed: autodetect_message
endif



ifeq ($(SYSTEM),unix)
  ALLEGRO_CONFIG_OPTS=--libs
  ifeq ($(SHARED),)
    ALLEGRO_CONFIG_OPTS+=--static
  else
    ALLEGRO_CONFIG_OPTS+=--shared
  endif
  ifneq ($(DEBUG),)
    ALLEGRO_CONFIG_OPTS+=debug
  endif
  ALLEGRO=`allegro-config $(ALLEGRO_CONFIG_OPTS)`
  RM=rm -f
  RMTREE=rm -rf
  INSTALL_DIR=install -d -m 755
  INSTALL_BIN=install -m 755
  INSTALL_HDR=install -m 644
  INSTALL_LIB=install -m 644
  INSTALL_MAN=install -m 644
  PATHSEP=/
  SUFFIX=
  ifeq ($(PREFIX),)
    PREFIX=/usr/local
  endif
else
ifeq ($(SYSTEM),dos)
  ifeq ($(DEBUG),)
    ALLEGRO=-lalleg
  else
    ALLEGRO=-lalld
  endif
  SUFFIX=.exe
  ifdef UNIX_TOOLS
    RM=rm -f
    RMTREE=rm -rf
    INSTALL_DIR=mkdir
    INSTALL_BIN=cp
    INSTALL_HDR=cp
    INSTALL_LIB=cp
    INSTALL_MAN=cp
    PATHSEP=/
    ifeq ($(PREFIX),)
      PREFIX=$(subst \,/,$(DJDIR))
    endif
  else
    RM=del
    RMTREE=deltree /y
    INSTALL_DIR=mkdir
    INSTALL_HDR=copy
    INSTALL_BIN=copy
    INSTALL_LIB=copy
    INSTALL_MAN=copy
    PATHSEP=\\
    ifeq ($(PREFIX),)
      PREFIX=$(subst /,\,$(DJDIR))
    endif
    ECHO_DISLIKES_QUOTES=1
  endif
else
ifeq ($(SYSTEM),windows)
  ALLEGRO=
  ifeq ($(DEBUG),)
    ifeq ($(SHARED),)
      ALLEGRO+=-lalleg_s
    else
      ALLEGRO+=-lalleg
    endif
  else
    ifeq ($(SHARED),)
      ALLEGRO+=-lalld_s
    else
      ALLEGRO+=-lalld
    endif
  endif
  ifeq ($(SHARED),)
    ALLEGRO+=-lkernel32 -luser32 -lgdi32 -lcomdlg32 -lole32 -ldinput \
              -lddraw -ldxguid -lwinmm -ldsound
  endif
  SUFFIX=.exe
  ifdef UNIX_TOOLS
    RM=rm -f
    RMTREE=rm -rf
    INSTALL_DIR=mkdir
    INSTALL_HDR=cp
    INSTALL_BIN=cp
    INSTALL_LIB=cp
    INSTALL_MAN=cp
    PATHSEP=/
    ifeq ($(PREFIX),)
      PREFIX=$(subst \,/,$(MINGDIR))
    endif
  else
    RM=del
    RMTREE=deltree /y
    INSTALL_DIR=mkdir
    INSTALL_HDR=copy
    INSTALL_BIN=copy
    INSTALL_LIB=copy
    INSTALL_MAN=copy
    PATHSEP=\\
    ifeq ($(PREFIX),)
      PREFIX=$(subst /,\,$(MINGDIR))
    endif
    ECHO_DISLIKES_QUOTES=1
  endif
else
  autodetect_failed: autodetect_message
endif
endif
endif

PREFIXD=$(PREFIX)$(PATHSEP)

VERSION=1.3.7
MAJOR=1

TARBALLNAME=alogg-$(VERSION)
ZIPNAME=alogg137

BASELIBNAME=libalogg
BASEBINNAME=alogg

ifneq ($(TREMOR),)
CFLAGS+=-DALOGG_USE_TREMOR
LDFLAGS+=-Wl,--rpath -Wl,$(TREMOR)
LIBS=-lvorbisidec
BASELIBNAME:=$(BASELIBNAME)t
BASEBINNAME:=$(BASEBINNAME)t
else
LIBS=-lvorbisfile -lvorbisenc -lvorbis -logg -lm
endif

ifneq ($(CURL),)
CFLAGS+=-DALOGG_USE_CURL
LIBS+=`curl-config --libs`
endif

ifneq ($(PTHREAD),)
CFLAGS+=-DALOGG_USE_PTHREAD
LIBS+=-lpthread
endif

SHAREDLIBNAME=$(BASELIBNAME).so.$(VERSION)
ifneq ($(SHARED),)
CFLAGS+=-fPIC -shared
LIBNAME=$(SHAREDLIBNAME)
else
CFLAGS+=-DALLEGRO_STATICLINK
LIBNAME=$(BASELIBNAME).a
endif

CC=gcc
CFLAGS+=-Wall -W -Wno-unused
LDFLAGS+=-L.
LIBS+=$(ALLEGRO)

ifeq ($(TYPE),)
TYPE=html
endif
TYPEEXT=$(TYPE)
ifeq ($(TYPE),man)
TYPEEXT=3
endif

ifeq ($(DEBUG),1)
CFLAGS+=-g -DDEBUGMODE -DDEBUG -fno-builtin
else
CFLAGS+=-O2
endif

ifeq ($(DMALLOC),1)
CFLAGS+=-DDMALLOC
LIBS+=-ldmallocth
endif

.PHONY: autodetect_failed autodetect_message clean dist docs
.PHONY: install install_lib install_header install_examples install_man
.PHONY: uninstall
.PHONY: patchtest patchclean

EXAMPLES= $(BASEBINNAME)_play$(SUFFIX) \
          $(BASEBINNAME)_stream$(SUFFIX) \
          $(BASEBINNAME)_encode$(SUFFIX) \
          $(BASEBINNAME)_dat$(SUFFIX)

ifneq ($(PTHREAD),)
EXAMPLES+=$(BASEBINNAME)_thread$(SUFFIX)
endif

all: $(LIBNAME) $(EXAMPLES) alogg.html

ifeq ($(SYSTEM),unix)
all: alogg-config
endif

LIBALOGG_OBJS = $(BASEBINNAME).o
ifneq ($(CURL),)
LIBALOGG_OBJS += $(BASEBINNAME)url.o
endif
ifneq ($(PTHREAD),)
LIBALOGG_OBJS += $(BASEBINNAME)pth.o
endif

$(BASELIBNAME).a: $(LIBALOGG_OBJS)
	ar cr $(BASELIBNAME).a $(LIBALOGG_OBJS)

$(SHAREDLIBNAME): $(LIBALOGG_OBJS)
	$(CC) $(LDFLAGS) -shared -Wl,-soname,$(BASELIBNAME).so.$(MAJOR) -o $@ \
              $(LIBALOGG_OBJS) $(LIBS)

$(BASEBINNAME).o: aloggcfg.h alogg.h aloggint.h

$(BASEBINNAME)url.o: alogg.h aloggurl.h

$(BASEBINNAME)pth.o: alogg.h aloggpth.h

aloggcfg.h:
	-$(MAKE) patchtest
	-$(MAKE) patchclean

%t.o: %.c alogg.h aloggint.h
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.c alogg.h aloggint.h
	$(CC) $(CFLAGS) -o $@ -c $<

$(BASEBINNAME)%.o: alogg%.c alogg.h aloggint.h
	$(CC) $(CFLAGS) -o $@ -c $<

$(BASEBINNAME)_play$(SUFFIX): $(BASEBINNAME)_play.o $(LIBNAME)
	$(CC) $(LDFLAGS) -o $@ $(BASEBINNAME)_play.o $(LIBNAME) $(LIBS)

$(BASEBINNAME)_stream$(SUFFIX): $(BASEBINNAME)_stream.o $(LIBNAME)
	$(CC) $(LDFLAGS) -o $@ $(BASEBINNAME)_stream.o $(LIBNAME) $(LIBS)

$(BASEBINNAME)_encode$(SUFFIX): $(BASEBINNAME)_encode.o $(LIBNAME)
	$(CC) $(LDFLAGS) -o $@ $(BASEBINNAME)_encode.o $(LIBNAME) $(LIBS)

$(BASEBINNAME)_dat$(SUFFIX): $(BASEBINNAME)_dat.o $(LIBNAME)
	$(CC) $(LDFLAGS) -o $@ $(BASEBINNAME)_dat.o $(LIBNAME) $(LIBS)

$(BASEBINNAME)_thread$(SUFFIX): $(BASEBINNAME)_thread.o $(LIBNAME)
	$(CC) $(LDFLAGS) -o $@ $(BASEBINNAME)_thread.o $(LIBNAME) $(LIBS)

ifeq ($(SYSTEM),unix)
alogg-config: alogg-config.skel
	(cat alogg-config.skel | \
          sed -e s/=VERSION/=$(VERSION)/ | \
          sed -e s#=PREFIX#=$(PREFIX)# | \
          sed -e s#=TREMOR#=$(TREMOR)# | \
          sed -e s#=CURL#=$(CURL)# | \
          sed -e s#=PTHREAD#=$(PTHREAD)# | \
          sed -e s#=SHARED#=$(SHARED)# | \
          sed -e s#=MAJOR#=$(MAJOR)# \
        > alogg-config) || $(RM) alogg-config
	chmod 755 alogg-config
endif

clean:
	-$(RM) $(BASELIBNAME).a
	-$(RM) $(SHAREDLIBNAME)
	-$(RM) alogg.o
	-$(RM) aloggurl.o
	-$(RM) aloggpth.o
	-$(RM) aloggt.o
	-$(RM) aloggturl.o
	-$(RM) aloggtpth.o
	-$(RM) $(BASEBINNAME)_play.o
	-$(RM) $(BASEBINNAME)_play$(SUFFIX)
	-$(RM) $(BASEBINNAME)_stream.o
	-$(RM) $(BASEBINNAME)_stream$(SUFFIX)
	-$(RM) $(BASEBINNAME)_encode.o
	-$(RM) $(BASEBINNAME)_encode$(SUFFIX)
	-$(RM) $(BASEBINNAME)_dat.o
	-$(RM) $(BASEBINNAME)_dat$(SUFFIX)
	-$(RM) $(BASEBINNAME)_thread.o
	-$(RM) $(BASEBINNAME)_thread$(SUFFIX)
	-$(RM) aloggcfg.h
	-$(RM) alogg*.html
	-$(RM) alogg.texi
	-$(RM) alogg.rtf
	-$(RM) alogg.txt
	-$(RM) alogg.ascii
	-$(RM) datogg_*.3
	-$(RM) alogg_*.3
	-$(RM) alogg-config

install: install_alogg_config \
         install_headers \
         install_examples \
         install_lib \
         install_man

install_alogg_config:
ifeq ($(SYSTEM),unix)
	-$(INSTALL_DIR) $(PREFIXD)bin
	$(INSTALL_BIN) alogg-config $(PREFIXD)bin
endif

install_lib: install_alogg_config
	-$(INSTALL_DIR) $(PREFIXD)lib
ifeq ($(SHARED),)
	-$(INSTALL_LIB) $(BASELIBNAME).a $(PREFIXD)lib
else
	ln -fs $(SHAREDLIBNAME) $(PREFIX)/lib/$(BASELIBNAME)-$(MAJOR).so
	-($(INSTALL_LIB) $(SHAREDLIBNAME) $(PREFIXD)lib && \
          /sbin/ldconfig $(PREFIX)/lib || \
          (echo "ldconfig could not be run" && \
          echo "you will have to tell the linker where $(SHAREDLIBNAME) lives"))
endif

install_headers:
	-$(INSTALL_DIR) $(PREFIXD)include
	-$(INSTALL_DIR) $(PREFIXD)include$(PATHSEP)alogg
	$(INSTALL_HDR) alogg.h $(PREFIXD)include$(PATHSEP)alogg
	$(INSTALL_HDR) aloggint.h $(PREFIXD)include$(PATHSEP)alogg
	$(INSTALL_HDR) aloggurl.h $(PREFIXD)include$(PATHSEP)alogg
	$(INSTALL_HDR) aloggpth.h $(PREFIXD)include$(PATHSEP)alogg

install_examples:
	-$(INSTALL_DIR) $(PREFIXD)bin
	$(INSTALL_BIN) $(BASEBINNAME)_play$(SUFFIX) $(PREFIXD)bin
	$(INSTALL_BIN) $(BASEBINNAME)_stream$(SUFFIX) $(PREFIXD)bin
	$(INSTALL_BIN) $(BASEBINNAME)_encode$(SUFFIX) $(PREFIXD)bin
	$(INSTALL_BIN) $(BASEBINNAME)_dat$(SUFFIX) $(PREFIXD)bin
ifneq ($(PTHREAD),)
	$(INSTALL_BIN) $(BASEBINNAME)_thread$(SUFFIX) $(PREFIXD)bin
endif

install_man:
	-$(INSTALL_DIR) $(PREFIXD)man/$(PATHSEP)man1
	$(INSTALL_MAN) alogg-config.1 $(PREFIXD)man$(PATHSEP)man1
	-$(INSTALL_DIR) $(PREFIXD)man/$(PATHSEP)man3
	-$(INSTALL_MAN) datogg_*.3 $(PREFIXD)man$(PATHSEP)man3
	-$(INSTALL_MAN) alogg_*.3 $(PREFIXD)man$(PATHSEP)man3

uninstall:
	-$(RM) $(PREFIXD)bin$(PATHSEP)$(BASEBINNAME)_play
	-$(RM) $(PREFIXD)bin$(PATHSEP)$(BASEBINNAME)_stream
	-$(RM) $(PREFIXD)bin$(PATHSEP)$(BASEBINNAME)_encode
	-$(RM) $(PREFIXD)bin$(PATHSEP)$(BASEBINNAME)_dat
	-$(RM) $(PREFIXD)bin$(PATHSEP)$(BASEBINNAME)_thread
ifeq ($(SYSTEM),unix)
	-$(RM) $(PREFIXD)bin$(PATHSEP)alogg-config
endif
	-$(RM) $(PREFIXD)lib$(PATHSEP)$(BASELIBNAME).a
	-$(RM) $(PREFIXD)lib$(PATHSEP)$(SHAREDLIBNAME) && \
          /sbin/ldconfig $(PREFIX)/lib || \
          (echo "ldconfig could not be run" && \
          echo "there might be some links to $(SHAREDLIBNAME) left")
	-$(RMTREE) $(PREFIXD)include$(PATHSEP)alogg
	-$(RM) $(PREFIXD)man$(PATHSEP)man1$(PATHSEP)alogg-config.1
	-$(RM) $(PREFIXD)man$(PATHSEP)man3$(PATHSEP)datogg_*.3
	-$(RM) $(PREFIXD)man$(PATHSEP)man3$(PATHSEP)alogg_*.3

dist:
	-rm -f $(TARBALLNAME).tar.gz
	-rm -f $(ZIPNAME).zip
	-mkdir $(TARBALLNAME)
	cp alogg.h aloggint.h alogg.c \
           aloggurl.c aloggurl.h \
           aloggpth.c aloggpth.h \
           datogg.c datogg.inc datogg.scu datogg.scr datogg.scm \
           alogg_play.c alogg_stream.c alogg_encode.c alogg_dat.c \
           alogg_thread.c \
           Makefile alogg._tx sample.diff libs.diff tremor.diff _alst.c \
           alogg-config.skel alogg-config.1 \
           README INSTALL lgpl.txt LICENSE AUTHORS ChangeLog TODO \
           $(TARBALLNAME)
	tar cvfz $(TARBALLNAME).tar.gz $(TARBALLNAME)
	mv $(TARBALLNAME)/README $(TARBALLNAME)/README.txt
	mv $(TARBALLNAME) $(ZIPNAME)
	zip -rl $(ZIPNAME).zip $(ZIPNAME)
	rm -rf $(ZIPNAME)

alogg.html: alogg._tx
	makedoc -html alogg.html alogg._tx
	@echo The docs can be generated in a variety of formats.
	@echo To generate the docs in the FOO format, type:
	@echo \ \ make TYPE=FOO docs
	@echo For example, to generate docs in text format, type:
	@echo \ \ make TYPE=txt docs
	@echo Note: Allegro\'s makedoc must be in your path.

docs:
	makedoc -$(TYPE) alogg.$(TYPEEXT) alogg._tx

autodetect_message:
	@echo Your system type could not be autodetected
	@echo Edit the Makefile and select your system before trying again

patchtest:
	-$(RM) aloggcfg.h
ifeq ($(ECHO_DISLIKES_QUOTES),)
	echo "#undef HAS_REGISTER_SAMPLE_FILE_TYPE" > aloggcfg.h
else
	echo #undef HAS_REGISTER_SAMPLE_FILE_TYPE > aloggcfg.h
endif
	$(CC) $(CFLAGS) -o _alst.o -c _alst.c
	$(CC) $(LDFLAGS) -o _alst$(SUFFIX) _alst.o $(LIBS)
ifeq ($(ECHO_DISLIKES_QUOTES),)
	echo "#define HAS_REGISTER_SAMPLE_FILE_TYPE" > aloggcfg.h
else
	echo #define HAS_REGISTER_SAMPLE_FILE_TYPE > aloggcfg.h
endif

patchclean:
	-$(RM) _alst$(SUFFIX)
	-$(RM) _alst.o

