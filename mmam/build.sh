#!/usr/bin/env bash

# Stop on error
set -e

autoreconf
./configure CXXFLAGS="-fpermissive -Wno-narrowing" 
automake
make clean
rm -rf /build/output/*
make -j$(nproc)
make install
cp -Rv /usr/local/share/mmam /build/output/share
cp -v src/mmam /build/output/mmam
cp -v /build/script/install.sh /build/output
