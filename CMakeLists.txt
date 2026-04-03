cmake_minimum_required(VERSION 3.16)
project(snake_game VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

# Use environment variables (set by nix develop in keycard-basecamp flake)
if(DEFINED ENV{LOGOS_CPP_SDK_ROOT})
    set(LOGOS_CPP_SDK "$ENV{LOGOS_CPP_SDK_ROOT}")
endif()
if(DEFINED ENV{LOGOS_MODULE_HEADERS})
    set(LOGOS_MODULE "$ENV{LOGOS_MODULE_HEADERS}")
endif()

# High Score module (PluginInterface pattern — works with our runtime)
add_library(high_score_plugin MODULE
    src/HighScorePlugin.h
    src/HighScorePlugin.cpp
)

set_target_properties(high_score_plugin PROPERTIES PREFIX "")

target_link_libraries(high_score_plugin PRIVATE
    Qt6::Core
)

target_include_directories(high_score_plugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LOGOS_CPP_SDK}/include
    ${LOGOS_CPP_SDK}/include/cpp
    ${LOGOS_MODULE}/include
    ${LOGOS_MODULE}
    # Also support direct CMake variables from logos-module-builder
    ${LOGOS_CPP_SDK_ROOT}/include
    ${LOGOS_CPP_SDK_ROOT}/include/cpp
    ${LOGOS_MODULE_ROOT}/include
    ${LOGOS_MODULE_ROOT}/include/module_lib
)

install(TARGETS high_score_plugin
    LIBRARY DESTINATION modules/high_score
)

install(FILES src/high_score_metadata.json
    DESTINATION modules/high_score
    RENAME manifest.json
)

# QML UI plugin
install(FILES
    plugins/snake-ui/Main.qml
    plugins/snake-ui/manifest.json
    plugins/snake-ui/metadata.json
    DESTINATION plugins/snake-ui
)
