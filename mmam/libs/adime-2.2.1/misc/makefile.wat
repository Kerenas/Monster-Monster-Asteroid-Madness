#     _      _ _
#    /_\  __| (_)_ __  ___
#   / _ \/ _` | | '  \/ -_)
#  /_/ \_\__,_|_|_|_|_\___|
#
#  makefile.wat:
#  Makefile for the Adime library with Watcom. Should not be used directly!
#
#  By Sven Sandberg
#
#  See readme.txt for more information about Adime.
#
#  See makefile.all for a list of the available targets.
#
#
#  This build uses djgpp for building the assembler sources and calculating
#  source dependencies, so you'll need to have that installed as well.
#
#  The "depend" target and assembler operations use sed.



# -------- Define some variables that the primary makefile may use. --------

PLATFORM = Watcom

EXE_SUFFIX = .exe
OBJ_SUFFIX = .obj
LIB_SUFFIX = .lib

HTML_SUFFIX = .htm
INFO_SUFFIX = .inf
TEXI_SUFFIX = .txi
_TX_HTML_FLAG = -htm
_TX_TEXI_FLAG = -txi

ifndef OBJ_ROOT_DIR
OBJ_ROOT_DIR = obj
OBJ_ROOT_DIR_D = obj
endif
OBJ_PLATFORM_ROOT_DIR = $(OBJ_ROOT_DIR)/watcom
OBJ_PLATFORM_ROOT_DIR_D = $(OBJ_ROOT_DIR_D)\watcom
OBJ_DIR = $(OBJ_PLATFORM_ROOT_DIR)/$(PACKAGE5)
OBJ_DIR_D = $(OBJ_PLATFORM_ROOT_DIR_D)\$(PACKAGE5)

ifndef LIB_ROOT_DIR
LIB_ROOT_DIR = lib
LIB_ROOT_DIR_D = lib
endif
LIB_DIR = $(LIB_ROOT_DIR)/watcom
LIB_DIR_D = $(LIB_ROOT_DIR)\watcom

LIB_BASENAME = $(PACKAGE5).lib
LIB_NAME = $(LIB_DIR)/$(LIB_BASENAME)



# -------- Check that the WATCOM environment variable is set. --------

ifndef WATCOM
  ifndef CROSSCOMPILE
    define BADWATCOM

      ERROR:
      Your WATCOM environment variable is not set!
      You must set the environment variable WATCOM to point to your Watcom
      directory, e.g. "set WATCOM=c:\watcom"

    endef
    $(error $(BADWATCOM))
  endif
endif

WATCOMDIR_U = $(subst \,/,$(WATCOM))
WATCOMDIR_D = $(subst /,\,$(WATCOM))

SYSTEM_INCLUDE_DIR = $(WATCOMDIR_U)/h
SYSTEM_INCLUDE_DIR_D = $(WATCOMDIR_D)\h
SYSTEM_LIB_DIR = $(WATCOMDIR_U)/lib386
SYSTEM_LIB_DIR_D = $(WATCOMDIR_D)\lib386



# -------- Try to figure out the compiler version. --------

ifndef WATCOM_VERSION
ifeq ($(wildcard $(WATCOMDIR_U)/binw/wdisasm.exe),)
WATCOM_VERSION=11
else
WATCOM_VERSION=10.6
endif
endif



# -------- Find Allegro, runner and makedoc. --------

ifndef NEED_ALLEGRO
  ifdef NEED_MAKEDOC
    ifeq ($(wildcard $(MAKEDOC_SOURCE)),)
      NEED_ALLEGRO = 1
    endif
  endif
  ifeq ($(wildcard $(RUNNER_SOURCE)),)
    NEED_ALLEGRO = 1
  endif
endif

ifdef NEED_ALLEGRO
  ifndef ALLEGRO
    define BADALLEGRO

      ERROR:
      Could not find Allegro!
      Is your ALLEGRO environment variable set correctly? It should
      point to the place where Allegro lives, e.g. c:\allegro. Run
        make ALLEGRO=c:\allegro
      (or whatever the location of Allegro is).
      See also readme.txt.

    endef
    $(error $(BADALLEGRO))
  endif
  ALLEGRODIR_U = $(subst \,/,$(ALLEGRO))
endif


ifneq ($(wildcard $(RUNNER_SOURCE)),)
  RUNNER = obj/watcom/runner.exe
  define COMPILE_RUNNER
    gcc -O -Wall -Werror -o $(RUNNER) $(RUNNER_SOURCE)
  endef
  RUNNER_DEP = obj/watcom/runner.exe
