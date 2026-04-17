# Phase 2 Summary: Core Gameplay Rules

## Phase Goal
Implement the core gameplay rules in C++ without renderer-side hacks:

- movement and gravity
- gold collection and teleport with fixed per-level uses
- enemy collision handling
- life loss, fixed spawn point respawn, and game over
- horizontal acceleration while left/right is held

## Implementation Completed

### 1. New Core Gameplay Layer
Added a dedicated gameplay orchestrator in `src/core`:

- `src/core/InputCommand.h`
- `src/core/GameStatus.h`
- `src/core/GameSession.h`
- `src/core/GameSession.cpp`

`GameSession` now owns runtime gameplay state transitions and per-tick rule execution, while keeping logic independent from SFML rendering objects.

### 2. Movement And Gravity
Implemented in `GameSession`:

- discrete grid movement through command processing
- passability checks via `Level::cellAt` and `isPassable`
- gravity application after movement updates

### 3. Gold Collection And Teleport Rules
Implemented in `GameSession`:

- gold pickup increments `player.goldCollected`
- collected gold cell is converted to `CellType::Empty`
- remaining gold tracking (`remainingGold`)
- hidden ladders are revealed when remaining gold reaches zero
- teleport consumes fixed per-level uses from `Level::teleportUses()`
- teleport is rejected when no uses remain
- teleport destination is selected from valid passable, non-enemy cells

### 4. Enemy Collision, Lives, Respawn, Game Over
Implemented in `GameSession`:

- simple enemy patrol update (v1 horizontal patrol, direction flip on wall)
- player-enemy collision detection
- life decrement on hit
- fixed spawn point respawn while lives remain
- transition to `GameStatus::GameOver` at zero lives

### 5. Horizontal Acceleration
Implemented in `GameSession`:

- held horizontal input axis (`-1/0/+1`)
- acceleration and deceleration constants
- max speed clamp
- deterministic tile-step movement via horizontal accumulator

### 6. Main Loop Integration
Updated `src/main.cpp` to use `GameSession`:

- fixed-step update loop (60 Hz)
- command mapping:
  - `W` -> move up
  - `S` -> move down
  - `Space` -> teleport
- held input mapping:
  - `A`/`D` -> horizontal acceleration axis
- basic runtime console diagnostics for game-over stats

### 7. Build Integration
Updated `CMakeLists.txt` to compile the new core source:

- added `src/core/GameSession.cpp`

## Verification Performed

- CMake configure and build completed successfully.
- Runtime smoke test completed successfully.
- Startup shows level/session bootstrap data (dimensions, spawn, gold count, teleport uses, enemy count).
- No compile/diagnostic errors in modified Phase 2 files.

## Current Status Against Phase 2 Exit Criteria

Phase 2 mechanics are now implemented in a dedicated core gameplay module and run without renderer-specific hacks.

## Notes For Next Phase

- Rendering and on-screen gameplay visualization remain Phase 3 work.
- Input abstraction (`src/input`) can be extracted from `main.cpp` in the next iteration.
- Add automated tests for `GameSession` transitions per `loadrunner-cpp-testing.instructions.md`.
