#include "PlayerState.h"

void PlayerState::reset(Position spawn, int startLives)
{
    pos           = spawn;
    velocityX     = 0.0f;
    lives         = startLives;
    steps         = 0;
    goldCollected = 0;
}
