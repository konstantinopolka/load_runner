# LoadRunner C# -> C++ (SFML) Port Plan

## Purpose

This document is the implementation guide for porting the existing C# desktop LoadRunner game to a simple, maintainable C++ version using SFML.

It is designed to be reused across the full development cycle.

## Scope And Non-Goals

- Scope:
  - Port current gameplay from C# to C++.
  - Implement improvements listed in `raw/Load Runner.md`.
  - Use architecture principles in `raw/cpp-sfml-project-summary.md`.
- Non-goals:
  - No ECS framework.
  - No scripting language.
  - No online/multiplayer features.
  - No advanced editor tooling in first iteration.

## Core Constraints

- Keep the project as simple as possible.
- Use SFML for rendering, input, timing, and window management.
- Keep game logic independent from SFML wherever practical.
- Organize code by feature/module with co-located `.h` and `.cpp` files.
- Prefer deterministic, testable logic in model/gameplay code.
- Store shipped levels as C++ source definitions to avoid runtime text parsing fragility.
- A local SFML source tree is available at `raw/SFML`; use it as a primary reference for API behavior, examples, and migration checks.

## Source Baseline (Current C#)

Primary source files in `raw/LoadRunner_csharp`:

- `Engine.cs`: movement, gravity, win/lose checks, teleport, pit behavior.
- `Area.cs`: grid storage and generation.
- `Cells.cs`: cell hierarchy and gameplay semantics.
- `MainForm.cs`: rendering and keyboard integration.

Current controls:

- Movement: `W`, `A`, `S`, `D`
- Dig pits: `Z` and `X`
- Teleport: `Space`

## Target Feature Set

### 1. Parity Features To Port First

- Grid-based level and movement.
- Cell types: empty, wall, ladder/stair, gold, teleport, player.
- Gravity/falling behavior.
- Gold collection and basic win/lose flow.
- Pit digging behavior (left/right).

### 2. Required Improvements (From Requirements)

Enemies:

- Enemy AI v1 is simple patrol only.
- Enemy speed stays low.
- If the player collides with an enemy, one life is lost.

Player:

- Add lives.
- Player can be killed by enemies.
- Use fixed spawn point respawn after enemy collision when lives remain.
- Horizontal movement accelerates while key is held.

Teleport:

- Use fixed per-level teleport uses.

Levels:

- Create exactly 3 hand-authored levels (no random generation in shipped game).
- Store levels in dedicated C++ level definition modules.
- Use minimal ASCII-only level definitions embedded in C++ source files.
- Support corridors with variable spacing where needed.
- Ensure ceiling cells are empty cells.
- Spawn a ladder after all gold is collected.

Stats:

- Track per-level step count.
- Track time to complete level.
- Track lives lost/used.
- Persist stats to disk.

Technical:

- Keep level update and timer in parallel execution paths.
- Avoid full map redraw every frame.
- Separate backend logic from GUI/rendering.

## Target Architecture

Use a simple game-loop architecture with clear boundaries:

- Model (`src/model`):
  - Pure gameplay data and rules (minimal or no SFML types).
  - Examples: `Cell`, `PlayerState`, `EnemyState`, `Level`, `GameRules`.
- Engine (`src/core`):
  - Frame update orchestration, collisions, win/lose, level transitions.
  - Examples: `Game`, `GameSession`, `GameStateMachine`.
- Input (`src/input`):
  - Translate SFML events to gameplay commands.
  - Example: `InputHandler`.
- Renderer (`src/renderer`):
  - All SFML drawing code, camera, textures, overlays.
  - Example: `Renderer`.
- States (`src/states`):
  - `MenuState`, `PlayState`, `PauseState`, `GameOverState`, `WinState`.

Rule: model/core logic must not directly depend on SFML rendering classes such as `sf::Sprite` and `sf::RenderWindow`.

## Proposed Folder Layout

