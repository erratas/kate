# AGENTS.md: read this first (every agent, every session)

Mandatory reorientation for all agentic work in this repo: Claude Code sessions, Hermes agents,
sub-agents. Read it before you start; update the board before you stop.

This is our fork of KDE's Kate text editor. **It is being renamed to Misty**: the editor for the
Misty "god tier canvas" and the text side of our terminal engines, packed with ASCII art, fonts,
colours, diagrams, tables and charts from our shared engine (erratas/misty-ascii, built with
Cosmopolitan). The minimap stays: the owner likes it.

## 1. Start-of-session ritual

1. Read this file.
2. Open [`KANBAN.md`](KANBAN.md) and pick your card (or the one the owner named).
3. Open [`CHECKLIST.md`](CHECKLIST.md) at that card's section.
4. Check live state before trusting the board: open PRs, and whether the dev build exists.
5. Move your card to **In progress** with your session name and the date.

## 2. End-of-session ritual (never skip)

1. Tick only checklist items you verified (built it, ran it, looked at it).
2. Update your card: column, PR link, what is left, blocker and who must act.
3. Add one line to the Session log in `KANBAN.md`: date | who | changed | failed | next.
4. Commit both files with your work. Code changed without a board update is not done.

## 3. How this repo fits the wider programme

The master board lives in **erratas/misty-ascii** (`KANBAN.md`, `CHECKLIST.md`, `AGENTS.md`).
This repo's board holds only the cards that touch the editor, with the same card IDs.

| Area | Path | Notes |
|---|---|---|
| ASCII Fun plugin | `addons/asciifun/` (on branch `claude/beautiful-bardeen-ybga5f`, PR #1) | Charts, banners, tables, image to ASCII, matrix rain. New work builds on this branch until it merges. |
| App entry points | `apps/kate/`, `apps/kwrite/`, `apps/lib/` | Rename work (card `R1`) starts here |
| Minimap | KTextEditor (framework), shown via the view | Keep working; check it after every UI change |

## 4. Building (cloud sessions)

Ubuntu 24.04 lacks Qt 6.5+ and KDE Frameworks 6. The cloud dev build lives in
`/home/user/.kdebuild` (not in git; rebuild if the container is new):

- Qt 6.11.3 prebuilt via `aqtinstall` into `.kdebuild/qt/6.11.3/gcc_64`.
- KDE Frameworks 6.31 built from release tarballs into `.kdebuild/kf6b`
  (`-DWITH_WAYLAND=OFF -DKWINDOWSYSTEM_WAYLAND=OFF -DWITH_TEXT_TO_SPEECH=OFF -DBUILD_TESTING=OFF`;
  knotifications before kjobwidgets; CMake 3.30+ needed).
- Diagon builds with `-DANTLR_BUILD_CPP_TESTS=OFF` (GoogleTest download is blocked).
- Then: `cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="<qt>;<kf6b>"` and `ninja -C build`.
- Run headless with `-platform offscreen` for screenshots and tests.

Full build notes: erratas/misty-ascii `AGENTS.md`, section "Build notes".

## 5. Ground rules

- Our code is ours. Rebuild features from GPL-3 projects (cfonts, cmatrix) from scratch;
  credit permissive ports (see erratas/misty-ascii `CREDITS.md`).
- Keep KDE's existing licence headers on files we did not write.
- Branch + draft PR for every change; never push to `master`.
- No em dashes in anything we write (owner preference).
- Verify before ticking; say plainly what was not verified.
