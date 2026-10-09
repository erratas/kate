# Prompt for the next session (copy everything below the line)

Written 2026-10-09 at the end of cloud session "determined-newton" (credits ran out). It carries
everything the next agent needs, including the engine API design that was never committed anywhere
else.

---

You are continuing the Misty ASCII programme for the owner (GitHub: `erratas`). Work fast and
well; the owner has approved the whole plan below and wants working, verified results pushed to
GitHub. Write short, plain replies. Never use em dashes or en dashes anywhere (owner rule).

## 0. First 10 minutes (do these in order)

1. Clone or open these repos: `erratas/misty-ascii`, `erratas/kate`, `erratas/misty-os`
   (later also `erratas/hermes-agent`, `cli`, `cli-go`, `cli-js`, `cli-react`).
2. In each repo check out branch `claude/determined-newton-9r2b34` if you continue that work, and
   read `AGENTS.md`, `KANBAN.md`, `CHECKLIST.md` (mandatory rituals: move your card to In progress,
   tick only verified items, add a session-log line, commit the board with your work).
3. Read `erratas/kate` file `docs/misty/TECH-NOTES.md` (architecture, CLI contract, deep dives).
4. Check live state before trusting any board: open PRs, CI status, whether a dev build exists.
5. If a plugin called **groundwork-specflow** is installed and blocks edits, tell the owner in one
   line: turn it off, or run `/groundwork-specflow:bypass <reason>` with the session's working
   directory set to the repo being edited (a bypass only applies to the current folder, 60 min).
   Do not work around it by writing through the GitHub API.

## 1. What exists today (all draft PRs, none merged)

| PR | What it holds |
|---|---|
| erratas/misty-os#1 (branch `fix/subtab-alignment`) | GUI subtab alignment in shared CSS, vitals box renamed TELEMETRY / "Hardware telemetry", bottom bar no longer covers content, offline embed card with Retry (no-cors probe), Playwright check `tests/gui/test_subtab_alignment.py` at 1400x900 and 1024x768, 22 screenshots in `docs/pr/subtab-alignment/` |
| erratas/misty-os#2 | AGENTS.md, KANBAN.md, CHECKLIST.md for misty-os |
| erratas/kate#1 (branch `claude/beautiful-bardeen-ybga5f`) | ASCII Fun plugin `addons/asciifun/`: charts, 10 banner fonts, tables, image to ASCII and braille, matrix rain |
| erratas/kate#2 | Agent files, board with cards and 35 ideas (`I1`-`I35`), `docs/misty/TECH-NOTES.md`, this prompt |
| erratas/misty-ascii#1 | Master AGENTS.md, KANBAN.md, CHECKLIST.md (full feature inventory per source repo), CREDITS.md, README |

Known red CI on misty-os (pre-existing on `main`, not caused by our PRs): 556 ruff lint errors and a
stub-inventory strict check. Owner has not decided on a cleanup PR; do not fix it unasked.

Not saved anywhere: the compiled Qt / KDE Frameworks / Kate build (cloud container is gone). The
engine code was designed but never written (section 3 has the design).

## 2. Owner decisions already made (do not re-ask)

- Kate fork is being **renamed to Misty**. It must become extremely feature rich in ASCII art,
  fonts, colours, text, graphing, diagrams. Keep the minimap.
- "Nothing left behind": every feature, font, colour, text and graphing capability from these
  repos goes into the engine: cfonts, astroterm, Diagon, ASCII-generator, FTXUI, go-pretty,
  cosmopolitan (toolchain, must have), ascii-art, ASCII-Data, asciigraph, asciichart,
  ascii-view, asciichart1, asciigraph-rs, ascii-image-converter, cmatrix. The per-repo feature
  inventory is in misty-ascii `CHECKLIST.md`.
- **One shared engine** in `erratas/misty-ascii` (private): a C++ library plus a CLI built with
  Cosmopolitan into one binary that runs on Linux, Windows and macOS.
- Engine plus Cosmopolitan first; Diagon compiled into the editor plugin; build Qt and KDE from
  source when verifying the editor.
- All brainstorm groups are in scope: live preview pane, render-in-place blocks, ASCII paint,
  table reformat, banners, ANSI viewer, art library, idle screen. Later: AI-SUITE terminals,
  astroterm star-sky screen.
- Any agent (Hermes, Claude Code, sub-agents) must be able to write to the Misty canvas, typed out
  with the typewriter animation and sounds.
- Integrate with Hermes CLI on E15 Misty and Venus Misty, the AI-SUITE terminals, and the other
  CLIs.
