#include "GameSession.h"

#include <algorithm>
#include <cmath>

namespace {

int clampAxis(int axis)
{
    if (axis < 0) {
        return -1;
    }
    if (axis > 0) {
        return 1;
    }
    return 0;
}

} // namespace

GameSession::GameSession(Level level, int startLives, unsigned int randomSeed)
    : m_level(std::move(level)), m_spawn(m_level.playerSpawn()), m_remainingGold(m_level.goldCount()),
      m_teleportUsesRemaining(m_level.teleportUses()), m_rng(randomSeed)
{
    m_player.reset(m_spawn, startLives);

    const auto& spawns = m_level.enemySpawns();
    m_enemies.reserve(spawns.size());
    for (const auto& spawn : spawns) {
        m_enemies.emplace_back(spawn);
    }

    applyCellEffectsAtPlayerPosition();
}

void GameSession::queueCommand(InputCommand command)
{
    m_pendingCommands.push_back(command);
}

void GameSession::setHorizontalInput(int axis)
{
    m_horizontalInputAxis = clampAxis(axis);
}

void GameSession::update(float dtSeconds)
{
    if (m_status != GameStatus::Running) {
        m_pendingCommands.clear();
        return;
    }

    if (dtSeconds <= 0.0f) {
        m_pendingCommands.clear();
        return;
    }

    for (const auto command : m_pendingCommands) {
        processCommand(command);
        if (m_status != GameStatus::Running) {
            m_pendingCommands.clear();
            return;
        }
    }
    m_pendingCommands.clear();

    applyHorizontalMovement(dtSeconds);
    applyGravity();

    if (m_status != GameStatus::Running) {
        return;
    }

    updateEnemies(dtSeconds);
    resolveEnemyCollision();
}

void GameSession::processCommand(InputCommand command)
{
    switch (command) {
        case InputCommand::MoveUp:
            tryMovePlayerBy(-1, 0, true);
            break;
        case InputCommand::MoveDown:
            tryMovePlayerBy(1, 0, true);
            break;
        case InputCommand::Teleport:
        {
            if (m_teleportUsesRemaining <= 0) {
                return;
            }

            Position destination{};
            if (!tryFindTeleportDestination(destination)) {
                return;
            }

            m_player.pos = destination;
            --m_teleportUsesRemaining;
            applyCellEffectsAtPlayerPosition();
            resolveEnemyCollision();
            break;
        }
    }
}

bool GameSession::tryMovePlayerBy(int dRow, int dCol, bool countStep)
{
    if (dRow == 0 && dCol == 0) {
        return false;
    }

    Position next = m_player.pos;
    next.row += dRow;
    next.col += dCol;

    if (!isPassable(m_level.cellAt(next.row, next.col))) {
        return false;
    }

    m_player.pos = next;
    if (countStep) {
        ++m_player.steps;
    }

    applyCellEffectsAtPlayerPosition();
    resolveEnemyCollision();

    return true;
}

void GameSession::applyHorizontalMovement(float dtSeconds)
{
    if (m_horizontalInputAxis != 0) {
        m_player.velocityX += static_cast<float>(m_horizontalInputAxis) * kHorizontalAcceleration * dtSeconds;
        m_player.velocityX = std::clamp(m_player.velocityX, -kMaxHorizontalSpeed, kMaxHorizontalSpeed);
    } else {
        const float decel = kHorizontalDeceleration * dtSeconds;
        if (m_player.velocityX > 0.0f) {
            m_player.velocityX = std::max(0.0f, m_player.velocityX - decel);
        } else if (m_player.velocityX < 0.0f) {
            m_player.velocityX = std::min(0.0f, m_player.velocityX + decel);
        }
    }

    m_horizontalAccumulator += m_player.velocityX * dtSeconds;

    while (m_horizontalAccumulator >= 1.0f) {
        if (!tryMovePlayerBy(0, 1, true)) {
            m_player.velocityX = 0.0f;
            m_horizontalAccumulator = 0.0f;
            return;
        }
        m_horizontalAccumulator -= 1.0f;

        if (m_status != GameStatus::Running) {
            return;
        }
    }

    while (m_horizontalAccumulator <= -1.0f) {
        if (!tryMovePlayerBy(0, -1, true)) {
            m_player.velocityX = 0.0f;
            m_horizontalAccumulator = 0.0f;
            return;
        }
        m_horizontalAccumulator += 1.0f;

        if (m_status != GameStatus::Running) {
            return;
        }
    }
}

