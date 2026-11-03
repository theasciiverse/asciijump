# Changes from upstream

This is the Asciiverse packaging of asciijump 1.0.2~beta (GPL-2.0-or-later)
by Grzegorz Moskal, with Michal Moskal and Przemek Niezborala; the last
upstream release (asciijump.prv.pl, long gone). History here starts from
the release tarball as Debian distributes it
(asciijump_1.0.2.beta.orig.tar.gz, sha256
cce73c5c8cb12f5bb1e33051237784b85c25a6f6518098465d3de68bbff7e952), tagged
`1.0.2.beta`.

## Debian's patches

From asciijump 1.0.2.beta-16 (asciijump_1.0.2.beta-16.debian.tar.xz, sha256
b5ef19ece12fcdf0a6243b69a78d319f3dd15c45519abb765b1da0c514dda3f9), one
commit each, in Debian's order: format-string safety, Makefile fixes,
S-Lang 2 (Mario Iseli), linking with --as-needed, a sorted link order
(Reiner Herrmann), the AC_DEFINE fix (TANIGUCHI Takaki), missing includes
and declarations, CPPFLAGS, the as_port declaration and the desktop icon
name (Reiner Herrmann), and the GCC 15 build fix (Adrian Bunk). Tagged
`debian/1.0.2.beta-16`.

## Ours

- **Hill records without setgid games.** asciijump keeps each hill's record
  in /var/games/asciijump/, which needs a setgid install. Now, when that
  folder is not writable, records go to ~/.asciijump/, created if needed.
  Writing a record's owner also checks that the file opened.
- **Relocatable data folder.** ASCIIJUMP_DATADIR overrides the compiled-in
  data folder (gfx/ and hills/), in the game and in aj-server, so av's
  install finds them. The game still looks in ./gfx and ./hills first, as
  before.
- **Builds without ctags.** `make all` no longer runs ctags, which only
  wrote a tags file.
- **Hill files load on ARM.** xgetline() kept fgetc()'s result in a char,
  so where char is unsigned (arm64) EOF never matched and reading the
  hills hung; it now reads into an int. Found by the Games on the Linux
  console blog (ttygames.wordpress.com, 2013) on a Raspberry Pi.
- **main.h includes <stdlib.h>** for getenv().
- **How to play it, at the top of README.** A short note on installing and
  starting it with av; the original text follows unchanged.

## Packaging (new files)

- `asciiverse/build.sh`: builds S-Lang 2.3.3 from its release tarball
  (checksum pinned) and links it in statically; static Linux build in
  Alpine, macOS builds per architecture; regenerates configure and takes
  current config.guess and config.sub from automake, as Debian does.
- `asciiverse/asciijump`, `asciiverse/aj-server`, `asciiverse/stage.sh`:
  av's start scripts and the archive layout (binaries in lib/, gfx/ and
  hills/ beside them).
- `asciiverse/nfpm.yaml`: .deb, .rpm, .apk and Arch packages; no setgid.
- `asciiverse/drive.py`, `asciiverse/asciijump.cast`: the catalog preview,
  played by a script that reads the screen and lands with a telemark.
- `.github/workflows/release.yml`: builds all of it on a tag.

## Not built

- Windows. Network play (aj-server and "join a game") is in the build but
  untested here.
