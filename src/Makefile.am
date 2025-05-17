SUFFIXES = .dat

bin_PROGRAMS = mmam

mmam_SOURCES = \
	mmam.cc mmam.h monstbase.cc auto_monstbase.inc fonts.dat fonts.h \
	albgi.cc albgi.h intro.cc bubbles.cc bubbles.h \
	dsound.cc dsound.h mmam_ai.cc mmam_ai.h rand.h rand.cc instruct.cc \
	qfe.cc qfe.h sounds.h sprites.h sprite_patch.h \
	setup_c.h setup.h hud.h hud.cc monstbase.h monster.cc asteroid.cc \
	gamelog.cc gamelog.h trader.cc trader.h event.cc event.h \
	alignment.cc alignment.h inventory.cc cage.h cage.cc input.h input.cc \
	gfx.h gfx.cc monster.h asteroid.h hints.cc xtrig.cc xtrig.h scripts.cc \
	scripts.h pmask.h pmask.cc altheora.cc altheora.h \
	particle.cc particle.h blood.cc blood.h mmam_aiut.cc ecosys.cc \
	graphs.cc graphs.h dumpdocs.cc namelist.cc namelist.h



mmam_DEPENDENCIES = @FONTS_O@ @WIN_ICON@

mmam_LDADD = @FONTS_O@ @WIN_ICON@ -ladime -ljpgal @PNG_LIBS@ @DUMB_LIB@ @OGG_LIBS@ @THEORA_LIB@ @ALLEG_LIBS@ @SYS_LIBS@ @EFENCE@
# @LIBNET@


INCLUDES = @MMAM_DEFINES@

MMAM_LEVELS = $(wildcard ../data/custom)

WINDRES=windres

%.res: %.rc
	$(WINDRES) -O coff -o $@ -i $<


EXTRA_DIST = fonts.dat mmam.rc  build_monstbase_h.sh  trader_strip.pl ecosys.pl mmam.cpr

BUILT_SOURCES = fonts.dat fonts.h monstbase.h auto_monstbase.inc sprite_patch.h ecosys.cc

CLEANFILES = fonts.s fonts.h mmam.res

fonts.h: fonts.o

fonts.o: fonts.dat
	dat2s fonts.dat -o fonts.s -h fonts.h -p font
	$(AS) -c fonts.s -o fonts.o || gcc -c fonts.s -o fonts.o
	@rm -f fonts.s || true

testnames: rand.o namelist.cc namelist.h
	g++ -DSTANDALONE -g -o testnames rand.o namelist.cc @ALLEG_LIBS@ @SYS_LIBS@

sprite_patch.h: sprites.h inventory.cc monstbase.cc trader_strip.pl
	perl trader_strip.pl

auto_monstbase.inc: sprites.h inventory.cc monstbase.cc trader_strip.pl
	perl trader_strip.pl

monstbase.h: sprites.h inventory.cc monstbase.cc trader_strip.pl
	perl trader_strip.pl

levels.dat: monstbase.cc $(MMAM_LEVELS) ecosys.pl
	perl ecosys.pl

ecosys.cc: monstbase.cc $(MMAM_LEVELS) ecosys.pl
	perl ecosys.pl

altheora: altheora.cc
	g++ -DSTANDALONE -o altheora altheora.cc @FONTS_O@ @ALLEG_LIBS@ @OGG_LIBS@ @THEORA_LIB@ @SYS_LIBS@ @EFENCE@

