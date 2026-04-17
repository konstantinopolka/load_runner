#include "Renderer.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <algorithm>
#include <cmath>

#include "core/GameSession.h"
#include "model/Cell.h"
#include "model/EnemyState.h"
#include "model/Level.h"
#include "model/PlayerState.h"

namespace {

constexpr float kHudHeight = 60.0f;
constexpr float kSidePadding = 16.0f;
constexpr float kBottomPadding = 16.0f;

} // namespace

void Renderer::draw(sf::RenderWindow& window,
                    const GameSession& session,
                    float elapsedSeconds,
                    int levelId,
                    bool gameWon)
{
    updateLayout(session.level(), window.getSize());
    ensureStaticLayer(window.getSize());

    const std::uint64_t signature = computeStaticSignature(session.level());
    if (signature != m_lastStaticSignature) {
        m_lastStaticSignature = signature;
        m_staticDirty = true;
    }

    if (m_staticDirty) {
        rebuildStaticLayer(session.level());
        m_staticDirty = false;
    }

    const sf::Sprite staticSprite(m_staticLayer.getTexture());
    window.draw(staticSprite);

    drawDynamicLayer(window, session);
    m_hud.draw(window, session, elapsedSeconds, levelId, m_levelOrigin.y);

    if (gameWon) {
        m_hud.drawGameWonOverlay(window,
                                 static_cast<float>(window.getSize().x),
                                 static_cast<float>(window.getSize().y));
    } else if (session.isLevelComplete()) {
        m_hud.drawLevelCompleteOverlay(window,
                                       static_cast<float>(window.getSize().x),
                                       static_cast<float>(window.getSize().y));
    } else if (session.isGameOver()) {
        m_hud.drawGameOverOverlay(window,
                                 static_cast<float>(window.getSize().x),
                                 static_cast<float>(window.getSize().y));
    }
}

void Renderer::updateLayout(const Level& level, sf::Vector2u windowSize)
{
    const float availableWidth = std::max(1.0f, static_cast<float>(windowSize.x) - 2.0f * kSidePadding);
    const float availableHeight = std::max(1.0f, static_cast<float>(windowSize.y) - kHudHeight - kBottomPadding);

    const float tileByWidth = availableWidth / static_cast<float>(std::max(1, level.width()));
    const float tileByHeight = availableHeight / static_cast<float>(std::max(1, level.height()));

    m_tileSize = std::floor(std::max(8.0f, std::min(tileByWidth, tileByHeight)));

    const float mapPixelWidth = m_tileSize * static_cast<float>(level.width());
    const float mapPixelHeight = m_tileSize * static_cast<float>(level.height());

    m_levelOrigin.x = (static_cast<float>(windowSize.x) - mapPixelWidth) * 0.5f;
    m_levelOrigin.y = kHudHeight + (availableHeight - mapPixelHeight) * 0.5f;
}

void Renderer::ensureStaticLayer(sf::Vector2u windowSize)
{
    if (m_staticLayerSize == windowSize) {
        return;
    }

    if (!m_staticLayer.resize(windowSize)) {
        return;
    }

    m_staticLayerSize = windowSize;
    m_staticDirty = true;
}

void Renderer::rebuildStaticLayer(const Level& level)
{
    m_staticLayer.clear(sf::Color(26, 31, 38));

    sf::RectangleShape tile({m_tileSize - 1.0f, m_tileSize - 1.0f});

    for (int row = 0; row < level.height(); ++row) {
        for (int col = 0; col < level.width(); ++col) {
            const sf::Vector2f cellPos = cellToPixel(row, col);
            const CellType cell = level.cellAt(row, col);

            tile.setPosition(cellPos);
            tile.setFillColor(sf::Color(35, 44, 53));
            m_staticLayer.draw(tile);

            if (cell == CellType::Wall) {
                tile.setFillColor(sf::Color(84, 70, 61));
                m_staticLayer.draw(tile);
                continue;
            }

            if (cell == CellType::Ladder) {
                sf::RectangleShape rail({3.0f, m_tileSize - 2.0f});
                rail.setFillColor(sf::Color(214, 191, 126));

                rail.setPosition({cellPos.x + m_tileSize * 0.25f, cellPos.y + 1.0f});
                m_staticLayer.draw(rail);

                rail.setPosition({cellPos.x + m_tileSize * 0.75f - 3.0f, cellPos.y + 1.0f});
                m_staticLayer.draw(rail);

                sf::RectangleShape rung({m_tileSize * 0.5f, 2.0f});
                rung.setFillColor(sf::Color(214, 191, 126));
                for (int i = 1; i <= 3; ++i) {
                    rung.setPosition({cellPos.x + m_tileSize * 0.25f, cellPos.y + i * (m_tileSize / 4.0f)});
                    m_staticLayer.draw(rung);
                }
                continue;
            }

            if (cell == CellType::Teleport) {
                sf::RectangleShape portal({m_tileSize * 0.6f, m_tileSize * 0.6f});
                portal.setPosition({cellPos.x + m_tileSize * 0.2f, cellPos.y + m_tileSize * 0.2f});
                portal.setFillColor(sf::Color(90, 140, 220));
                m_staticLayer.draw(portal);
            }
        }
    }

    m_staticLayer.display();
}

void Renderer::drawDynamicLayer(sf::RenderTarget& target, const GameSession& session) const
{
    sf::RectangleShape actor({m_tileSize - 8.0f, m_tileSize - 8.0f});

    for (int row = 0; row < session.level().height(); ++row) {
        for (int col = 0; col < session.level().width(); ++col) {
            if (session.level().cellAt(row, col) != CellType::Gold) {
                continue;
            }

            sf::CircleShape gold(m_tileSize * 0.18f);
            gold.setFillColor(sf::Color(243, 202, 68));
            gold.setPosition({cellToPixel(row, col).x + m_tileSize * 0.32f,
                              cellToPixel(row, col).y + m_tileSize * 0.32f});
            target.draw(gold);
        }
    }

    actor.setFillColor(sf::Color(90, 200, 250));
    actor.setPosition({cellToPixel(session.player().pos.row, session.player().pos.col).x + 4.0f,
                       cellToPixel(session.player().pos.row, session.player().pos.col).y + 4.0f});
    target.draw(actor);

    actor.setFillColor(sf::Color(220, 90, 95));
    for (const EnemyState& enemy : session.enemies()) {
        actor.setPosition({cellToPixel(enemy.pos.row, enemy.pos.col).x + 4.0f,
                           cellToPixel(enemy.pos.row, enemy.pos.col).y + 4.0f});
        target.draw(actor);
    }
}

std::uint64_t Renderer::computeStaticSignature(const Level& level) const
{
    std::uint64_t hash = 1469598103934665603ull;

    for (int row = 0; row < level.height(); ++row) {
        for (int col = 0; col < level.width(); ++col) {
            const CellType cell = level.cellAt(row, col);
            if (cell == CellType::Wall || cell == CellType::Ladder ||
                cell == CellType::Teleport || cell == CellType::HiddenLadder) {
                hash ^= static_cast<std::uint64_t>(row * 131 + col * 17 + static_cast<int>(cell));
                hash *= 1099511628211ull;
            }
        }
    }

    return hash;
}

sf::Vector2f Renderer::cellToPixel(int row, int col) const
{
    return {
        m_levelOrigin.x + static_cast<float>(col) * m_tileSize,
        m_levelOrigin.y + static_cast<float>(row) * m_tileSize
    };
}
