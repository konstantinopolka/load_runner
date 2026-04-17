#pragma once

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

#include <cstdint>

#include "HudRenderer.h"
#include "persistence/LevelStats.h"

class GameSession;
class Level;

/// Renders level geometry and runtime entities with static/dynamic layers.
class Renderer {
public:
    void draw(sf::RenderWindow& window,
              const GameSession& session,
              float elapsedSeconds,
              int levelId,
              bool gameWon,
              const LevelStats& bestStats);

private:
    void updateLayout(const Level& level, sf::Vector2u windowSize);
    void ensureStaticLayer(sf::Vector2u windowSize);
    void rebuildStaticLayer(const Level& level);

    void drawDynamicLayer(sf::RenderTarget& target, const GameSession& session) const;

    [[nodiscard]] std::uint64_t computeStaticSignature(const Level& level) const;
    [[nodiscard]] sf::Vector2f cellToPixel(int row, int col) const;

private:
    HudRenderer m_hud;

    sf::RenderTexture m_staticLayer;
    sf::Vector2u m_staticLayerSize{};

    bool m_staticDirty = true;
    std::uint64_t m_lastStaticSignature = 0;

    float m_tileSize = 32.0f;
    sf::Vector2f m_levelOrigin{16.0f, 72.0f};
};
