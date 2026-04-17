# Phase 0, Phase 1, and Phase 1.5 Summary

## Phase 0: Project Bootstrap
- **Goal:** Create the basic CMake project, manage the SFML dependency, and verify that an empty SFML window compiles, runs, and closes cleanly.
- **Achievements:** 
  - `CMakeLists.txt` created with C++17 support.
  - Used `FetchContent` to download and link SFML 3.0 automatically without requiring local package installs.
  - Minimal window event loop established in `src/main.cpp`.
  - Build verified successfully.

## Phase 1: Domain Model Port
- **Goal:** Port the game logic data structures (Model layer) from C# to C++ keeping it completely independent of SFML rendering.
- **Achievements:**
  - Implemented `CellType` enum and helper methods (`src/model/Cell.h`).
  - Implemented `Level` class (`src/model/Level.h` / `.cpp`) with model-side grid validation, spawn extraction, and gold counting.
  - Implemented `PlayerState` data type mapping position, velocity, and lives (`src/model/PlayerState.h` / `.cpp`).
  - Implemented `EnemyState` for basic patrol mechanics (`src/model/EnemyState.h` / `.cpp`).

## Phase 1.5: Level Storage Migration (Completed)
- **Goal:** Remove runtime dependency on external ASCII txt files and load authored levels from compiled C++ definitions.
- **Achievements:**
  - Added level catalog and definitions module (`src/model/levels/LevelDefinitions.h` / `.cpp`) with three authored levels.
  - Refactored level loading to in-memory APIs (`Level::loadFromDefinition` and `Level::loadFromRows`) in `src/model/Level.h` / `.cpp`.
  - Added per-level teleport-use configuration through level definitions.
  - Updated startup loading path in `src/main.cpp` to use definition-based loading (`levelId = 1`) instead of file-path loading.
  - Archived legacy file-based level data from `assets/levels/level01.txt` to `assets/levels/archive/level01_legacy.txt`.
