#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <string>

class GameSession;

/// Draws HUD and simple status overlays.
class HudRenderer {
public:
    HudRenderer();

    void draw(sf::RenderTarget& target,
              const GameSession& session,
              float elapsedSeconds,
              int levelId,
              float mapTopY) const;

    void drawLevelCompleteOverlay(sf::RenderTarget& target, float windowWidth, float windowHeight) const;
    void drawGameWonOverlay(sf::RenderTarget& target, float windowWidth, float windowHeight) const;
    void drawGameOverOverlay(sf::RenderTarget& target, float windowWidth, float windowHeight) const;

private:
    static std::string formatElapsed(float elapsedSeconds);

    bool m_fontReady = false;
    sf::Font m_font;
};
