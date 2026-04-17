---
description: "Use when writing or modifying C++ tests for LoadRunner model/core gameplay logic verification, including movement, collisions, level parsing, and progression rules."
name: "LoadRunner C++ Testing Conventions"
applyTo:
  - "tests/**/*.cpp"
  - "tests/**/*.cc"
  - "tests/**/*.cxx"
  - "tests/**/*.h"
  - "tests/**/*.hpp"
---

# LoadRunner C++ Testing Conventions

## Primary Goal

Verify model/core gameplay logic behavior in a deterministic way without coupling tests to SFML rendering or input event plumbing.

## Mandatory

- Focus tests on model/core logic and state transitions.
- Keep tests independent from `sf::RenderWindow`, textures, and rendering loops.
- Use deterministic setups: fixed inputs, fixed random seeds, fixed time steps.
- Write tests that are small, fast, and isolated.
- Validate edge and boundary conditions for movement and collisions.
- When changing model/core behavior, add or update tests in the same change.

## Required Coverage Areas

- Movement and gravity rules.
- Collision and passability checks.
- Enemy collision outcomes and life loss.
- Fixed spawn point respawn behavior.
- Enemy AI v1 remains simple patrol only.
- Fixed per-level teleport uses and no recharge behavior.
- Level parser rules for minimal ASCII-only level files.
- Ladder reveal after all gold is collected.
- Win/lose transitions and level progression.
- Stats serialization/deserialization for model/core contracts.

## Test Design Rules

- Prefer one behavior assertion focus per test case.
- Name tests clearly using behavior intent (for example: Given_When_Then style).
- Use test fixtures/builders to avoid duplicated level setup code.
- Keep filesystem usage minimal; use temporary files only when parser/persistence behavior requires it.
- Do not depend on wall-clock timing; use controlled clocks or injected time sources.

## Preferred

- Prefer table-driven tests for tile/state transition matrices.
- Prefer explicit expected-state assertions over broad snapshot comparisons.
- Prefer verifying public behavior and contracts rather than private implementation details.

## Out Of Scope

- Pixel-perfect rendering verification.
- Visual animation timing verification.
- Input device integration testing through SFML event loop.
