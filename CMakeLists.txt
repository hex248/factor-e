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
elseif (UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE m pthread dl)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(X11 REQUIRED x11)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES})
elseif (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${COCOA_LIBRARY} ${IOKIT_LIBRARY})
endif()