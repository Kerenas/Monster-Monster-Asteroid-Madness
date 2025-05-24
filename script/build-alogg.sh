#!/usr/bin/env bash

cd "${0%/*}"
source /build/env

[ -d /build/src ] || mkdir -p /build/src
cd /build/src
[ ! -d ${ALOGG_FULLNAME} ] || rm -rf ${ALOGG_FULLNAME}
tar -xzvf /build/external/${ALOGG_TARBALL}
cd ${ALOGG_FULLNAME}
patch -p0 < /build/patch/alogg.1.patch
make
make install
ldconfig
