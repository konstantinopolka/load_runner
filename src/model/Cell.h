#pragma once

/// Tile types for the LoadRunner grid.
/// Maps 1:1 with the ASCII level file characters defined in CPP_SFML_PORT_PLAN.md.
enum class CellType : char {
    Empty        = '.',
    Wall         = '#',
    Ladder       = 'H',
    Gold         = 'G',
    PlayerSpawn  = 'P',
    EnemySpawn   = 'E',
    Teleport     = 'T',
    HiddenLadder = 'L'  // becomes Ladder after all gold is collected
};

/// Returns true if an entity can walk/fall through this cell type.
inline bool isPassable(CellType type)
{
    switch (type) {
        case CellType::Wall:
            return false;
        default:
            return true;
    }
}

/// Convert an ASCII character from a level file to a CellType.
/// Returns true on success. On failure, returns false and leaves 'out' unchanged.
inline bool charToCellType(char ch, CellType& out)
{
    switch (ch) {
        case '.': out = CellType::Empty;        return true;
        case '#': out = CellType::Wall;         return true;
        case 'H': out = CellType::Ladder;       return true;
        case 'G': out = CellType::Gold;         return true;
        case 'P': out = CellType::PlayerSpawn;  return true;
        case 'E': out = CellType::EnemySpawn;   return true;
        case 'T': out = CellType::Teleport;     return true;
        case 'L': out = CellType::HiddenLadder; return true;
        default:  return false;
    }
}

/// Convert a CellType back to its ASCII character.
inline char cellTypeToChar(CellType type)
{
    return static_cast<char>(type);
}
