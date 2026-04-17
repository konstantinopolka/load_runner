#pragma once

#include "Cell.h"
#include "Level.h"

/// Player gameplay state — pure data, no SFML types.
struct PlayerState {
    Position pos{};
    float    velocityX     = 0.0f;   // for horizontal acceleration
    int      lives         = 3;
    int      steps         = 0;
    int      goldCollected = 0;

    /// Reset to initial state at given spawn point.
    void reset(Position spawn, int startLives);
};
