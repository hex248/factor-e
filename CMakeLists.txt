cmake_minimum_required(VERSION 3.11)
project(factor-e)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# enable comprehensive warnings to catch cross-platform issues
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# attempt to find raylib
# skip finding system raylib when cross-compiling for Windows
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(raylib 5.5.0 QUIET)
endif()

# if raylib wasn't found or we're cross-compiling for Windows
if(NOT raylib_FOUND)
    message(STATUS "raylib not found as system package, fetching from github")
    # use FetchContent to download and build raylib as fallback
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
    )
    
    # disable warnings for external dependencies
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    set(BUILD_EXAMPLES OFF)
    set(BUILD_GAMES OFF)
    
    FetchContent_MakeAvailable(raylib)
    
    # disable warnings for raylib target if it exists
    if(TARGET raylib)
        target_compile_options(raylib PRIVATE -w)
    endif()
    
    message(STATUS "raylib fetched and built: 5.5")
else()
    message(STATUS "raylib found as system package")
endif()

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/config.cpp
    src/player.cpp
    src/world.cpp
    src/controls.cpp
    src/ui.cpp
    src/mouse.cpp
    src/item.cpp
    src/tex.cpp
)

# platform-specific compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # gcc and clang warnings
    set(WARNING_FLAGS 
        -Wall -Wextra -Wpedantic
        -Wformat-security
        -Wunused
        -Wshadow
        -Wcast-align
        -Wpointer-arith
        -Wwrite-strings
        -Wredundant-decls
        -Winit-self
        -Wconversion
        -Wsign-conversion
    )
    
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        # gcc specific warnings
        list(APPEND WARNING_FLAGS -Wlogical-op -Wduplicated-cond)
    endif()
    
    target_compile_options(${PROJECT_NAME} PRIVATE ${WARNING_FLAGS})
elseif(MSVC)
    # msvc warnings
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
endif()

# enable debug symbols in release builds for better error reporting
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE -g)
    elseif(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /Zi)
        target_link_options(${PROJECT_NAME} PRIVATE /DEBUG)
    endif()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "copying assets to output directory"
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
endif()