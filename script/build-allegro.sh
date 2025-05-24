#!/usr/bin/env bash

cd "${0%/*}"
source /build/env

[ -d /build/src ] || mkdir -p /build/src
cd /build/src
[ ! -d ${ALLEGRO_FULLNAME} ] || rm -rf ${ALLEGRO_FULLNAME}
tar -xzvf /build/external/${ALLEGRO_TARBALL}
cd ${ALLEGRO_FULLNAME}
patch -p0 < /build/patch/allegro.1.patch
mkdir -p build
cd build
cmake ..
make
cd ..
mkdir -p docs/build
cd docs/build
cmake ..
make
cp -v makedoc /usr/local/bin
cd ../..
cp -v docs/txt/* build/docs/txt
cd build
make install
ldconfig
