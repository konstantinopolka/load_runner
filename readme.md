# LoadRunner (C++ / SFML)

A C++17 SFML port of LoadRunner focused on clear module boundaries between gameplay logic (`model`/`core`) and platform code (`input`/`renderer`).

## Current status

- Built with CMake and SFML 3.0 (via `FetchContent`)
- 3 authored built-in ASCII levels (`src/model/levels/LevelDefinitions.cpp`)
- Core gameplay loop with:
  - gravity and movement
  - gold collection
  - teleport with fixed per-level uses
  - enemy patrol + collision/life loss/respawn
  - ladder reveal after collecting all gold
  - level progression (1 -> 2 -> 3) and campaign win/game-over states
- HUD and overlay rendering

## Controls

- `A` / `D`: move left/right (held acceleration)
- `W`: move up
- `S`: move down
- `Space`: teleport (limited uses per level)
- `Esc`: quit

## Build

### Prerequisites

- CMake 3.24+
- C++17 compiler
- On Linux: X11 development libraries (required by SFML windowing)

### Commands

```bash
cmake -S . -B build
cmake --build build
```

Run:

```bash
`./build/LoadRunner`
```

## Test

The repository currently has no automated test targets configured in CMake (`ctest` reports no tests).

## Project layout

```text
src/
  core/      # gameplay orchestration and state transitions
  model/     # pure gameplay data and rules
  input/     # keyboard -> gameplay command translation
  renderer/  # SFML drawing and HUD
```
