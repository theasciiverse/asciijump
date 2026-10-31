#!/bin/sh
# Lays out the av archive in stage/: the start script at the top, the
# binaries in lib/, and the gfx/ and hills/ folders beside them. The
# network server, aj-server, comes along with its own start script for
# anyone hosting a game.
set -eu
rm -rf stage && mkdir -p stage/lib
cp asciijump aj-server stage/lib/
cp asciiverse/asciijump asciiverse/aj-server stage/
cp -r gfx hills stage/
find stage/gfx stage/hills -name a -size 0 -delete
cp COPYING README stage/
