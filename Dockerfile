FROM ubuntu:24.04

ARG LC_ALL

# Base update
RUN \
  apt update

# Fix locales
RUN \
  apt install locales && \
  locale-gen $LC_ALL

# A few dev tools
RUN \
 apt install -y gdb neovim less git

# Dependencies
RUN \
  apt install -y wget build-essential autotools-dev automake cmake

# Alogg Dependencides
RUN \
  apt install -y libvorbis-dev 

RUN \
  /build/script/build-allegro.sh

RUN \
  /build/script/build-adime.sh

RUN \
  /build/script/build-alogg.sh

# Set working directory
WORKDIR /build/src/mmam

RUN \
  ./build.sh
