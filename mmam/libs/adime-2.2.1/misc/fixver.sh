#! /bin/sh
#
#  Shell script to adjust the version numbers and dates in adime.h,
#  dllver.rc, readme._tx, adime._tx and makefile.ver.


if [ $# -lt 3 -o $# -gt 4 ]; then
   echo "Usage: fixver major_num sub_num wip_num [comment]" 1>&2
   echo "Example: fixver 3 9 1 WIP" 1>&2
   exit 1
fi

# get the version and date strings in a nice format
if [ $# -eq 3 ]; then
   verstr="$1.$2.$3"
else
   verstr="$1.$2.$3 ($4)"
fi

year=$(date +%Y)
month=$(date +%m)
day=$(date +%d)
datestr="$(date +%b) $day, $year"

# patch include/adime.h
echo "s/\#define ADIME_VERSION_MAJOR .*/\#define ADIME_VERSION_MAJOR      $1/" > fixver.sed
echo "s/\#define ADIME_VERSION_MINOR .*/\#define ADIME_VERSION_MINOR      $2/" >> fixver.sed
echo "s/\#define ADIME_VERSION_PATCH .*/\#define ADIME_VERSION_PATCH      $3/" >> fixver.sed
echo "s/\#define ADIME_DATE .*/\#define ADIME_DATE               $year$month$day    \/\* yyyymmdd \*\//" >> fixver.sed
echo "Patching include/adime.h..."
cp include/adime.h fixver.tmp
sed -f fixver.sed fixver.tmp > include/adime.h

# todo: allegro patches src/win/dllver.rc, do we need this?

# patch docs/src/readme._tx
echo "s/^\( *Version\) .*/\1 $verstr/" > fixver.sed
echo "s/By Sven Sandberg, .*\./By Sven Sandberg, $datestr\./" >> fixver.sed

echo "Patching docs/src/readme._tx..."
cp docs/src/readme._tx fixver.tmp
sed -f fixver.sed fixver.tmp > docs/src/readme._tx

# patch docs/src/adime._tx, docs/src/readme._tx and docs/src/internal._tx
echo "s/@manh=\"version [^\"]*\"/@manh=\"version $verstr\"/" >> fixver.sed

echo "Patching docs/src/adime._tx..."
cp docs/src/adime._tx fixver.tmp
sed -f fixver.sed fixver.tmp > docs/src/adime._tx

echo "Patching docs/src/internal._tx..."
cp docs/src/internal._tx fixver.tmp
sed -f fixver.sed fixver.tmp > docs/src/internal._tx

# patch misc/makefile.ver
echo "s/LIB_VERSION = .*/LIB_VERSION = $1$2/" > fixver.sed
echo "s/shared_version = .*/shared_version = $1.$2.$3/" >> fixver.sed
echo "s/shared_major_minor = .*/shared_major_minor = $1.$2/" >> fixver.sed

echo "Patching misc/makefile.ver..."
cp misc/makefile.ver fixver.tmp
sed -f fixver.sed fixver.tmp > misc/makefile.ver

# patch misc/zipup.sh
echo "s/zipversion=.*/zipversion=$1$2$3/" > fixver.sed
echo "s/tgzversion=.*/tgzversion=$1.$2.$3/" >> fixver.sed

echo "Patching misc/zipup.sh..."
cp misc/zipup.sh fixver.tmp
sed -f fixver.sed fixver.tmp > misc/zipup.sh

# clean up after ourselves
rm fixver.sed fixver.tmp

echo "Done!"
