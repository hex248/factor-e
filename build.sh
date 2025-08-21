#!/bin/bash

# exit on any error
set -e

is_wayland() {
    [ "$WAYLAND_DISPLAY" ] || [ "$XDG_SESSION_TYPE" = "wayland" ]
}

cleanup_build() {
    if [ -d "build" ]; then
        echo "cleaning old build directory..."
        rm -rf build
    fi
}

if [ "$1" = "clean" ]; then
    cleanup_build
    echo "build directory cleaned."
    exit 0
fi

mkdir -p build
cd build

if is_wayland; then
    if cmake ..; then
        echo "CMAKE successful"
    else
        echo "CMAKE failed"
        exit 1
    fi
else
    if cmake ..; then
        echo "CMAKE successful"
    else
        echo "CMAKE failed"
        exit 1
    fi
fi

if is_wayland; then
    if make GLFW_LINUX_ENABLE_WAYLAND=TRUE; then
        echo "BUILD successful"
    else
        echo "BUILD failed"
        exit 1
    fi
else
    if make; then
        echo "BUILD successful"
    else
        echo "BUILD failed"
        exit 1
    fi
fi