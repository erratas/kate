# KANBAN: Misty editor (Kate fork)

Mandatory for all agentic work here. Read [`AGENTS.md`](AGENTS.md) first. Master board for the
whole ASCII programme: erratas/misty-ascii `KANBAN.md` (same card IDs).

Last updated: 2026-10-09 by Claude Code (cloud session "determined-newton").

## Blocked (waiting on someone)

- `X4` **Review the ASCII Fun plugin PR** `[owner]`: erratas/kate#1 (draft, from an earlier
  session). New editor work builds on its branch until it merges.

## In progress

- `B1` **Dev build in the cloud container**: Qt 6.11.3 + KDE Frameworks 6.31 + ktexteditor built,
  and the editor compiles and starts (`--version` works). Left: a main-window screenshot and the
  ASCII Fun plugin build. The cloud container is temporary: redo the build on a new machine with
  the steps in `AGENTS.md` section 4 (ktexteditor also needs Qt `qtmultimedia` and `qtspeech`).
- `M0` Agent files for this repo (this PR).

## Next

1. `K0` ASCII Fun plugin on the shared misty-ascii engine (all existing actions keep working and
   gain their options).
2. `K1` Diagon diagrams bundled in the plugin: all 9 types, live preview pane, render-in-place
   blocks.
3. `R1` **Rename the editor to Misty**: scope to be confirmed with the owner (see CHECKLIST).

## Backlog

- `K2` Tables, lists, trees, progress bars, table reformatter.
- `K3` Charts with every option.
- `K4` Fonts gallery, banners, comment banners, colours and gradients, ANSI viewer and export.
- `K5` Images and video to ASCII.
- `K6` ASCII paint mode, art library, idle screen (matrix rain, star sky).
- `K7` Misty canvas link: open and save canvas documents, see agents' writes live.

## Ideas (brainstorm, not scheduled)

Fun ideas from the 2026-10-09 session. Technical notes for several of them: [`docs/misty/TECH-NOTES.md`](docs/misty/TECH-NOTES.md). Not promised work: the owner picks which become cards.
Ideas that touch other repos also belong on the master board in erratas/misty-ascii.

Editor (Misty):
- `I1` **Typewriter mode**: typing in the editor plays the canvas typewriter sounds and the
  carriage-return "ding" at the line end; mute and volume in settings.
- `I2` **Banner on save**: optional ASCII banner header for new files (project name in a chosen
  font, comment style matched to the language).
- `I3` **Minimap art**: the minimap can show a big banner or logo as a landmark you can click.
- `I4` **ASCII diff view**: show a diff as a box-drawn side-by-side table.
- `I5` **Live chart of a selection**: select a column of numbers and see a sparkline in the
  status bar; click to insert the full chart.
- `I6` **Colour picker that speaks ANSI**: pick a colour, insert it as hex, ANSI 256 or true-colour
  escape for the current language.
- `I7` **Box-drawing autocomplete**: type `+--` and get a clean box; arrow keys extend lines and
  fix the joins.
- `I8` **Fortune on startup**: a random tip or art piece from the art library on the start page.
- `I9` **Night sky screensaver** when idle, with real star positions for your city (card T2).

Agents and canvas:
- `I10` **Agent signatures**: each agent (Hermes E15, Venus, Claude Code) gets its own colour,
  font banner and typing speed when it writes to the canvas.
- `I11` **Replay**: play back a canvas document's history as a typewriter film; export it as an
  animated GIF or an `.ans` file.
- `I12` **Two agents, one canvas**: side-by-side cursors with names, like a shared editor.
- `I13` **Agent status in ASCII**: a small live table of running agents with progress bars.

Terminals and CLIs:
- `I14` **`misty` one-liners**: `misty banner`, `misty table`, `misty chart`, `misty sky`,
  `misty rain` in every CLI and AI-SUITE terminal, from one portable binary.
- `I15` **Themed prompts**: shell prompt themes built from the engine's colours and gradients.
- `I16` **Login splash** for E15 Misty and Venus Misty: a gradient banner plus system stats.
- `I17` **Pipe anything to a chart**: `some-command | misty chart` with auto-detected columns.
- `I18` **Weather and calendar widgets** drawn in ASCII for the terminal header.

Just for fun:
- `I19` **ASCII pet** that lives in the status bar and reacts to build failures.
- `I20` **Daily art drop**: one new piece in the art library every day, made by an agent.
- `I21` **Hidden easter egg**: Konami code in the editor starts matrix rain.

Round 2 (same session, later):
- `I22` **Photo booth**: webcam to live ASCII video in a canvas panel; snap a frame into the doc.
- `I23` **ASCII Pong / Snake** in a split view, played with the arrow keys; agents can play too.
- `I24` **Mood lighting**: the editor theme gradient shifts with the time of day.
- `I25` **Commit art**: a tiny ASCII badge in commit messages showing lines added and removed.
- `I26` **Music visualiser**: when audio plays, a bar chart dances in the status bar.
- `I27` **Sticker book**: drag ready-made ASCII stickers (cats, rockets, frames) into any doc.
- `I28` **Font forge**: draw a new banner font letter by letter in a grid and save it to the
  engine's font library.
- `I29` **Glitch effect**: one keypress makes selected text glitch and settle, for demos.
- `I30` **Fireworks** in the canvas when a long build or agent task finishes.
- `I31` **Story mode**: agents write a choose-your-own-adventure in the canvas, the user picks
  the path with number keys.
- `I32` **ASCII map**: city or world map from the astroterm data, with a "you are here" marker.
- `I33` **Clock wall**: big banner-font clocks for several time zones in a side panel.
- `I34` **Rubber duck**: an ASCII duck that asks "what did you expect to happen?" when you stare
  at the same line for too long.
- `I35` **Postcard export**: render any canvas page as a PNG postcard with a frame and caption.

## Done

- `D0` Earlier session: ASCII Fun plugin (charts, 10 banner fonts, tables, image to ASCII and
  braille, matrix rain) in erratas/kate#1 (draft, unmerged).

## Session log

- 2026-10-09 | Claude Code cloud (determined-newton) | this board; dev build started (Qt 6.11 +
  KF 6.31) | KF 6.24 does not build on Qt 6.8; container restart paused the build | finish B1,
  then K0/K1.
- 2026-10-09 | Claude Code cloud (determined-newton) | KF 6.31, ktexteditor and the editor built;
  editor starts headless | cloud credits nearly gone, session stopped | screenshot, plugin build,
  then K0/K1 (the engine in misty-ascii is not written yet: card E1 there comes first).
