#pragma once

#include <cstdint>

/// Per-level aggregate stats — pure data, no SFML types.
/// bestTimeMs == 0 and bestSteps == 0 indicate no completed run has been recorded yet.
struct LevelStats {
    int      levelId        = 0;
    int64_t  bestTimeMs     = 0;  ///< milliseconds; 0 = not yet set
    int      bestSteps      = 0;  ///< 0 = not yet set
    int      totalRuns      = 0;
    int      totalLivesUsed = 0;
};
