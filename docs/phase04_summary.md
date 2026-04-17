# Phase 4 Summary: Level Progression

## Phase Goal
Implement full progression across exactly three authored levels:

- detect level completion reliably
- reveal ladder after all gold is collected
- transition level 1 -> 2 -> 3
- enter a final campaign win state after level 3
- validate that the top row has at least one empty cell so ladder exit usage is possible

Exit target for this phase was: player can complete all three levels in sequence.

## Implementation Completed

### 1. Core Progression State
Updated core status model:

- `src/core/GameStatus.h`

Added `GameStatus::LevelComplete` to represent successful completion of the current level without ending the campaign.

In `src/core/GameSession.h` / `src/core/GameSession.cpp`:

- added `isLevelComplete()` query
- added `checkLevelCompletion()` as a dedicated completion rule check

### 2. Level Completion Rule
Implemented level completion logic in `GameSession`:

- completion can only happen when all gold is collected (`remainingGold == 0`)
- player must reach the top row (`row == 0`)
- completion is accepted only when ladder continuity exists below the player (`below == CellType::Ladder`)

This keeps completion behavior explicit and deterministic.

### 3. Top-Row Validation For Progression Safety
Updated level validation in:

- `src/model/Level.h`
- `src/model/Level.cpp`

Added:

- `Level::hasTopRowEmptyCell()`

Enforced during `Level::loadFromRows(...)`:

- level loading now throws if the first row has no empty cell (`.`)
- error message clearly explains ladder-exit viability requirement

This prevents authored levels that could block progression.

### 4. Three-Level Campaign Orchestration
Refactored startup and progression flow in:

- `src/main.cpp`

Implemented:

- strict authored-level count check (`levelDefinitionCount() == 3`)
- per-level `GameSession` creation by level id
- carry-over lives between levels
- automatic transition to next level on `LevelComplete`
- final campaign win flag after clearing level 3

Also kept game-over behavior unchanged and compatible with progression flow.

### 5. Renderer/HUD Progression Feedback
Updated renderer integration:

- `src/renderer/Renderer.h`
- `src/renderer/Renderer.cpp`
- `src/renderer/HudRenderer.h`
- `src/renderer/HudRenderer.cpp`

Added:

- `drawLevelCompleteOverlay(...)`
- `drawGameWonOverlay(...)`
- renderer branching for level-complete, game-over, and campaign-won overlays

Result: progression state is now visible and understandable to the player.

## Verification Performed

- Build completed successfully after Phase 4 integration (`Built target LoadRunner`).
- Runtime smoke launch (`timeout 6s ./build/LoadRunner`) started cleanly and printed bootstrap diagnostics without runtime exceptions.
- Diagnostics check for modified Phase 4 files reported no errors.

## Phase 4 Status Against Exit Criteria

Phase 4 baseline is complete:

- three authored levels are orchestrated in sequence
- level completion is detected through explicit rule checks
- hidden ladders are revealed once gold is fully collected
- campaign enters final win state after the third level
- top-row empty-cell validation is enforced at load time

## Notes For Next Phase

- Phase 5 work remains: persistence and stats storage (`steps`, `completion time`, `lives used`) per level.
- Add focused automated tests for progression transitions and top-row validation behavior.
