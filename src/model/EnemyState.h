#pragma once

#include "Cell.h"
#include "Level.h"

/// Enemy gameplay state — simple patrol behavior, pure data.
struct EnemyState {
    Position pos{};
    int      patrolDirection = 1;  // +1 = right, -1 = left

    explicit EnemyState(Position spawn)
        : pos(spawn), patrolDirection(1) {}
};
