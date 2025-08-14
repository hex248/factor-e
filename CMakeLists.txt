cmake_minimum_required(VERSION 3.11)
project(factor-e)

set(CMAKE_CXX_STANDARD 17)

find_package(raylib 5.5.0 QUIET)
if (NOT raylib_FOUND)
    message(STATUS "raylib not found: go to https://github.com/raysan5/raylib/releases, exiting.")
    return()
else()
    message(STATUS "raylib found: ${raylib_VERSION}")
endif()

include_directories(include)


add_executable(${PROJECT_NAME}
    src/main.cpp
    src/config.cpp
    src/player.cpp
    src/world.cpp
    src/controls.cpp
    src/ui.cpp
)

target_link_libraries(${PROJECT_NAME} raylib)

file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (WIN32)
    target_link_libraries(${PROJECT_NAME} winmm)
endif()