void GameSession::applyGravity()
{
    while (m_status == GameStatus::Running) {
        const CellType current = m_level.cellAt(m_player.pos.row, m_player.pos.col);
        const CellType below = m_level.cellAt(m_player.pos.row + 1, m_player.pos.col);

        if (current == CellType::Ladder) {
            break;
        }

        if (!isPassable(below) || below == CellType::Ladder) {
            break;
        }

        m_player.pos.row += 1;
        applyCellEffectsAtPlayerPosition();
        resolveEnemyCollision();
    }
}

void GameSession::updateEnemies(float dtSeconds)
{
    m_enemyAccumulator += kEnemySpeed * dtSeconds;

    while (m_enemyAccumulator >= 1.0f) {
        for (auto& enemy : m_enemies) {
            Position target = enemy.pos;
            target.col += enemy.patrolDirection;

            if (!isPassable(m_level.cellAt(target.row, target.col))) {
                enemy.patrolDirection *= -1;
                target = enemy.pos;
                target.col += enemy.patrolDirection;
            }

            if (isPassable(m_level.cellAt(target.row, target.col))) {
                enemy.pos = target;
            }
        }

        m_enemyAccumulator -= 1.0f;
    }
}

void GameSession::applyCellEffectsAtPlayerPosition()
{
    const CellType current = m_level.cellAt(m_player.pos.row, m_player.pos.col);

    if (current == CellType::Gold) {
        ++m_player.goldCollected;
        m_remainingGold = std::max(0, m_remainingGold - 1);
        m_level.setCellAt(m_player.pos.row, m_player.pos.col, CellType::Empty);

        if (m_remainingGold == 0) {
            revealHiddenLadders();
        }
    }

    checkLevelCompletion();
}

void GameSession::checkLevelCompletion()
{
    if (m_status != GameStatus::Running) {
        return;
    }

    if (m_remainingGold != 0) {
        return;
    }

    if (m_player.pos.row != 0) {
        return;
    }

    const CellType below = m_level.cellAt(m_player.pos.row + 1, m_player.pos.col);
    if (below == CellType::Ladder) {
        m_status = GameStatus::LevelComplete;
    }
}

void GameSession::revealHiddenLadders()
{
    for (int row = 0; row < m_level.height(); ++row) {
        for (int col = 0; col < m_level.width(); ++col) {
            if (m_level.cellAt(row, col) == CellType::HiddenLadder) {
                m_level.setCellAt(row, col, CellType::Ladder);
            }
        }
    }
}

bool GameSession::isEnemyAt(const Position& pos) const
{
    for (const auto& enemy : m_enemies) {
        if (enemy.pos.row == pos.row && enemy.pos.col == pos.col) {
            return true;
        }
    }

    return false;
}

bool GameSession::tryFindTeleportDestination(Position& outPos)
{
    std::vector<Position> candidates;
    candidates.reserve(static_cast<size_t>(m_level.width() * m_level.height()));

    for (int row = 0; row < m_level.height(); ++row) {
        for (int col = 0; col < m_level.width(); ++col) {
            Position pos{row, col};
            if (!isPassable(m_level.cellAt(row, col))) {
                continue;
            }
            if (isEnemyAt(pos)) {
                continue;
            }
            candidates.push_back(pos);
        }
    }

    if (candidates.empty()) {
        return false;
    }

    std::uniform_int_distribution<size_t> distribution(0, candidates.size() - 1);
    outPos = candidates[distribution(m_rng)];
    return true;
}

void GameSession::resolveEnemyCollision()
{
    if (m_status != GameStatus::Running) {
        return;
    }

    if (isEnemyAt(m_player.pos)) {
        handlePlayerHitByEnemy();
    }
}

void GameSession::handlePlayerHitByEnemy()
{
    --m_player.lives;

    if (m_player.lives <= 0) {
        m_status = GameStatus::GameOver;
        return;
    }

    m_player.pos = m_spawn;
    m_player.velocityX = 0.0f;
    m_horizontalAccumulator = 0.0f;
}
