#pragma once

#include <random>
#include <vector>

#include "GameStatus.h"
#include "InputCommand.h"
#include "model/EnemyState.h"
#include "model/Level.h"
#include "model/PlayerState.h"

/// Runtime gameplay orchestrator for core rules.
/// Keeps all phase-2 logic independent from renderer and SFML APIs.
class GameSession {
public:
    explicit GameSession(Level level, int startLives = 3, unsigned int randomSeed = 1337u);

    /// Queue a one-shot command to be processed in the next update tick.
    void queueCommand(InputCommand command);

    /// Set held horizontal input axis: -1 (left), 0 (none), +1 (right).
    void setHorizontalInput(int axis);

    /// Advance the simulation by a fixed timestep in seconds.
    void update(float dtSeconds);

    [[nodiscard]] GameStatus status() const { return m_status; }
    [[nodiscard]] bool isLevelComplete() const { return m_status == GameStatus::LevelComplete; }
    [[nodiscard]] bool isGameOver() const { return m_status == GameStatus::GameOver; }

    [[nodiscard]] const Level& level() const { return m_level; }
    [[nodiscard]] const PlayerState& player() const { return m_player; }
    [[nodiscard]] const std::vector<EnemyState>& enemies() const { return m_enemies; }

    [[nodiscard]] int remainingGold() const { return m_remainingGold; }
    [[nodiscard]] int teleportUsesRemaining() const { return m_teleportUsesRemaining; }

private:
    bool tryMovePlayerBy(int dRow, int dCol, bool countStep);
    void processCommand(InputCommand command);
    void applyHorizontalMovement(float dtSeconds);
    void applyGravity();
    void updateEnemies(float dtSeconds);

    void applyCellEffectsAtPlayerPosition();
    void checkLevelCompletion();
    void revealHiddenLadders();

    [[nodiscard]] bool isEnemyAt(const Position& pos) const;
    [[nodiscard]] bool tryFindTeleportDestination(Position& outPos);

    void resolveEnemyCollision();
    void handlePlayerHitByEnemy();

private:
    static constexpr float kHorizontalAcceleration = 10.0f;
    static constexpr float kHorizontalDeceleration = 14.0f;
    static constexpr float kMaxHorizontalSpeed = 5.0f; // tiles/sec
    static constexpr float kEnemySpeed = 1.0f;         // tiles/sec

    Level m_level;
    PlayerState m_player;
    std::vector<EnemyState> m_enemies;

    Position m_spawn{};
    int m_remainingGold = 0;
    int m_teleportUsesRemaining = 0;

    int m_horizontalInputAxis = 0;
    float m_horizontalAccumulator = 0.0f;
    float m_enemyAccumulator = 0.0f;

    GameStatus m_status = GameStatus::Running;
    std::vector<InputCommand> m_pendingCommands;

    std::mt19937 m_rng;
};
