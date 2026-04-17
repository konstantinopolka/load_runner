# Level Parsing Issue (Resolved)

## Status
Resolved in Phase 1.5 (Level Storage Migration).

The runtime no longer depends on `assets/levels/level01.txt` and no longer loads authored levels from external txt files.

## Historical Error (Before Fix)
When loading `assets/levels/level01.txt`, the app failed with:

```text
Level load error: Level row 3 has 19 columns, expected 20 (file: assets/levels/level01.txt)
```

## Root Cause (Historical)
1. Cross-platform text line ending issues (`\r\n` in Windows-edited files on Linux/WSL runtime).
2. Fragile manual edits in external ASCII files causing row-width mismatches.

## Implemented Resolution
Authored levels were migrated to compiled C++ definitions, and loading now uses in-memory rows.

- Level catalog and embedded maps: [src/model/levels/LevelDefinitions.h](../src/model/levels/LevelDefinitions.h), [src/model/levels/LevelDefinitions.cpp](../src/model/levels/LevelDefinitions.cpp)
- In-memory loading APIs: [src/model/Level.h](../src/model/Level.h), [src/model/Level.cpp](../src/model/Level.cpp)
- Runtime entry point now loads by level id from definitions: [src/main.cpp](../src/main.cpp)

## New LevelDefinitions Flow
1. `main()` requests a level by id via `Level::loadFromDefinition(levelId)`.
2. `Level::loadFromDefinition` pulls the definition from `getLevelDefinition(levelId)`.
3. `Level::loadFromRows` validates row widths and tile characters in memory.
4. Parsed model data is produced (spawn points, enemy spawns, gold count, teleport uses).

## Legacy Data Handling
The old text level was preserved only for reference:

- Archived file: [assets/levels/archive/level01_legacy.txt](../assets/levels/archive/level01_legacy.txt)

## Outcome
The original parsing failure path is eliminated for shipped levels because runtime loading no longer reads external txt maps.
