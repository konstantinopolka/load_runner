#include "LevelDefinitions.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace {

const std::vector<LevelDefinition> kLevelDefinitions = {
    {
        1,
        "level01",
        {
            "#..............L...#",
            "#..............L...#",
            "#....H.........H...#",
            "#.G..H..G......H...#",
            "#####H#########H####",
            "#....H....#....H...#",
            "#..G.H..G.#....H.P.#",
            "#####H###.#..##H####",
            "#....H.E..#....H.T.#",
            "####################"
        },
        3
    },
    {
        2,
        "level02",
        {
            "#............L.....#",
            "#......G.....H.....#",
            "#.####H######H.###.#",
            "#.#...H....#.H...#.#",
            "#.#...H.E..#.H...#.#",
            "#.##########.H...#.#",
            "#.PT..H......H..G..#",
            "#.####H###########.#",
            "#.....H..G.........#",
            "####################"
        },
        2
    },
    {
        3,
        "level03",
        {
            "#...........L......#",
            "#....G......H..G...#",
            "######H#####H#####.#",
            "#..E..H...#.H....#.#",
            "######H...#.H....#.#",
            "#.....#####.H....#.#",
            "#..T........H....#.#",
            "#.#####H#########..#",
            "#..P...H...G.......#",
            "####################"
        },
        1
    }
};

} // namespace

const LevelDefinition& getLevelDefinition(int levelId)
{
    for (const auto& definition : kLevelDefinitions) {
        if (definition.id == levelId) {
            return definition;
        }
    }

    throw std::runtime_error(
        "Unknown level id " + std::to_string(levelId) +
        ", available range is 1.." + std::to_string(levelDefinitionCount()));
}

int levelDefinitionCount()
{
    return static_cast<int>(kLevelDefinitions.size());
}
