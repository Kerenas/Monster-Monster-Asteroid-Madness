#!/usr/bin/env bash

cd "${0%/*}"

sudo sh -c \
	"mkdir -p /usr/local/share/mmam && \
	cp -Rv share/* /usr/local/share/mmam && \
	cp -v mmam /usr/local/bin"
