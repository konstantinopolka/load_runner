#pragma once

#include <string>
#include <vector>

#include "LevelStats.h"

/// Lightweight flat-file persistence for per-level stats.
/// All functions gracefully handle missing or malformed files.
namespace StatsStorage {

/// Load all per-level stats from the given file path.
/// Returns an empty vector if the file does not exist or cannot be parsed.
std::vector<LevelStats> loadAll(const std::string& filePath);

/// Return the best stats recorded for a specific level.
/// Returns a zero-initialised LevelStats with the given levelId if no record exists.
LevelStats loadBest(const std::string& filePath, int levelId);

/// Record a completed run for a level.
/// Updates bestTimeMs and bestSteps only when the new values are strictly better.
/// Increments totalRuns and accumulates totalLivesUsed unconditionally.
/// Creates the stats file if it does not yet exist.
void recordRun(const std::string& filePath,
               int               levelId,
               int64_t           timeMs,
               int               steps,
               int               livesUsed);

} // namespace StatsStorage
