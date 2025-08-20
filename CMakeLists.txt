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

# asset dependencies
file(GLOB_RECURSE ASSET_FILES "${CMAKE_SOURCE_DIR}/assets/*")

# copy assets when newer than stamp file
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/assets_copied.stamp
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/assets_copied.stamp # create stamp file to mark that assets have been copied
    DEPENDS ${ASSET_FILES}
    COMMENT "copying assets to output directory"
)

add_custom_target(copy_assets ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/assets_copied.stamp)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
endif()