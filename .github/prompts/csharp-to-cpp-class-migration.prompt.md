---
name: "LoadRunner C# to C++ Class Migration"
description: "Use when converting one C# LoadRunner class to C++ SFML in a single pass while preserving behavior and project constraints."
argument-hint: "Source class/file, target C++ module/path, and any behavior changes"
agent: "agent"
---
Migrate one LoadRunner class from C# to C++ in one pass.

Reference constraints before coding:
- [.github/instructions/loadrunner-cpp-sfml.instructions.md](../instructions/loadrunner-cpp-sfml.instructions.md)
- [CPP_SFML_PORT_PLAN.md](../../CPP_SFML_PORT_PLAN.md)
- [raw/Load Runner.md](../../raw/Load%20Runner.md)

Input you should use from the user argument:
- Source C# class and file path
- Target C++ module and destination path
- Any behavior deltas for this class only

Execution requirements:
1. Read the source C# class and identify direct dependencies and side effects.
2. Preserve gameplay behavior unless user explicitly requests a change.
3. Keep responsibilities aligned with project architecture:
- model/core logic stays independent of SFML rendering/window types
- rendering and input stay in renderer/input modules
4. Create or update C++ files in the target module with co-located header and source files.
5. Keep implementation simple and avoid overengineering.
6. Apply locked project decisions when relevant:
- simple patrol only
- fixed spawn point respawn
- fixed per-level teleport uses
- minimal ASCII-only level files
7. Validate changed files for obvious compile or logic issues and report any unresolved gaps.

Expected output format:
1. Converted files and what each file contains
2. Behavior parity notes (what is equivalent to C#)
3. Any intentional deviations requested by user
4. Follow-up tasks needed for full integration