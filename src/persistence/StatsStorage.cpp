#include "StatsStorage.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

// File format (plain text, one record per line):
//   # loadrunner stats v1
//   <levelId> <bestTimeMs> <bestSteps> <totalRuns> <totalLivesUsed>
// Lines starting with '#' are comments and are ignored.
// Any line that cannot be parsed is silently skipped (malformed-file fallback).

namespace StatsStorage {

namespace {

/// Update a best value: replace current with candidate only when strictly better.
/// A zero current value means "not yet set", so any positive candidate wins.
template <typename T>
void updateBest(T& current, T candidate)
{
    if (current == 0 || candidate < current) {
        current = candidate;
    }
}

bool tryParseLine(const std::string& line, LevelStats& out)
{
    if (line.empty() || line[0] == '#') {
        return false;
    }

    std::istringstream ss(line);
    LevelStats tmp;
    if (!(ss >> tmp.levelId >> tmp.bestTimeMs >> tmp.bestSteps
             >> tmp.totalRuns >> tmp.totalLivesUsed)) {
        return false;
    }

    out = tmp;
    return true;
}

void writeAll(const std::string& filePath, const std::vector<LevelStats>& records)
{
    std::ofstream out(filePath, std::ios::trunc);
    if (!out.is_open()) {
        return; // best-effort write — silent failure avoids crashing the game
    }

    out << "# loadrunner stats v1\n";
    for (const LevelStats& r : records) {
        out << r.levelId
            << ' ' << r.bestTimeMs
            << ' ' << r.bestSteps
            << ' ' << r.totalRuns
            << ' ' << r.totalLivesUsed
            << '\n';
    }
}

} // namespace

std::vector<LevelStats> loadAll(const std::string& filePath)
{
    std::ifstream in(filePath);
    if (!in.is_open()) {
        return {};
    }

    std::vector<LevelStats> records;
    std::string line;
    while (std::getline(in, line)) {
        LevelStats record;
        if (tryParseLine(line, record)) {
            records.push_back(record);
        }
    }

    return records;
}

LevelStats loadBest(const std::string& filePath, int levelId)
{
    const std::vector<LevelStats> all = loadAll(filePath);
    for (const LevelStats& r : all) {
        if (r.levelId == levelId) {
            return r;
        }
    }

    LevelStats empty;
    empty.levelId = levelId;
    return empty;
}

void recordRun(const std::string& filePath,
               int               levelId,
               int64_t           timeMs,
               int               steps,
               int               livesUsed)
{
    std::vector<LevelStats> records = loadAll(filePath);

    auto it = std::find_if(records.begin(), records.end(),
                           [levelId](const LevelStats& r) { return r.levelId == levelId; });

    if (it == records.end()) {
        // First completed run for this level
        LevelStats fresh;
        fresh.levelId        = levelId;
        fresh.bestTimeMs     = timeMs;
        fresh.bestSteps      = steps;
        fresh.totalRuns      = 1;
        fresh.totalLivesUsed = livesUsed;
        records.push_back(fresh);
    } else {
        updateBest(it->bestTimeMs, timeMs);
        updateBest(it->bestSteps, steps);
        it->totalRuns      += 1;
        it->totalLivesUsed += livesUsed;
    }

    writeAll(filePath, records);
}

} // namespace StatsStorage
