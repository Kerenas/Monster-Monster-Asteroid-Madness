#!/usr/bin/env bash

cd "${0%/*}"
source /build/env

mkdir -p /usr/local/share/mmam
cp -Rv share/* /usr/local/share/mmam
cp mmam /usr/local/bin
