# Phase 5 Summary: Persistence And Stats

## Phase Goal

Implement a lightweight per-level stats system that:

- saves completed-level outcomes to disk
- persists best values across sessions
- reloads best stats on startup and level transition
- displays best metrics in the HUD
- preserves the model/core vs renderer separation

Exit target for this phase: the game records best time, best steps, and run counts per level, persists them between restarts, and shows them in the HUD.

## Implementation Completed

### 1. Per-Level Stats DTO

Added:

- `src/persistence/LevelStats.h`

`LevelStats` is a pure-data struct (no SFML types) with:

- `levelId` — identifies which level the record belongs to
- `bestTimeMs` — fastest recorded completion time in milliseconds; 0 = not yet set
- `bestSteps` — fewest steps in a completed run; 0 = not yet set
- `totalRuns` — cumulative count of completed runs
- `totalLivesUsed` — cumulative lives spent across all completed runs

### 2. Persistence Module

Added:

- `src/persistence/StatsStorage.h`
- `src/persistence/StatsStorage.cpp`

`StatsStorage` namespace provides three functions:

- `loadAll(filePath)` — reads all per-level records from the stats file; returns empty vector if the file is absent or malformed
- `loadBest(filePath, levelId)` — returns the record for one level, or a zero-initialised default if not found
- `recordRun(filePath, levelId, timeMs, steps, livesUsed)` — records one completed run; updates `bestTimeMs` and `bestSteps` only when the new values are strictly better; increments `totalRuns` and accumulates `totalLivesUsed` unconditionally

**File format** (plain text, human-readable):

```
# loadrunner stats v1
<levelId> <bestTimeMs> <bestSteps> <totalRuns> <totalLivesUsed>
```

Lines beginning with `#` are comments. Malformed lines are silently skipped (first-run and corrupted-file safety).

**Save rule**: only completed runs are recorded. Game-over runs do not update best values.

### 3. CMakeLists.txt

Updated:

- `CMakeLists.txt`

Added `src/persistence/StatsStorage.cpp` to the `SOURCES` list so the persistence module compiles in all builds.

### 4. Main-Loop Wiring

Updated:

- `src/main.cpp`

Changes:

- `levelStartLives` captured when a new `GameSession` is created, for accurate `livesUsed` calculation
- `levelStatsSaved` flag prevents double-save if the level-complete frame is rendered more than once before transition
- `StatsStorage::recordRun` called exactly once per completed level, before transition logic
- `currentBestStats` loaded at startup via `StatsStorage::loadBest` and reloaded after every level transition
- `currentBestStats` passed to `Renderer::draw` so the HUD always shows the correct level's best

### 5. HUD Data Flow

Updated:

- `src/renderer/Renderer.h` — `draw(...)` signature extended with `const LevelStats& bestStats`
- `src/renderer/Renderer.cpp` — `bestStats` forwarded to `m_hud.draw(...)`
- `src/renderer/HudRenderer.h` — `draw(...)` signature extended with `const LevelStats& bestStats`; added `formatMs(long long)` helper
- `src/renderer/HudRenderer.cpp` — HUD now renders two rows within the 60 px HUD band:
  - **Row 1 (y=6, size 16)**: current-run stats: level, lives, gold, steps, time, teleports
  - **Row 2 (y=34, size 13)**: best-run stats: best time, best steps, total runs; shows `"Best: -- (no completed run recorded)"` on first run

Renderer is strictly presentation-only; no persistence logic touches `Renderer` or `HudRenderer`.

## Verification Performed

- Build completed successfully (`[100%] Built target LoadRunner`) after adding all Phase 5 files.
- `src/persistence/StatsStorage.cpp` compiled without warnings as part of the main target.
- Stats file `loadrunner_stats.dat` is created automatically on first completed run (first-run check).
- Incomplete runs (game over before level completion) do not call `recordRun`, preserving best values.
- `levelStatsSaved` flag ensures exactly one save per completed level even if the level-complete state persists across multiple rendered frames.
- `currentBestStats` is reloaded on every level transition so each level's HUD shows its own best.

## Phase 5 Status Against Exit Criteria

Phase 5 baseline is complete:

- per-level aggregate stats (`bestTimeMs`, `bestSteps`, `totalRuns`, `totalLivesUsed`) persist to disk
- best values only improve, never regress
- stats file is created automatically on first completed run; absence of file does not crash
- best stats are loaded at startup and after each level transition
- HUD displays both current-run metrics and best-run metrics in a compact two-row layout
- persistence logic stays fully outside the renderer

## Notes

- Stats file location is `loadrunner_stats.dat` in the working directory (build directory when launched from the build folder).
- Run-history analytics, cloud sync, and leaderboards are explicitly out of scope for Phase 5.
- No CPP_SFML_PORT_PLAN.md changes were required; all Phase 5 decisions align with the plan as written.
