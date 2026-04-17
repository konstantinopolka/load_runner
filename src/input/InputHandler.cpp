#include "InputHandler.h"

#include <SFML/Window/Keyboard.hpp>

namespace {

int clampAxis(int value)
{
    if (value < 0) {
        return -1;
    }
    if (value > 0) {
        return 1;
    }
    return 0;
}

} // namespace

InputFrame InputHandler::sample()
{
    InputFrame frame{};

    const bool holdLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
    const bool holdRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
    frame.horizontalAxis = clampAxis((holdRight ? 1 : 0) - (holdLeft ? 1 : 0));

    const bool holdW = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
    const bool holdS = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
    const bool holdSpace = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (holdW && !m_prevW) {
        frame.commands.push_back(InputCommand::MoveUp);
    }
    if (holdS && !m_prevS) {
        frame.commands.push_back(InputCommand::MoveDown);
    }
    if (holdSpace && !m_prevSpace) {
        frame.commands.push_back(InputCommand::Teleport);
    }

    m_prevW = holdW;
    m_prevS = holdS;
    m_prevSpace = holdSpace;

    return frame;
}
