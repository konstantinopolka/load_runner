#pragma once

#include <string>
#include <vector>

#include "Cell.h"

/// Simple 2D grid position.
struct Position {
    int row = 0;
    int col = 0;
};

/// Represents a loaded game level — pure data, no SFML types.
class Level {
public:
    /// Load a built-in authored level by 1-based id.
    static Level loadFromDefinition(int levelId);

    /// Load a level from in-memory ASCII rows. Throws std::runtime_error on failure.
    static Level loadFromRows(const std::vector<std::string>& rows, const std::string& sourceName);

    int width()  const { return m_width; }
    int height() const { return m_height; }

    CellType cellAt(int row, int col) const;
    void     setCellAt(int row, int col, CellType type);

    const Position&              playerSpawn()  const { return m_playerSpawn; }
    const std::vector<Position>& enemySpawns()  const { return m_enemySpawns; }
    int                          goldCount()    const { return m_goldCount; }
    int                          teleportUses() const { return m_teleportUses; }

    [[nodiscard]] bool hasTopRowEmptyCell() const;

private:
    int m_width  = 0;
    int m_height = 0;
    std::vector<CellType> m_cells;   // row-major: index = row * m_width + col

    Position              m_playerSpawn{};
    std::vector<Position> m_enemySpawns;
    int                   m_goldCount = 0;
    int                   m_teleportUses = 0;
};
