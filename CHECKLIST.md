# CHECKLIST: Misty editor (Kate fork)

Companion to [`KANBAN.md`](KANBAN.md). Sections match card IDs. Tick only what you verified.
The full feature list for every engine feature is in erratas/misty-ascii `CHECKLIST.md`; this file
lists what the editor itself must do.

## M0 Agent files

- [x] AGENTS.md, KANBAN.md, CHECKLIST.md, CLAUDE.md pointer

## B1 Dev build

- [x] Qt 6.11.3 prebuilt installed
- [x] KDE Frameworks 6.31 built (plus Qt Multimedia and Qt Speech, which ktexteditor needs)
- [x] Editor builds against them (2026-10-09, `.kdebuild/kate-build/bin/kate`)
- [x] Editor starts headless: `kate --version -platform offscreen` prints `kate 26.11.70`
- [ ] A screenshot shows the main window (not done yet)
- [ ] ASCII Fun plugin from PR #1 builds and loads

## K0 Plugin on the shared engine

- [ ] Plugin links misty-ascii; charts, banners, tables, image, braille, matrix still work
- [ ] Each action has an options dialog (height, width, style, threshold, colours)
- [ ] Unit tests for every action

## K1 Diagrams

- [ ] Tools menu entries for all 9 Diagon types (math, sequence, tree, table, frame, DAG graph,
      planar graph, flowchart, grammar) on the selection, with their options
- [ ] Live preview pane: source on the left, rendered art on the right, updates while typing
- [ ] Render-in-place fenced blocks (` ```diagon flowchart `) that toggle between source and art
- [ ] Diagon and ANTLR bundled in the build; no network needed at build time

## R1 Rename to Misty (scope to confirm with the owner)

- [ ] Decide scope: window title and About box only, or also the binary name, app id
      (`org.kde.kate`), config folder, desktop file, icons and translations
- [ ] Keep user settings working after the rename (migrate the old config folder)
- [ ] Misty branding: name, icon, splash, colour theme
- [ ] Update docs and the README

## K2-K7 Editor UI for engine features

- [ ] K2 tables, lists, trees, progress bars, reformatter
- [ ] K3 charts with all options
- [ ] K4 font gallery, banners, comment banners, colours, gradients, ANSI viewer and export
- [ ] K5 images and video
- [ ] K6 ASCII paint mode, art library, idle screen
- [ ] K7 open/save Misty canvas documents; agents' writes appear live
- [ ] Minimap still works after every change
