#!/usr/bin/env bash

# Stop on error
set -e

autoreconf
./configure CXXFLAGS="-fpermissive -Wno-narrowing" 
automake
make clean
make #-j$(nproc)
