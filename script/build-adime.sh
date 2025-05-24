#!/usr/bin/env bash

cd "${0%/*}"
source /build/env

[ -d /build/src ] || mkdir -p /build/src
cd /build/src
[ ! -d ${ADIME_FULLNAME} ] || rm -rf ${ADIME_FULLNAME}
tar -xzvf /build/external/${ADIME_TARBALL}
cd ${ADIME_FULLNAME}
./fix.sh unix
make OFLAGS="-O2 -funroll-loops -ffast-math" CFLAGS=-fPIC
make install
ldconfig
