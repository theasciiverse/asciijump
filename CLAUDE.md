# asciijump (Asciiverse packaging)

asciijump 1.0.2~beta, the ASCII-art ski jumping game by Grzegorz Moskal and
others, with Debian's patches and a few of ours, packaged for the Asciiverse
catalog and as system packages. Not our code: keep changes small and list
every one in PATCHES.md.

- No upstream repository exists (asciijump.prv.pl is gone); history starts
  from Debian's orig tarball (tag `1.0.2.beta`), then Debian's patches (tag
  `debian/1.0.2.beta-16`), then ours. Debian's packaging repo is
  salsa.debian.org/games-team/asciijump.
- Release: tag `v1.0.2.beta-av.<n>` and push it.
- Local static build: `docker run --rm -v "$PWD":/src -w /src alpine:3.22
  sh asciiverse/build.sh` leaves `./asciijump` and `./aj-server`. It needs
  autoconf; without it, the shipped configure also works with gcc on glibc.
- Keys: Enter starts the run, jumps and lands (telemark); left/right lean
  in the air; q leaves the hill. `'` in flight is a hidden "lucky" gust.
- The game chdirs into the hills folder while reading it.
- Preview: run `./asciijump` from `stage/` in a 100x30 tmux session with
  the status line off, record `tmux attach` with hub's `tools/castrec.py`,
  and drive it with `python3 asciiverse/drive.py <session> 38`.
