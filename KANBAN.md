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
