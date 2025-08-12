#!/bin/bash

# exit on any error
set -e

echo "current step: build"

cd build

echo "current step: cmake"
if cmake ..; then
    echo "CMAKE successful"
else
    echo "CMAKE failed"
    exit 1
fi

echo "current step: make"
if make; then
    echo "BUILD successful"
else
    echo "BUILD failed"
    exit 1
fi

echo "current step: run"
./factor-e
