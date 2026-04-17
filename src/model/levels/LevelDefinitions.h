#pragma once

#include <string>
#include <vector>

struct LevelDefinition {
    int id = 0;
    const char* name = "";
    std::vector<std::string> rows;
    int teleportUses = 0;
};

/// Returns the level definition for a 1-based level id.
/// Throws std::runtime_error when levelId is not found.
const LevelDefinition& getLevelDefinition(int levelId);

/// Number of built-in authored levels.
int levelDefinitionCount();