- Licensing: all code we write is the owner's. Rebuild features from GPL projects (cfonts,
  cmatrix, Stellarium constellation data) from scratch and draw our own fonts in their styles; do
  not copy their code or glyph files verbatim. MIT/BSD/Apache sources may be ported with a line
  in `CREDITS.md`. Do not ship `ascii-generator/fonts/arial-unicode.ttf` or `simsun.ttc`.
  astroterm city data needs a GeoNames CC-BY 4.0 credit.
- Branch plus draft PR for every change; never push to `main`/`master`.

## 3. Engine API design (never committed; implement this in misty-ascii card E1-E3)

Header `include/misty/ascii.hpp`, namespace `misty::ascii`, C++17, no Qt, no network:

```cpp
// Text
int displayWidth(const std::string&);            // East Asian wide = 2, ANSI escapes = 0
std::string stripAnsi(const std::string&);
enum class Align { Left, Center, Right, Auto };   // Auto: numbers right, text left
std::string pad(const std::string& s, int width, Align a);
std::string snip(const std::string& s, int width, const std::string& indicator = "~");
std::vector<std::string> wrap(const std::string& s, int width);
std::vector<std::string> lines(const std::string& s);

// Colour
struct Rgb { int r, g, b; };
enum class ColorDepth { None, Basic16, Ansi256, TrueColor };
std::optional<Rgb> parseColor(const std::string& name);   // names, #hex, rgb()
std::vector<std::string> colorNames();
std::string colorize(const std::string& text, Rgb fg, ColorDepth d, std::optional<Rgb> bg = {});
std::string gradient(const std::string& text, const std::vector<Rgb>& stops, ColorDepth d); // HSV
ColorDepth detectColorDepth(bool isTty);          // honours NO_COLOR, FORCE_COLOR, COLORTERM

// Tables (go-pretty feature set)
enum class TableStyle { Default, Light, Rounded, Bold, Double, Ascii, None };
enum class TableFormat { Text, Markdown, Csv, Tsv, Html };
enum class SortMode { None, Asc, Dsc, AscNumeric, DscNumeric };
struct ColumnConfig { Align align = Align::Auto; int widthMax = 0; bool hidden = false; };
struct TableOptions {
  TableStyle style = TableStyle::Light; TableFormat format = TableFormat::Text;
  bool header = true, footer = false, separateRows = false, border = true, autoIndex = false;
  int sortColumn = -1; SortMode sortMode = SortMode::None;
  std::string title, caption; std::vector<ColumnConfig> columns;
};
using Row = std::vector<std::string>;
std::string renderTable(const std::vector<Row>& rows, const TableOptions& opt);
std::vector<Row> parseDelimited(const std::string& text, char delimiter = 0); // auto-detect,
                                                   // CSV quotes, markdown tables
std::vector<std::string> tableStyleNames();
std::optional<TableStyle> parseTableStyle(const std::string&);

// Charts (asciichart algorithm, plus asciigraph/ASCII-Data extras)
struct ChartOptions {
  int height = 10, width = 0; std::optional<double> min, max;
  int offset = 3, precision = 2; std::string caption;
  std::vector<std::string> legends; std::vector<Rgb> colors; ColorDepth depth = ColorDepth::None;
};
std::string plot(const std::vector<std::vector<double>>& series, const ChartOptions& opt); // NaN = gap
std::string sparkline(const std::vector<double>& values);
std::string barChart(const std::vector<std::string>& labels, const std::vector<double>& values,
                     int width = 40);
std::vector<std::vector<double>> parseSeries(const std::string& text);
```

Files: `src/text.cpp`, `src/color.cpp`, `src/table.cpp`, `src/chart.cpp`, `cli/main.cpp`,
`tests/` (golden files compared byte for byte), `CMakeLists.txt` (library + CLI + tests, works as a
subproject for the editor), `scripts/build-cosmo.sh`, `.github/workflows/ci.yml` (Linux build +
tests, cosmocc build, upload the `misty` binary as an artifact).

Then E4 fonts (redraw cfonts/figlet-style fonts ourselves: block, chrome, 3d, simple, shade,
slick, huge, grid, pallet, tiny, plus banner styles), E5 images (ascii-image-converter,
ascii-view, ASCII-generator features: ramps, braille, colour, edge mode, dithering), E6 fun
(matrix rain with all cmatrix options rebuilt, star sky), E7 colour extras, E8 diagrams (Diagon
port or link: math, sequence, tree, table, frame, DAG, planar graph, flowchart, grammar).

## 4. CLI contract (summary; full table in kate `docs/misty/TECH-NOTES.md` section 2)

