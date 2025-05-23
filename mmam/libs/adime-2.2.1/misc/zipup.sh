#     _      _ _
#    /_\  __| (_)_ __  ___
#   / _ \/ _` | | '  \/ -_)
#  /_/ \_\__,_|_|_|_|_\___|
#
# zipup.sh:
# Script to generate the distributions.
#
# By Sven Sandberg
#
# Requires bash, cat, gzip, ln, rm, sed, sort, tar, and zip.
##

nodepend=0
if [ "$1" = "--no-depend" ] ; then
  nodepend=1
  shift
fi

if [ "$1" != "" ] ; then
   echo 1>&2 <<EOF
Usage: $0 [--no-depend]
  Creates adime-VERSION.tar.gz and adimeVERSION.zip You must run this
  script under Linux, after having compiled adime.chm under windows.
  See docs/txt/internal.txt for more information. The --no-depend flag
  disables re-generating dependencies; us it only if you know they are
  up to date.
EOF
fi

if [ ! -f docs/chm/adime.chm ] ; then
   echo "error: generate docs/chm/adime.chm first" 1>&2
   exit
fi

echo "hiding chm file to avoid deleting it..." 1>&2
mv docs/chm/adime.chm docs/chm/adime.chm.renamed

# note: these are patched by fixver.sh script.
zipversion=221
tgzversion=2.2.1

echo "converting to unix format..." 1>&2
./fix.sh unix --dtou
echo "running make clean..." 1>&2
make clean

# Dont generate dependencies if a parameter was given.  This is
# because I often realize that I missed something essential _after_
# calling zipup.sh, so it saves lots of time if the dependencies don't
# need to be rebuilt
if [ $nodepend = 1 ]; then
   echo "skip generating dependencies" 1>&2
   ./fix.sh unix --quick
else
   echo "generating dll stuff..." 1>&2
   ./misc/fixdll.sh
   echo "generating dependencies..." 1>&2
   ./fix.sh mingw32 --quick
   make depend CROSSCOMPILE=1
   ./fix.sh msvc --quick
   make depend CROSSCOMPILE=1
   ./fix.sh watcom --quick
   make depend CROSSCOMPILE=1
   ./fix.sh unix --quick
   make depend CROSSCOMPILE=1
   ./fix.sh djgpp --quick
   make depend CROSSCOMPILE=1
   ./fix.sh bcc32 --quick
   make depend CROSSCOMPILE=1
   ./fix.sh qnx --quick
   make depend CROSSCOMPILE=1
   ./fix.sh unix --quick
   make depend CROSSCOMPILE=1
   ./fix.sh macosx --quick
   make depend CROSSCOMPILE=1
fi

echo "unhiding chm file..." 1>&2
mv docs/chm/adime.chm.renamed docs/chm/adime.chm

echo "generating documentation..." 1>&2
make docs


#############################################################################
echo "generating default makefile..." 1>&2
cat > makefile << END_OF_MAKEFILE
error:
	@echo ""
	@echo No makefile generated! You must first run one of the following
	@echo "(choose your platform):"
	@echo " " ./fix.sh unix
	@echo " " fix.bat mingw32
	@echo " " fix.bat bcc32
	@echo " " fix.bat msvc
	@echo " " fix.bat djgpp
	@echo " " fix.bat watcom
	@echo " " ./fix.sh qnx
	@echo " " ./fix.sh macosx
	@echo ""
	@echo See the following files for detailed installation instructions:
	@echo " " docs/build/linux.txt
	@echo " " docs/build/mingw32.txt
	@echo " " docs/build/bcc32.txt
	@echo " " docs/build/msvc.txt
	@echo " " docs/build/djgpp.txt
	@echo " " docs/build/watcom.txt
	@echo " " docs/build/qnx.txt
	@echo " " docs/build/macosx.txt
END_OF_MAKEFILE
#############################################################################

files=`echo \
       makefile misc/makefile* \
       fix.{bat,sh} misc/*.sh \
       misc/{dllsyms.lst,scanexp.c} \
       misc/runner.c misc/makedoc/make*.[ch] \
       misc/template.c misc/TemplateInfo.plist misc/Info.plist \
       misc/project.pbxproj misc/project.pbxuser \
       obj/*/*/*.dep lib/*/*.def \
       lib/*/tmpfile.txt \
       readme.txt docs/html/readme.html docs/rtf/readme.rtf \
       docs/src/*._tx docs/chm/adime.chm license.txt \
       docs/build/*.txt docs/{txt,texi,info,man,devhelp,chm}/tmpfile.txt \
       src/*.c examples/ex*.c \
       include/{adime,fortify,fortify2,ufortify,adime/*}.h`

tgzfiles=`( for f in $files; do echo $f; done ) | sort | sed -e's!^!'adime-$tgzversion'/!'`
zipfiles=`( for f in $files; do echo $f; done ) | sort | sed -e's!^!'adime$zipversion'/!'`

echo "removing old archives..." 1>&2
rm -f adime$zipversion.zip
rm -f adime-$tgzversion.tar.gz

echo "creating adime-$tgzversion.tar.gz..." 1>&2
ln -s . adime-$tgzversion
tar cv $tgzfiles | gzip -9 > adime-$tgzversion.tar.gz
rm adime-$tgzversion

echo "converting to dos text format..." 1>&2
./fix.sh none --utod
echo "creating adime$zipversion..." 1>&2
ln -s . adime$zipversion
zip -9 adime$zipversion $zipfiles
rm adime$zipversion
echo "converting back to unix text format..." 1>&2
./fix.sh unix --dtou

echo "All done!" 1>&2
