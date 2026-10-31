"""drive.py SESSION SECONDS [GAP] [debug]: play asciijump's training in a
tmux pane, jumping until SECONDS have passed, then quit.

Recorded asciiverse/asciijump.cast: asciijump runs in a 100x30 tmux session
(status line off), hub's tools/castrec.py records a `tmux attach` to it, and
this script reads the pane (with colours) and sends the keys. The skier is drawn in blue and cyan and the hill in white;
it lands with a telemark when the gap between them is small."""
import re, subprocess, sys, time

S, secs = sys.argv[1], float(sys.argv[2])
GAP = int(sys.argv[3]) if len(sys.argv) > 3 else 2
DEBUG = len(sys.argv) > 4
def key(*k): subprocess.run(["tmux", "send-keys", "-t", S, *k])
def wait(t): time.sleep(t)
SGR = re.compile(r"\x1b\[([0-9;]*)m")

def cells():
    """The pane as rows of (char, foreground code)."""
    out = subprocess.run(["tmux", "capture-pane", "-e", "-p", "-t", S],
                         capture_output=True, text=True).stdout
    rows = []
    for line in out.split("\n")[:30]:
        fg, row, i = "39", [], 0
        while i < len(line):
            m = SGR.match(line, i)
            if m:
                for c in (m.group(1) or "0").split(";"):
                    if c in ("0", "39") or c.startswith(("3", "9")):
                        fg = "39" if c == "0" else c
                i = m.end()
            else:
                row.append((line[i], fg)); i += 1
        rows.append(row)
    return rows

def gap(rows):
    """Rows between the skier's lowest part and the hill under it, or None."""
    # the skier stays near the middle; the HUD and status line use blue too
    sk = [(y, x) for y, r in enumerate(rows[:28]) for x, (c, f) in enumerate(r)
          if 20 <= x < 80 and f in ("34", "36") and c != " "]
    if not sk:
        return None
    bottom = max(y for y, _ in sk)
    xs = [x for y, x in sk if y == bottom]
    for y in range(bottom, len(rows)):
        # the hill is white; the matrix rain behind it is black and grey
        if any(x < len(rows[y]) and rows[y][x][0] != " " and rows[y][x][1] in ("97", "37", "39") for x in xs):
            return y - bottom
    return None

def status(rows):
    return "".join(c for c, _ in rows[-1]) if rows else ""

def jump(hill_downs):
    """From the hill list: pick a hill, ride, fly, land, and back to the list."""
    for _ in range(hill_downs):
        key("Down"); wait(0.35)
    wait(0.6); key("Enter"); wait(1.0); key("Enter")
    t0, flying, landed, low = time.time(), False, False, 0
    while time.time() - t0 < 12:
        rows = cells()
        g = gap(rows)
        if DEBUG: print(round(time.time() - t0, 2), g, status(rows)[40:], file=sys.stderr)
        if " m " in status(rows) + " ":
            break
        if g is not None and g > GAP + 2:
            flying = True
        # two low readings in a row, so a half-drawn frame cannot trigger it
        low = low + 1 if flying and g is not None and g <= GAP else 0
        if low >= 2 and not landed:
            key("Enter"); landed = True
        wait(0.03)
    wait(3.5)
    print(status(cells()).strip(), file=sys.stderr)
    key("q"); wait(1.2)
    for _ in range(hill_downs):
        key("Up"); wait(0.1)

wait(3.0)                       # the intro
key("Down"); wait(0.6); key("Enter"); wait(1.2)   # training
t0 = time.time()
for n in (3, 0, 7, 1, 5):       # czumulungma k150, be free! k100, tarnica k180 ...
    if time.time() - t0 > secs:
        break
    jump(n)
for _ in range(8):              # down the hill list to "<<(", then "quit"
    key("Down"); wait(0.15)
key("Enter"); wait(0.8)
for _ in range(3):
    key("Down"); wait(0.3)
key("Enter")