```text
load_runner/
├── CMakeLists.txt
├── CPP_SFML_PORT_PLAN.md
├── assets/
│   ├── fonts/
│   ├── textures/
│   └── levels/
│       └── README.md
├── data/
│   └── stats.json
└── src/
    ├── main.cpp
    ├── core/
    │   ├── Game.h
    │   ├── Game.cpp
    │   ├── GameSession.h
    │   └── GameSession.cpp
    ├── model/
    │   ├── Cell.h
    │   ├── Cell.cpp
    │   ├── Level.h
    │   ├── Level.cpp
    │   ├── levels/
    │   │   ├── LevelDefinitions.h
    │   │   └── LevelDefinitions.cpp
    │   ├── PlayerState.h
    │   ├── PlayerState.cpp
    │   ├── EnemyState.h
    │   └── EnemyState.cpp
    ├── states/
    │   ├── IGameState.h
    │   ├── MenuState.h
    │   ├── MenuState.cpp
    │   ├── PlayState.h
    │   └── PlayState.cpp
    ├── renderer/
    │   ├── Renderer.h
    │   └── Renderer.cpp
    ├── input/
    │   ├── InputCommand.h
    │   ├── InputHandler.h
    │   └── InputHandler.cpp
    └── persistence/
        ├── LevelLoader.h
        ├── LevelLoader.cpp
        ├── StatsStore.h
        └── StatsStore.cpp
```

## C# -> C++ Mapping

- `Program.cs` -> `src/main.cpp`
- `MainForm.cs` -> `src/core/Game.*` + `src/renderer/Renderer.*` + `src/input/InputHandler.*`
- `Engine.cs` -> `src/core/GameSession.*` + `src/model/*` rules
- `Area.cs` -> `src/model/Level.*`
- `Cells.cs` -> `src/model/Cell.*` (+ optional derived types)

## Data Contracts

### Level Definitions (`src/model/levels/LevelDefinitions.cpp`)

Use minimal ASCII-only level definitions embedded in C++ source files, with one character per tile.

- `#`: wall
- `.`: empty
- `H`: ladder
- `G`: gold
- `P`: player spawn
- `E`: enemy spawn
- `T`: teleport
- `L`: hidden ladder spawn marker (becomes ladder after all gold is collected)

Rules:

- Exactly one `P` per level.
- At least one `G` per level.
- All rows in one level must have equal width.
- Top row should be mostly empty except explicit structures.
- No metadata headers or key-value sections are used.
- Teleport use counts are not encoded in ASCII map rows.

Example definition shape:

```cpp
const std::vector<std::string> kLevel01 = {
  "####################",
  "#..................#",
  "#.G..H..G..G...H...#",
  "#....H.........H...#",
  "####.H.####.##.H####",
  "#..G.H....#....H...#",
  "#....H..G.#..H.H...#",
  "####.H.##.#..#.H####",
  "#.P..H.E..#.H..H.T.#",
  "####################"
};
```

### Teleport Uses

- Use fixed per-level teleport uses.
- Keep teleport uses in code-side level configuration indexed by level id.
- No recharge mechanic is used.

## Level Storage Migration Plan (txt -> C++)

Immediate next plan after Phase 1 completion:

1. Add `src/model/levels/LevelDefinitions.h/.cpp` with three level maps as `std::vector<std::string>` constants.
2. Add a small level catalog API (for example `getLevelDefinition(int levelId)`) that returns map rows plus per-level config such as teleport uses.
3. Refactor `Level` loading entry point from file-path based loading to definition-based loading.
4. Keep existing row-width and tile validation logic, but validate in-memory map rows instead of text file lines.
5. Remove runtime dependency on external level txt files for shipped gameplay levels.

Exit criteria for this migration plan:

- App loads level 1 from C++ definitions without reading `assets/levels/level01.txt`.
- Validation errors still identify row/column issues clearly.
- Behavior parity for spawn points, gold count, and enemy spawns is preserved.

### Stats Format (`data/stats.json`)

Store per-level aggregate stats:

- `levelId`
- `bestTimeMs`
- `bestSteps`
- `totalRuns`
- `totalLivesUsed`

## Multithreading Plan (Simple)

Keep gameplay deterministic while satisfying the requirement that level and timer run in parallel:

- Main thread:
  - SFML event polling
  - fixed-step gameplay updates
  - rendering
- Timer thread:
  - increments elapsed time counters using `std::chrono`
  - writes to `std::atomic<int64_t>` elapsed milliseconds

Synchronization rules:

