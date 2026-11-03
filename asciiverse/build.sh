#!/bin/sh
# Builds asciijump and aj-server. Linux: run inside alpine:3.22 for fully
# static binaries; macOS: run on the runner with CC_ARCH set to x86_64 or
# arm64.
#   docker run --rm -v "$PWD":/src -w /src alpine:3.22 sh asciiverse/build.sh
# Leaves ./asciijump and ./aj-server. S-Lang 2.3.3 is built from its
# release tarball (checksum pinned) and linked in statically everywhere,
# so players need no S-Lang of their own; it still reads the system's
# terminfo, as ncurses does.
# Data is compiled in as /usr/share/asciijump for the system packages; av's
# start script points ASCIIJUMP_DATADIR at the folder next to it instead.
set -eu
SLANG=slang-2.3.3
SLANG_SHA256=f9145054ae131973c61208ea82486d5dd10e3c5cdad23b7c4a0617743c8f5a18

if [ -f /etc/alpine-release ]; then
  # ncurses-terminfo-base gives /etc/terminfo: without any terminfo folder
  # S-Lang's configure falls back to termcap and wants -ltermcap
  apk add --no-cache build-base autoconf automake curl ncurses-terminfo-base >/dev/null
  ldflags="-static"
else
  ldflags=""
fi
cc="cc${CC_ARCH:+ -arch $CC_ARCH}"
top=$PWD
work=$top/.slang
rm -rf "$work" && mkdir -p "$work"

curl -sfL --retry 8 --retry-delay 15 --retry-all-errors "https://www.jedsoft.org/releases/slang/$SLANG.tar.bz2" -o "$work/$SLANG.tar.bz2"
echo "$SLANG_SHA256  $work/$SLANG.tar.bz2" | sha256sum -c - >/dev/null 2>&1 \
  || echo "$SLANG_SHA256  $work/$SLANG.tar.bz2" | shasum -a 256 -c - >/dev/null
tar xjf "$work/$SLANG.tar.bz2" -C "$work"
(cd "$work/$SLANG" &&
  ./configure --prefix="$work/root" --without-png --without-pcre --without-onig \
    --without-z --without-iconv CC="$cc" >/dev/null &&
  make static >/dev/null &&
  make install-static >/dev/null)

# the shipped configure (autoconf 2.58) fails its own checks under current
# compilers: regenerate it, as Debian does, and take current config.guess
# and config.sub from automake (the 2003 ones cannot name arm64 Macs)
autoconf 2>/dev/null
cp "$(automake --print-libdir)/config.guess" "$(automake --print-libdir)/config.sub" .
./configure --prefix=/usr --bindir=/usr/bin --datadir=/usr/share --localstatedir=/var \
  CC="$cc" CPPFLAGS="-I$work/root/include" LDFLAGS="-L$work/root/lib $ldflags" LIBS="-lm" >/dev/null
make clean >/dev/null 2>&1 || true
make >/dev/null
strip asciijump aj-server