else
  RUNNER = $(wildcard $(ALLEGRODIR_U)/obj/watcom/runner.exe)
  RUNNER_DEP =
endif

ifeq ($(RUNNER),)
  ifndef CROSSCOMPILE
    define BADRUNNER
  
      ERROR:
      Could not find Allegros runner utility.
      Have you forgotten to make Allegro? You need to make the Watcom version of
      Allegro before making the Watcom version of Adime.
      
    endef
    $(error $(BADRUNNER))
  endif
endif


ifdef NEED_MAKEDOC
include misc/makefile.doc
endif



# -------- decide what compiler options to use --------

ifdef WARNMODE
WFLAGS = -w3 -we
else
WFLAGS = -w1
endif

ifdef DEBUGMODE
# -------- debugging build --------
CFLAGS = -DDEBUGMODE=$(DEBUGMODE) $(WFLAGS) -bt=dos4g -5s -s -d2
SFLAGS = -DDEBUGMODE=$(DEBUGMODE) -Wall
LFLAGS = "option quiet" "option stack=128k" "system dos4g" "debug all" "library alleg"
else
ifdef PROFILEMODE
# -------- profiling build --------
CFLAGS = $(WFLAGS) -bt=dos4g -5s -ox -et
SFLAGS = -Wall
LFLAGS = "option quiet" "option stack=128k" "system dos4g" "library alleg"
else
# -------- optimised build --------
CFLAGS = $(WFLAGS) -bt=dos4g -5s -ox
SFLAGS = -Wall
LFLAGS = "option quiet" "option stack=128k" "system dos4g"
LIB_FLAGS = "library alleg"
endif
endif

CFLAGS_NO_OPTIMIZE = $(WFLAGS) -bt=dos4g -5s -s

COMPILE_FLAGS = $(subst src/,-dADIME_SRC ,$(findstring src/, $<))$(CFLAGS)
COMPILE_FLAGS_NO_OPTIMIZE = $(subst src/,-dADIME_SRC ,$(findstring src/, $<))$(CFLAGS_NO_OPTIMIZE)

ifdef ADIME_FORTIFY
CFLAGS += -D_ADIME_FORTIFY=1 -DFORTIFY
endif
ifdef ADIME_DEVELOPING
CFLAGS += -D_ADIME_DEVELOPING=1
endif



# -------- List platform specific objects and programs. --------

VPATH +=

OBJ_LIST = $(COMMON_OBJS)

# Insert names of targets to build platform specific programs here.
PROGRAMS =
# Insert targets to build platform specific programs here.
# myprogram: path/myprogram$(EXE_SUFFIX)



# -------- Define how to compile. --------

GCC2WATCOM = -D__WATCOMC__ -D__SW_3S -D__SW_S -U__GNUC__ -UDJGPP -U__unix__

COMPILE_C_TO_OBJ_DEPS = $(RUNNER_DEP)
define COMPILE_C_TO_OBJ
$(RUNNER) wcc386 \\ $< @ $(COMPILE_FLAGS) -zq -fr=nul -I./include -fo=$@
endef
define COMPILE_C_TO_OBJ_NO_OPTIMIZE
$(RUNNER) wcc386 \\ $< @ $(COMPILE_FLAGS_NO_OPTIMIZE) -zq -fr=nul -I./include -fo=$@
endef

ifeq ($(WATCOM_VERSION),11)
# Watcom 11.0 supports COFF.
COMPILE_S_TO_OBJ_DEPS =
define COMPILE_S_TO_OBJ
gcc $(GCC2WATCOM) $(SFLAGS) -I./include -x assembler-with-cpp -o $(OBJ_DIR)/$*.obj -c $<
endef
else
# Black magic to build asm sources with Watcom 10.6.
COMPILE_S_TO_OBJ_DEPS = $(RUNNER_DEP)
define COMPILE_S_TO_OBJ
gcc $(GCC2WATCOM) $(SFLAGS) -I./include -x assembler-with-cpp -o $(OBJ_DIR)/$*.o -c $<
$(RUNNER) wdisasm \\ -a $(OBJ_DIR)/$*.o > $(OBJ_DIR)/$*.lst
sed -e "s/\.text/_TEXT/; s/\.data/_DATA/; s/\.bss/_BSS/; s/\.386/\.586/; s/lar *ecx,cx/lar ecx,ecx/" $(OBJ_DIR)/$*.lst > $(OBJ_DIR)/$*.asm
$(RUNNER) wasm \\ $(WFLAGS) -zq -fr=nul -fp3 -fo=$@ $(OBJ_DIR)/$*.asm
endef
endif

