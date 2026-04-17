#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <string>

#include "persistence/LevelStats.h"

class GameSession;

/// Draws HUD and simple status overlays.
class HudRenderer {
public:
    HudRenderer();

    void draw(sf::RenderTarget& target,
              const GameSession& session,
              float elapsedSeconds,
              int levelId,
              float mapTopY,
              const LevelStats& bestStats) const;

    void drawLevelCompleteOverlay(sf::RenderTarget& target, float windowWidth, float windowHeight) const;
    void drawGameWonOverlay(sf::RenderTarget& target, float windowWidth, float windowHeight) const;
    void drawGameOverOverlay(sf::RenderTarget& target, float windowWidth, float windowHeight) const;

private:
    static std::string formatElapsed(float elapsedSeconds);
    static std::string formatMs(int64_t ms);

    bool m_fontReady = false;
    sf::Font m_font;
};
