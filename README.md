# Introduction

The build system uses podman, a container engine similar to docker. In this document we will use Archlinux to build the game but it should work on any modern Linux.

# Building

## Prerequisites

In the building phase we need podman and fuse-overlayfs:

`sudo pacman -S podman fuse-overlayfs`

To start the build process, enter the source directory (containing the Dockerfile and makefile) and launch the build:

`make`

At the end, you should get a few files (mmam among them) in the output/ directory

# Installing the game

You will need to go to the output directory and launch the installer:

`cd output
./install.sh
`

You will be asked to enter your password as the installer needs root privileges to copy data files to /usr/local/share/mmam

# Launching

## Prerequisite

You will need to install version 4 of the Allegro library to be able to play on your system:

`sudo pacman -S allegro4`

## Running

And now to launch the game, from any directory:

`mmam`

# Repository structure

A few important directories:

Directory | note
--- | ---
mmam | The sources of the actual game
patch | Patches to libraries to make them work for us
script | build scripts for each of the libraries
output | Directory where the final output will be found

## Notes

# Modifications to the game itself

## dat2c

Probably the biggest modication is in using `dat2c` instead of `dat2s`. Those are two programs, both provided by Allegro that generate code from font files. The difference between the two is `dat2s` would generate assembly and `dat2c` would generate C code.

However, `dat2s` would not work on 64-bits system so I switched to `dat2c`. That lead me to have to fix a small bug in Allegro but it all worked out in the end.

## pmask.h

There was a weird compile-time error where the (easy) fix was to tell pmask (a library to detect sprite collisions) to use 64-bit integers instead of 32 bit ones.

# Modifications to the libraries

## Allegro

I made a modification to the Allegro library because of a crash that happened using dat2c (an executable provided by the library) crashed when using a prefix for the generated font variable names.

## Alogg

In this case I just modified the header to the `alogg-config` script to use `bash` instead of `sh`. I think maybe Alogg was written while a real `sh` was packaged with Linux but it has been replaced by a symlink to `dash` for a long time now.

## ADime

The library compiled with optimisations for the Pentium processor and would not run on any other architecture. I just changed the compilation flags to fix it.

