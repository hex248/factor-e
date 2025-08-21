#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

BUILD_DIR="build-linux"

if [ -f "$BUILD_DIR/factor-e" ]; then
    echo -e "${GREEN}running linux version...${NC}"
    cd "$BUILD_DIR"
    ./factor-e
    cd ..
else
    echo -e "${RED}error: factor-e not found in \"$BUILD_DIR\"${NC}"
    echo -e "${YELLOW}make sure you have built the project first with: \"./build.sh\"${NC}"
    exit 1
fi