- Timer thread never mutates level entities.
- Shared timer values use atomics only.
- No locks needed in first iteration.

## Rendering Plan (No Full Map Redraw)

Use a two-layer renderer:

- Static layer (`sf::RenderTexture` cached): walls, ladders, other static tiles.
- Dynamic layer (draw per frame): player, enemies, collected-gold effects, UI text.

Update strategy:

- Rebuild static layer only when static tiles change (for example hidden ladder revealed).
- Draw dynamic entities every frame.

This keeps the implementation simple while avoiding full map redraw work each frame.

## Implementation Phases

### Phase 0: Project Bootstrap

- Create CMake project.
- Add SFML dependency.
- Build and run an empty window.

Exit criteria:

- App opens SFML window and closes cleanly.

### Phase 1: Domain Model Port

- Implement tile enum/cell model.
- Implement level data structure and validation.
- Implement player state (position, velocity, lives, step count).
- Implement enemy state (position, simple patrol only behavior).

Exit criteria:

- Level data loads correctly and validation errors are clear.

### Phase 1.5: Level Storage Migration

- Move authored levels from txt files to C++ level definition modules.
- Keep minimal ASCII-only level definitions in source.
- Keep parser validation semantics while switching from file I/O to in-memory definitions.
- Bind fixed per-level teleport uses in the same code-side level catalog.

Exit criteria:

- Runtime level loading no longer depends on external txt files.
- Level 1/2/3 definitions compile and load through the level catalog API.

### Phase 2: Core Gameplay Rules

- Port movement and gravity behavior.
- Port gold collection and teleport rules with fixed per-level teleport uses.
- Implement enemy collision killing player.
- Implement life loss and fixed spawn point respawn.
- Implement game over when lives reach zero.
- Add acceleration for left/right movement while key is held.

Exit criteria:

- Core mechanics work without renderer-specific hacks.

### Phase 3: Rendering And Input

- Implement input command translation.
- Implement static + dynamic renderer.
- Add HUD: lives, collected gold, elapsed time, steps.

Exit criteria:

- Playable level with responsive controls and readable HUD.

### Phase 4: Level Progression

- Add exactly 3 authored levels.
- Implement level completion condition.
- Reveal/spawn ladder when all gold is collected.
- Add progression to next level and final win state.
- verify that levels have empty cells in the very first row so that the the ladder can be used and the player can go up to the next level
Exit criteria:

- Player can complete all three levels in sequence.

### Phase 5: Persistence And Stats

- Save stats on level end.
- Load and display best stats.
- Record lives used, steps, and completion time.

Exit criteria:

- Stats persist between runs.

### Phase 6: Stabilization

- Add collision and edge-case checks.
- Tune movement acceleration and enemy speed.
- Improve UX text and failure feedback.

Exit criteria:

- Stable gameplay across all 3 levels.

## Testing Strategy

### Manual Test Checklist

- Movement in all directions behaves correctly.
- Gravity behaves correctly near ladders and pits.
- Enemies kill player and consume a life.
- Enemy movement in v1 is simple patrol only.
- Fixed spawn point respawn works until lives reach zero.
- Gold collection updates counters and score.
- Ladder appears only after all gold is collected.
- Fixed per-level teleport uses are enforced with no recharge.
- Teleport behavior is valid and never spawns inside walls.
- Level definitions remain minimal ASCII-only level definitions in C++ source.
- Runtime level loading works with no external txt file dependency.
- Level transition works from level 1 to level 3.
- Stats file updates after run completion.

### Suggested Lightweight Automated Tests

Prioritize pure model/core logic tests (without SFML window):

- Level parser validity.
- Collision and passability checks.
- Gold collection and win condition transitions.
- Enemy-player collision outcomes.
- Stats serialization/deserialization.

## Definition Of Done

- Three-level C++ SFML game is playable end-to-end.
- Required improvements from `raw/Load Runner.md` are implemented.
- Game logic and rendering are cleanly separated.
- Project remains simple to read and extend.
- Plan deviations are documented in this file.

## Resolved Decisions

- Enemy AI v1: simple patrol only.
- Respawn model: fixed spawn point respawn.
- Teleport design: fixed per-level teleport uses.
- Level format: minimal ASCII-only level definitions in C++ source files.