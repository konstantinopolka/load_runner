cmake_minimum_required(VERSION 3.24)
project(LoadRunner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- SFML 3.0 via FetchContent ---
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(SFML)

# --- Sources ---
set(SOURCES
    src/main.cpp
    src/core/GameSession.cpp
    src/input/InputHandler.cpp
    src/model/Level.cpp
    src/model/levels/LevelDefinitions.cpp
    src/model/PlayerState.cpp
    src/model/EnemyState.cpp
    src/renderer/Renderer.cpp
    src/renderer/HudRenderer.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics)

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