LINK_LIB_DEPS = $(RUNNER_DEP)
define LINK_LIB
$(RUNNER) wlib \\ @ -q -b -n $(LIB_NAME) $(addprefix +,$(LIB_OBJS))
endef

LINK_EXE_DEPS = $(RUNNER_DEP)
define LINK_EXE
$(RUNNER) wlink \\ @ $(LFLAGS) "name $@" "file $<" "library $(LIB_NAME)" $(LIB_FLAGS)
endef

LINK_EXE_NOLIB_DEPS = $(RUNNER_DEP)
define LINK_EXE_NOLIB
$(RUNNER) wlink \\ @ $(LFLAGS) "name $@" "file $<" $(LIB_FLAGS)
endef

LINK_EXE_ALL_DEPS = $(RUNNER_DEP)
define LINK_EXE_ALL
$(RUNNER) wlink \\ @ $(LFLAGS) "name $@" "$(addprefix file ,$(filter-out $(RUNNER_DEP),$^))" "library $(LIB_NAME)" $(LIB_FLAGS)
endef

LINK_EXE_ALL_NOLIB_DEPS = $(RUNNER_DEP)
define LINK_EXE_ALL_NOLIB
$(RUNNER) wlink \\ @ $(LFLAGS) "name $@" "$(addprefix file ,$(filter-out $(RUNNER_DEP),$^))" $(LIB_FLAGS)
endef



# -------- Rules for installing and removing the library files. --------

$(SYSTEM_LIB_DIR)/$(LIB_BASENAME): $(LIB_NAME)
	copy $(subst /,\,$< $@)

$(SYSTEM_INCLUDE_DIR)/adime.h: include/adime.h
	copy $(subst /,\,$< $@)

$(SYSTEM_INCLUDE_DIR)/adime:
	md $(subst /,\,$@)

$(SYSTEM_INCLUDE_DIR)/adime/%.h: include/adime/%.h include/adime
	copy $(subst /,\,$< $@)


INSTALL_HEADERS = $(addprefix $(SYSTEM_INCLUDE_DIR)/adime/,$(notdir $(wildcard include/adime/*.h)))

INSTALL_FILES = $(SYSTEM_LIB_DIR)/$(LIB_BASENAME) \
					 $(SYSTEM_INCLUDE_DIR)/adime.h \
					 $(SYSTEM_INCLUDE_DIR)/adime \
					 $(INSTALL_HEADERS)

install: $(INSTALL_FILES)
	@echo The $(DESCRIPTION) Watcom library has been installed.

UNINSTALL_FILES = $(SYSTEM_LIB_DIR)/adime.lib $(SYSTEM_LIB_DIR)/adimd.lib \
						$(SYSTEM_INCLUDE_DIR)/adime.h \
						$(SYSTEM_INCLUDE_DIR)/adime/*.h

uninstall:
	-rm -fv $(UNINSTALL_FILES)
	-rd $(SYSTEM_INCLUDE_DIR_D)\adime
	@echo All gone!



# -------- Compile anything that can't be done in makefile.all. --------

# this section is empty



# -------- Generate automatic dependencies. --------
# Note that this must be done with gcc!

DEPEND_PARAMS = $(GCC2WATCOM) -MM -MG -I./include -DADIME_SCAN_DEPEND

depend:
	gcc $(DEPEND_PARAMS) src/*.c examples/*.c misc/makedoc/*.c misc/runner.c > _depend.tmp
	sed -e "s/^[a-zA-Z0-9_\/]*\///" -e "s/[^ ]*version\.h//" -e "s/[^ ]*djgpp\.ver//" -e "s/[^ ]*allegro[^ ]*//g" -e'/^ *\\/d'  _depend.tmp > _depend2.tmp
	sed -e "s/^\([a-zA-Z0-9_]*\.o:\)/obj\/watcom\/adime\/\1/" _depend2.tmp > obj/watcom/adime/makefile.dep
	sed -e "s/^\([a-zA-Z0-9_]*\.o:\)/obj\/watcom\/adimd\/\1/" _depend2.tmp > obj/watcom/adimd/makefile.dep
	rm _depend.tmp _depend2.tmp
