# Building the game

## Archlinux
`podman` and `fuse-overlayfs` packages need to be installed




# makefile / configure

## Steps

- autoconf
- ./configure CXXFLAGS=-fpermissive

## Mods

- configure file needs to be changed to use dat2c

# Allegro

- Needs dat2c.c to be patched, add +1 to strlen in do_conversion

- Probably needs to be rebuilt with OSS

# Alogg

- Needs to use /bin/bash instead of /bin/sh

# MMAM

- Code needs to be modified for fonts.h/fonts.c
