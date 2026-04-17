#pragma once

#include <vector>

#include "core/InputCommand.h"

/// Input snapshot for a single frame.
struct InputFrame {
    int horizontalAxis = 0; // -1 left, 0 none, +1 right
    std::vector<InputCommand> commands;
};

/// Translates raw keyboard state into gameplay commands.
class InputHandler {
public:
    /// Capture current keyboard state.
    /// Horizontal movement is continuous, vertical/teleport are one-shot on key press edges.
    InputFrame sample();

private:
    bool m_prevW = false;
    bool m_prevS = false;
    bool m_prevSpace = false;
};