Art on stdout, errors on stderr; `--json` returns `{"ok":true,"text":...,"width":N,"height":N}`;
exit codes 0 ok, 1 bad input, 2 bad usage, 3 unavailable; width from `--width`, `$COLUMNS`, the
terminal, else 80; `--color auto|never|16|256|truecolor` with `NO_COLOR`/`FORCE_COLOR`; `--ascii`
fallback; `misty capabilities --json`; start under 20 ms. Subcommands: `banner table chart spark
bar color gradient image diagram rain sky fonts capabilities`.

## 5. Work queue (do in this order; one card ~ one PR)

1. **E1-E3 engine core + CLI** in misty-ascii (section 3 and 4). Verify: `ctest` green, the
   cosmocc binary runs (`./misty table < x.csv`), golden tests pass. Update the master board.
2. **Master board catch-up** in misty-ascii: add per-repo board links (misty-os#2, kate#2), card
   R1 (rename to Misty), mark M0 done, B1 status, copy ideas `I1`-`I35` from kate `KANBAN.md`.
3. **B1 dev build** of the editor (kate repo `AGENTS.md` section 4): Qt 6.11.3 via aqtinstall
   (also modules `qtmultimedia qtspeech`), KDE Frameworks 6.31 from tarballs with
   `-DWITH_WAYLAND=OFF -DKWINDOWSYSTEM_WAYLAND=OFF -DBUILD_TESTING=OFF -DBUILD_QCH=OFF`, CMake 3.30+,
   knotifications before kjobwidgets. KF 6.24 does NOT build on Qt 6.8. Last time the editor built
   and `kate --version -platform offscreen` printed `kate 26.11.70`. Still to verify: a main-window
   screenshot and the ASCII Fun plugin build.
4. **K0** ASCII Fun plugin on the shared engine (link misty-ascii as a CMake subproject); every
   action gets an options dialog; unit tests.
5. **K1** Diagon in the plugin: all 9 diagram types, live preview pane, render-in-place fenced
   blocks (```` ```diagon flowchart ````). Diagon builds with `-DANTLR_BUILD_CPP_TESTS=OFF`.
6. **C1** in misty-os: MCP tool `canvas_write(text, canvas?, doc?, author, mode?)` plus
   `canvas_read`/`canvas_list`, wrapping the existing `POST /api/canvas/write`
   (`services/gui/server.py` ~line 4825) and `GET /api/canvas/activity`; Hermes tool registration;
   a Claude Code skill documenting it. Test: a tool call lands text in the open document.
7. **C2** typed agent writes: typewriter animation + synth sounds from
   `services/gui/static/misty-typewriter.js`, author colours, reduced-motion fallback, speed-up for
   long writes, never overwrite unsaved user edits.
8. **H1** Hermes CLI (E15 Misty, Venus Misty) uses the `misty` binary: banners, tables, charts,
   login splash (idea I16).
9. **R1** rename to Misty: ask the owner the scope first (title/About only, or also binary name,
   app id `org.kde.kate`, config folder, desktop file, icons); migrate old settings.
10. **K2-K7** editor UI for every engine feature; then ideas in the order given in
    `docs/misty/TECH-NOTES.md` section 4 (I14, I5, I10, I1, I7, I11, I22).
11. **T1-T3** AI-SUITE terminals and the star-sky idle screen (own constellation data).
12. Other CLIs (`cli`, `cli-go`, `cli-js`, `cli-react`): a small `misty_path()` helper (env
    `MISTY_BIN`, then PATH, then bundled copy) and one pilot command each.

## 6. Open owner questions (ask with a picker, recommendation first, only when you reach them)

- Rename scope (R1).
- Install location and update method for the `misty` binary.
- Engine licence.
- One big Misty editor plugin or several small ones.
- misty-os lint cleanup PR: yes or no.

## 7. Hard-won gotchas

- misty-os `vendor/` is gitignored; the GUI Playwright test skips without gridstack, leaflet,
  katex, mermaid, xterm, three. Download them locally to run it (Playwright 1.56 matches the
  preinstalled Chromium).
- misty-os `app.js` binds subtab handlers late; tests must wait for them.
- `cosmoc++` does not read source from stdin; pass a file. Get cosmocc from
  https://cosmo.zip/pub/cosmocc/cosmocc.zip.
- Diagon pulls ANTLR at configure time; disable its tests or the GoogleTest download fails behind a
  proxy.
- `grep -c` returns exit 1 on zero matches; it once silently skipped a commit in a `&&` chain.
- Verify copyright holders against each repo's LICENSE before writing CREDITS lines.
- Commit trailers and PR footers: follow the session's attribution instructions.
