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

# copy assets to the output directory after building
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "copying assets to output directory"
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
endif()