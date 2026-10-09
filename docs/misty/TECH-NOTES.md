# Misty: technical notes for the first ideas

Status: draft design notes (2026-10-09). Nothing here is built. Idea IDs (`I1`...) are in
[`KANBAN.md`](../../KANBAN.md), section "Ideas". Engine card IDs (`E1`...) are on the master board
in erratas/misty-ascii.

## 1. Where things live (one engine, three ways in)

```
            erratas/misty-ascii  (C++17, no Qt, no network)
            ├─ libmisty-ascii    text, colour, tables, charts, fonts, images
            └─ misty             CLI, built with cosmocc into one portable binary
                    │
  ┌─────────────────┼───────────────────────────┬──────────────────────────────┐
  │ links the       │ runs the binary           │ runs the binary or calls     │
  │ library         │                           │ the misty-os HTTP API        │
  ▼                 ▼                           ▼                              │
Misty editor     Hermes CLI (E15, Venus),    misty-os GUI server (Python)     │
(this repo,      cli, cli-go, cli-js,        services/gui/server.py           │
 addons/)        cli-react, AI-SUITE          /api/canvas/* endpoints  ◄───────┘
                 terminals                    (agents write here)
```

Rules:
- **The editor links the library** (static, built as a CMake subproject). No process spawn per
  keystroke; works offline.
- **Everything else runs the `misty` binary.** One file, same on Linux, Windows and macOS
  (Cosmopolitan "APE" format). Go, Node, Python and Rust callers only need `exec`.
- **Agents never draw on the canvas themselves.** They send text to `POST /api/canvas/write`
  (already exists, `services/gui/server.py` line ~4825); the GUI and the editor render it.

## 2. CLI compatibility contract (applies to every `misty` subcommand)

This is the part every CLI depends on, so it is fixed first and covered by tests.

| Topic | Rule |
|---|---|
| Input | Data on stdin or as a file argument; text as positional args. `-` means stdin. |
| Output | Art on stdout only. Errors and hints on stderr. |
| Machine mode | `--json` prints `{"ok":true,"text":"...","width":N,"height":N}`; errors as `{"ok":false,"error":"..."}`. Callers in Go/Node/Python parse this instead of scraping. |
| Exit codes | 0 ok, 1 bad input, 2 bad usage, 3 feature unavailable (for example no image decoder). |
| Width | `--width N`; else `$COLUMNS`; else the terminal size; else 80. Never wrap when piped unless asked. |
| Colour | `--color auto|never|16|256|truecolor`. `auto` honours `NO_COLOR`, `FORCE_COLOR`, `COLORTERM=truecolor`, and turns colour off when stdout is not a terminal. |
| Unicode | `--ascii` forces plain ASCII (old Windows consoles, logs). Default is UTF-8 box drawing. |
| Windows | Enable virtual terminal output at start (Cosmopolitan does this); fall back to `--ascii` if it fails. Test in cmd.exe, PowerShell and Windows Terminal. |
| Speed | Start under 20 ms for small jobs, so shells can call it in a prompt. |
| Versioning | `misty --version`; `misty capabilities --json` lists subcommands, fonts and styles so callers can feature-check instead of pinning versions. |

Subcommands planned: `banner`, `table`, `chart`, `spark`, `bar`, `color`, `gradient`, `image`,
`diagram`, `rain`, `sky`, `fonts`, `capabilities`.

Example calls from each CLI:
- Go: `exec.Command("misty","table","--json","--style","rounded").Stdin = csv`
- Node: `execFileSync("misty", ["chart","--height","8"], {input: numbers})`
- Python (Hermes): `subprocess.run(["misty","banner","--font","block","Venus"], capture_output=True)`
- Shell: `ps aux | misty table --sort 3 --dsc`

Open questions: where the binary is installed on each machine (`~/.local/bin`? bundled per CLI?);
whether Hermes may call it on every reply or only on request (latency budget).

## 3. Deep dives

### I14: `misty` one-liners in every terminal
- **Where:** misty-ascii `cli/main.cpp`, one function per subcommand calling the library.
- **Build:** `cosmoc++ -O2 -o misty cli/*.cpp src/*.cpp`; CI uploads the binary as a release asset.
- **Install:** each CLI repo gets a tiny `misty_path()` helper: env `MISTY_BIN`, then `PATH`,
  then a bundled copy. If missing, print one hint and continue without art (never crash).
- **Tests:** golden files (`tests/golden/*.txt`) compared byte for byte, run on all three OSes.
- **Size:** first: banner, table, chart, spark. Then the rest.

