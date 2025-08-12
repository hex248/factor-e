#!/bin/bash

# exit on any error
set -e

echo "current step: build"

cd build

echo "current step: cmake"
if [ "$WAYLAND_DISPLAY" ] || [ "$XDG_SESSION_TYPE" = "wayland" ]; then
    if cmake -DGLFW_BUILD_WAYLAND=ON ..; then
        echo "CMAKE successful"
    fi
else
    if cmake ..; then
        echo "CMAKE successful"
    fi
fi
else
    echo "CMAKE failed"
    exit 1
fi

echo "current step: make"
if [ "$WAYLAND_DISPLAY" ] || [ "$XDG_SESSION_TYPE" = "wayland" ]; then
    if make GLFW_LINUX_ENABLE_WAYLAND=TRUE; then
        echo "BUILD successful"
    fi
else
    if make; then
        echo "BUILD successful"
    fi
fi
else
    echo "BUILD failed"
    exit 1
fi

echo "current step: run"
./factor-e
