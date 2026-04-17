#include <SFML/Graphics.hpp>
#include <iostream>

#include "core/GameSession.h"
#include "input/InputHandler.h"
#include "model/Level.h"
#include "model/levels/LevelDefinitions.h"
#include "renderer/Renderer.h"

int main()
{
    constexpr int firstLevelId = 1;
    constexpr int expectedAuthoredLevels = 3;
    constexpr int startLives = 3;

    const int totalLevels = levelDefinitionCount();
    if (totalLevels != expectedAuthoredLevels) {
        std::cerr << "Configuration error: expected exactly " << expectedAuthoredLevels
                  << " authored levels, found " << totalLevels << '\n';
        return 1;
    }

    auto makeSession = [](int levelId, int lives) {
        return GameSession(Level::loadFromDefinition(levelId), lives, static_cast<unsigned int>(levelId));
    };

    auto printLevelBootstrap = [totalLevels](const GameSession& session, int levelId) {
        std::cout << "Level loaded: " << session.level().width() << "x" << session.level().height() << '\n';
        std::cout << "Level id: " << levelId << " / " << totalLevels << '\n';
        std::cout << "Player spawn: row=" << session.level().playerSpawn().row
                  << " col=" << session.level().playerSpawn().col << '\n';
        std::cout << "Initial gold count: " << session.level().goldCount() << '\n';
        std::cout << "Teleport uses: " << session.teleportUsesRemaining() << '\n';
        std::cout << "Enemy spawns: " << session.enemies().size() << '\n';
    };

    int currentLevelId = firstLevelId;
    int campaignLives = startLives;
    GameSession session = makeSession(currentLevelId, campaignLives);
    printLevelBootstrap(session, currentLevelId);

    sf::RenderWindow window(sf::VideoMode({800u, 600u}), "LoadRunner");

    sf::Clock frameClock;
    float accumulator = 0.0f;
    float elapsedSeconds = 0.0f;
    constexpr float fixedDt = 1.0f / 60.0f;

    bool gameOverPrinted = false;
    bool campaignWon = false;
    bool campaignWonPrinted = false;
    InputHandler inputHandler;
    Renderer renderer;

    try {
        while (window.isOpen()) {
            while (const auto event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>() ||
                    (event->is<sf::Event::KeyPressed>() &&
                     event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)) {
                    window.close();
                }
            }

            if (!campaignWon && !session.isGameOver() && !session.isLevelComplete()) {
                const InputFrame inputFrame = inputHandler.sample();
                session.setHorizontalInput(inputFrame.horizontalAxis);
                for (const InputCommand command : inputFrame.commands) {
                    session.queueCommand(command);
                }
            } else {
                session.setHorizontalInput(0);
            }

            accumulator += frameClock.restart().asSeconds();
            if (accumulator > 0.25f) {
                accumulator = 0.25f;
            }

            while (accumulator >= fixedDt) {
                if (!campaignWon) {
                    session.update(fixedDt);
                }

                if (!campaignWon && session.status() == GameStatus::Running) {
                    elapsedSeconds += fixedDt;
                }

                accumulator -= fixedDt;
            }

            if (session.isGameOver() && !gameOverPrinted) {
                std::cout << "Game over: lives exhausted." << '\n';
                std::cout << "Final stats: steps=" << session.player().steps
                          << " gold=" << session.player().goldCollected
                          << " teleports_left=" << session.teleportUsesRemaining() << '\n';
                gameOverPrinted = true;
            }

            if (campaignWon && !campaignWonPrinted) {
                std::cout << "Campaign complete: all three levels cleared." << '\n';
                std::cout << "Final stats: lives_left=" << campaignLives << '\n';
                campaignWonPrinted = true;
            }

            const bool levelCompletedThisFrame = session.isLevelComplete();

            window.clear(sf::Color(20, 24, 30));
            renderer.draw(window, session, elapsedSeconds, currentLevelId, campaignWon);
            window.display();

            if (!campaignWon && levelCompletedThisFrame) {
                campaignLives = session.player().lives;

                if (currentLevelId >= totalLevels) {
                    campaignWon = true;
                } else {
                    ++currentLevelId;
                    elapsedSeconds = 0.0f;
                    gameOverPrinted = false;

                    session = makeSession(currentLevelId, campaignLives);
                    printLevelBootstrap(session, currentLevelId);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
