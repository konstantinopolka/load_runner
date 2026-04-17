#include "Level.h"

#include "levels/LevelDefinitions.h"

#include <stdexcept>
#include <string>
#include <vector>

Level Level::loadFromDefinition(int levelId)
{
    const LevelDefinition& definition = getLevelDefinition(levelId);

    Level level = Level::loadFromRows(definition.rows, definition.name);
    level.m_teleportUses = definition.teleportUses;

    return level;
}

Level Level::loadFromRows(const std::vector<std::string>& rows, const std::string& sourceName)
{
    const std::string sourceLabel = sourceName.empty() ? "<in-memory>" : sourceName;

    Level level;

    if (rows.empty()) {
        throw std::runtime_error("Level data is empty: " + sourceLabel);
    }

    level.m_width  = static_cast<int>(rows[0].size());
    level.m_height = static_cast<int>(rows.size());
    level.m_cells.resize(level.m_width * level.m_height);

    bool foundPlayer = false;

    for (int r = 0; r < level.m_height; ++r) {
        if (static_cast<int>(rows[r].size()) != level.m_width) {
            throw std::runtime_error(
                "Level row " + std::to_string(r + 1) + " has " +
                std::to_string(rows[r].size()) + " columns, expected " +
                std::to_string(level.m_width) + " (source: " + sourceLabel + ")");
        }

        for (int c = 0; c < level.m_width; ++c) {
            CellType cell;
            if (!charToCellType(rows[r][c], cell)) {
                throw std::runtime_error(
                    "Unknown tile character '" + std::string(1, rows[r][c]) +
                    "' at row " + std::to_string(r + 1) +
                    " col " + std::to_string(c + 1) +
                    " (source: " + sourceLabel + ")");
            }

            level.m_cells[r * level.m_width + c] = cell;

            if (cell == CellType::PlayerSpawn) {
                if (foundPlayer) {
                    throw std::runtime_error(
                        "Multiple player spawns found at row " +
                        std::to_string(r + 1) + " col " +
                        std::to_string(c + 1) + " (source: " + sourceLabel + ")");
                }
                level.m_playerSpawn = {r, c};
                foundPlayer = true;
            }

            if (cell == CellType::EnemySpawn) {
                level.m_enemySpawns.push_back({r, c});
            }

            if (cell == CellType::Gold) {
                level.m_goldCount++;
            }
        }
    }

    if (!foundPlayer) {
        throw std::runtime_error("No player spawn (P) found in level data: " + sourceLabel);
    }

    if (level.m_goldCount == 0) {
        throw std::runtime_error("No gold (G) found in level data: " + sourceLabel);
    }

    if (!level.hasTopRowEmptyCell()) {
        throw std::runtime_error(
            "Top row must contain at least one empty cell (.) to allow ladder exit usage: " +
            sourceLabel);
    }

    return level;
}

bool Level::hasTopRowEmptyCell() const
{
    if (m_height <= 0 || m_width <= 0) {
        return false;
    }

    for (int col = 0; col < m_width; ++col) {
        if (cellAt(0, col) == CellType::Empty) {
            return true;
        }
    }

    return false;
}

CellType Level::cellAt(int row, int col) const
{
    if (row < 0 || row >= m_height || col < 0 || col >= m_width) {
        return CellType::Wall; // out-of-bounds treated as wall (matches C# behavior)
    }
    return m_cells[row * m_width + col];
}

void Level::setCellAt(int row, int col, CellType type)
{
    if (row < 0 || row >= m_height || col < 0 || col >= m_width) {
        return; // silently ignore out-of-bounds writes
    }
    m_cells[row * m_width + col] = type;
}
