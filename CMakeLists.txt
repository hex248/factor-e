cmake_minimum_required(VERSION 3.11)
project(factor-e)

set(CMAKE_CXX_STANDARD 17)

# attempt to find raylib
find_package(raylib 5.5.0 QUIET)

# if raylib wasn't found
if(NOT raylib_FOUND)
    message(STATUS "raylib not found as system package, fetching from github")
    # use FetchContent to download and build raylib as fallback
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
    )
    FetchContent_MakeAvailable(raylib)
    message(STATUS "raylib fetched and built: 5.5")
else()
    message(STATUS "raylib found as system package")
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

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "copying assets to output directory"
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
endif()