### I1: typewriter sounds while typing in the editor
- **Source of truth for the sound:** misty-os `services/gui/static/misty-typewriter.js`. It
  synthesises every sound (no audio files): key clack, escapement tick, bell, carriage return.
- **Editor side:** new plugin `addons/mistyfun/` (or inside the ASCII Fun plugin).
  - Listen to `KTextEditor::Document::textInsertedRange` (key) and newline inserts (return).
  - Bell when the cursor passes a set column (default 72), like a real typewriter.
- **Audio:** port the synthesis to C++ and play through Qt Multimedia `QAudioSink` with a small
  pull-mode generator. Qt Multimedia is already installed in the dev build (ktexteditor needed it).
  Simpler first version: render each sound once to a buffer at startup, play with `QSoundEffect`.
- **Settings:** on/off, volume, style (lever/basket like the web), bell column. Off by default.
- **Risks:** audio latency on Linux (PipeWire/Pulse); must never block typing. Mute when the
  window is not focused.

### I5: sparkline of selected numbers
- **Where:** same editor plugin. Connect `KTextEditor::View::selectionChanged`.
- **How:** pull numbers from the selection with a regex, call the engine's `sparkline(values)`,
  show the result in a small label added to the main window status bar.
- **Click** opens a dialog with the full `plot()` chart and options, then inserts it.
- **Limits:** skip selections over ~10 000 numbers; debounce 150 ms.

### I10: agent signatures on the canvas
- **Already there:** `POST /api/canvas/write` takes `author`; `GET /api/canvas/activity` returns
  who wrote what since a time.
- **Add:** an authors registry in misty-os (`services/canvas/authors.json`):
  `{"hermes-e15": {"color":"#7fd1ff","font":"block","cps":45,"sound":"lever"}, ...}`.
  - The web canvas reads it to colour and pace the typed text (card C2).
  - The editor plugin polls `/api/canvas/activity` every 2 s (or a later websocket) and
    highlights each author's lines in their colour with `KTextEditor::MovingRange` attributes.
- **MCP tool (card C1):** `canvas_write(text, canvas?, doc?, author, mode?)` wraps the same
  endpoint, so Claude Code, Hermes and sub-agents all go through one door.
- **Open question:** who may register a new author (any agent, or only the owner)?

### I7: box drawing that finishes itself
- **Where:** editor plugin, a key event filter on the view.
- **Rule set:** typing `+--` or `┌─` starts a box; arrow keys with a modifier extend the line;
  on each change, recompute the joins of the 3x3 neighbourhood with a lookup table
  (`┬ ┴ ├ ┤ ┼` and the ASCII `+`). Same table lives in the engine so the CLI can "clean" boxes:
  `misty boxfix < sketch.txt`.
- **Risk:** must not fight normal typing. Only active in "ASCII paint" mode (card K6).

### I11: replay a document as a typewriter film
- **Data:** canvas activity events already have times, authors and text.
- **Format:** export as asciicast v2 (`.cast`, plain JSON lines; plays in `asciinema` and many web
  players) and as `.ans`. GIF later, rendered by the engine to frames.
- **Where:** misty-os endpoint `GET /api/canvas/{doc}/replay.cast`; the web canvas plays it with
  the typewriter sounds; the CLI gets `misty play file.cast`.

### I22: webcam photo booth
- **Editor:** Qt Multimedia `QCamera` + `QVideoSink` gives frames; convert each frame with the
  engine's image-to-ASCII (card E5) at ~10 fps into a read-only panel; "snap" inserts the frame.
- **Web canvas:** `getUserMedia` in the browser, same algorithm ported to JS or a server call.
- **Privacy:** camera only on explicit click, with a visible "camera on" marker.

## 4. Order of work

1. Engine E1 (text, colour, tables, charts) + CLI contract tests (section 2).
2. I14 first four subcommands, release binary, wire into one CLI (Hermes) as the pilot.
3. Editor plugin skeleton linking the engine (K0), then I5 (small, proves the link).
4. C1 MCP tool + I10 authors registry; then I1 sounds (needs C++ port of the synth).
5. I7, I11, I22 after that.

## 5. Things we still have to decide (owner)

- Install location and update method for the `misty` binary on each machine.
- Licence for the engine (owner's choice; all code is ours).
- Whether the editor plugin is one big "Misty" plugin or several small ones.
- How far the rename to Misty goes (card R1): title only, or binary name, app id and config.
