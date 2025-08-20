cmake_minimum_required(VERSION 3.11)
project(factor-e)

set(CMAKE_CXX_STANDARD 17)

find_package(raylib CONFIG REQUIRED)

if (raylib_FOUND)
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
    src/mouse.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
endif()