#include "HudRenderer.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <array>
#include <filesystem>
#include <iomanip>
#include <sstream>

#include "core/GameSession.h"

namespace {

constexpr float kHudHeight = 60.0f;

} // namespace

HudRenderer::HudRenderer()
{
    const std::array<std::filesystem::path, 4> candidates = {
        std::filesystem::path("assets/fonts/DejaVuSans.ttf"),
        std::filesystem::path("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"),
        std::filesystem::path("/usr/share/fonts/TTF/DejaVuSans.ttf"),
        std::filesystem::path("/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf")
    };

    for (const auto& path : candidates) {
        if (std::filesystem::exists(path) && m_font.openFromFile(path)) {
            m_fontReady = true;
            break;
        }
    }
}

void HudRenderer::draw(sf::RenderTarget& target,
                       const GameSession& session,
                       float elapsedSeconds,
                       int levelId,
                       float mapTopY) const
{
    const auto targetSize = target.getSize();

    sf::RectangleShape hudBackground({static_cast<float>(targetSize.x), kHudHeight});
    hudBackground.setPosition({0.0f, 0.0f});
    hudBackground.setFillColor(sf::Color(14, 20, 26));
    target.draw(hudBackground);

    sf::RectangleShape hudDivider({static_cast<float>(targetSize.x), 2.0f});
    hudDivider.setPosition({0.0f, kHudHeight - 2.0f});
    hudDivider.setFillColor(sf::Color(55, 75, 95));
    target.draw(hudDivider);

    sf::RectangleShape mapDivider({static_cast<float>(targetSize.x), 1.0f});
    mapDivider.setPosition({0.0f, mapTopY - 8.0f});
    mapDivider.setFillColor(sf::Color(40, 55, 70));
    target.draw(mapDivider);

    if (!m_fontReady) {
        return;
    }

    std::ostringstream hudStream;
    hudStream << "Level " << levelId
              << "   Lives: " << session.player().lives
              << "   Gold: " << session.player().goldCollected << "/" << session.level().goldCount()
              << "   Steps: " << session.player().steps
              << "   Time: " << formatElapsed(elapsedSeconds)
              << "   Teleports: " << session.teleportUsesRemaining();

    sf::Text hudText(m_font, hudStream.str(), 18);
    hudText.setPosition({16.0f, 18.0f});
    hudText.setFillColor(sf::Color(230, 235, 245));
    target.draw(hudText);
}

void HudRenderer::drawGameOverOverlay(sf::RenderTarget& target, float windowWidth, float windowHeight) const
{
    sf::RectangleShape overlay({windowWidth, windowHeight});
    overlay.setPosition({0.0f, 0.0f});
    overlay.setFillColor(sf::Color(5, 5, 8, 130));
    target.draw(overlay);

    if (!m_fontReady) {
        return;
    }

    sf::Text gameOverText(m_font, "GAME OVER", 42);
    gameOverText.setFillColor(sf::Color(245, 90, 90));
    gameOverText.setPosition({windowWidth * 0.5f - 140.0f, windowHeight * 0.5f - 70.0f});
    target.draw(gameOverText);

    sf::Text hintText(m_font, "Press Escape to exit", 20);
    hintText.setFillColor(sf::Color(220, 225, 235));
    hintText.setPosition({windowWidth * 0.5f - 110.0f, windowHeight * 0.5f - 10.0f});
    target.draw(hintText);
}

void HudRenderer::drawLevelCompleteOverlay(sf::RenderTarget& target, float windowWidth, float windowHeight) const
{
    sf::RectangleShape overlay({windowWidth, windowHeight});
    overlay.setPosition({0.0f, 0.0f});
    overlay.setFillColor(sf::Color(8, 12, 18, 110));
    target.draw(overlay);

    if (!m_fontReady) {
        return;
    }

    sf::Text completeText(m_font, "LEVEL COMPLETE", 42);
    completeText.setFillColor(sf::Color(115, 230, 140));
    completeText.setPosition({windowWidth * 0.5f - 165.0f, windowHeight * 0.5f - 70.0f});
    target.draw(completeText);

    sf::Text hintText(m_font, "Loading next level...", 20);
    hintText.setFillColor(sf::Color(225, 232, 242));
    hintText.setPosition({windowWidth * 0.5f - 95.0f, windowHeight * 0.5f - 10.0f});
    target.draw(hintText);
}

void HudRenderer::drawGameWonOverlay(sf::RenderTarget& target, float windowWidth, float windowHeight) const
{
    sf::RectangleShape overlay({windowWidth, windowHeight});
    overlay.setPosition({0.0f, 0.0f});
    overlay.setFillColor(sf::Color(8, 18, 12, 120));
    target.draw(overlay);

    if (!m_fontReady) {
        return;
    }

    sf::Text winText(m_font, "YOU WON ALL LEVELS", 42);
    winText.setFillColor(sf::Color(126, 245, 155));
    winText.setPosition({windowWidth * 0.5f - 220.0f, windowHeight * 0.5f - 70.0f});
    target.draw(winText);

    sf::Text hintText(m_font, "Press Escape to exit", 20);
    hintText.setFillColor(sf::Color(225, 232, 242));
    hintText.setPosition({windowWidth * 0.5f - 110.0f, windowHeight * 0.5f - 10.0f});
    target.draw(hintText);
}

std::string HudRenderer::formatElapsed(float elapsedSeconds)
{
    if (elapsedSeconds < 0.0f) {
        elapsedSeconds = 0.0f;
    }

    const int totalSeconds = static_cast<int>(elapsedSeconds);
    const int minutes = totalSeconds / 60;
    const int seconds = totalSeconds % 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes
        << ':'
        << std::setfill('0') << std::setw(2) << seconds;
    return oss.str();
}
