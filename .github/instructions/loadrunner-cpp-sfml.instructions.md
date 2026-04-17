---
description: "Use when implementing or modifying the C++ SFML LoadRunner port, including game logic, level loading, rendering, input, and persistence."
name: "LoadRunner C++ SFML Port Conventions"
applyTo: "**/*.{h,hpp,cpp,cc,cxx}"
---

# LoadRunner C++ SFML Port Conventions

## Primary Goal

Build and maintain a simple, readable C++ SFML version of LoadRunner based on:

- `CPP_SFML_PORT_PLAN.md`
- `raw/Load Runner.md`
- `raw/cpp-sfml-project-summary.md`

## Mandatory Rules

- Keep architecture split into model/core/input/renderer/states.
- Keep gameplay model logic free from SFML rendering/window types.
- Use SFML only in renderer/input/platform-facing code.
- Keep `.h` and `.cpp` files co-located by module (feature folders), not split into mirrored `include/` and `src/` trees.
- Prefer simple implementations over generic abstractions.
- Do not add framework-heavy patterns (ECS, service locators, plugin systems) unless explicitly requested.
- Preserve game behavior consistency when porting from C# before adding enhancements.

## Gameplay Requirements To Always Preserve

- Enemy AI v1 is simple patrol only.
- Use fixed spawn point respawn.
- Use fixed per-level teleport uses.
- Use minimal ASCII-only level definitions in C++ source files.
- Player has lives and can be killed by enemies.
- Horizontal movement supports acceleration while key is held.
- Exactly 3 authored levels are supported.
- Levels are provided by dedicated C++ level definition modules.
- A ladder is revealed/spawned after all gold is collected.
- Stats include steps, completion time, and lives used.

## Technical Requirements

- Keep timer handling parallel to gameplay updates with a simple, safe model (atomics preferred).
- Avoid full-map redraw every frame by separating static and dynamic rendering layers.
- Keep `main.cpp` minimal (create game object and run loop).
- Avoid runtime txt level parsing for shipped levels; load levels from compiled C++ definitions.
- Use the locally downloaded SFML source tree at `raw/SFML` as the primary API/examples reference when implementing SFML-related code.
- Prefer checking local SFML headers/examples/docs before external web lookup when uncertain about SFML behavior.

## Coding Preferences

- Prefer RAII and value types where possible.
- Keep functions short and purpose-driven.
- Add short intent-focused comments only when logic is non-obvious.
- Avoid premature optimization except where required by project constraints.

## Porting Workflow

- Step 1: Reach feature parity with the C# baseline.
- Step 2: Implement explicit requirement improvements.
- Step 3: Refine performance and readability without overengineering.

When uncertain, choose the simplest design that satisfies `CPP_SFML_PORT_PLAN.md`.