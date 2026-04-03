cmake_minimum_required(VERSION 3.16)
project(snake_game VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(snake_game_plugin MODULE
    src/SnakeGamePlugin.h
    src/SnakeGamePlugin.cpp
)

set_target_properties(snake_game_plugin PROPERTIES PREFIX "")

target_link_libraries(snake_game_plugin PRIVATE
    Qt6::Core
)

target_include_directories(snake_game_plugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LOGOS_CPP_SDK_ROOT}/include
    ${LOGOS_CPP_SDK_ROOT}/include/cpp
    ${LOGOS_MODULE_ROOT}/include
    ${LOGOS_MODULE_ROOT}/include/module_lib
)

install(TARGETS snake_game_plugin
    LIBRARY DESTINATION modules/snake_game
)

install(FILES
    src/plugin_metadata.json
    DESTINATION modules/snake_game
    RENAME manifest.json
)
