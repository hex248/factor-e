#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

LINUX_BUILD_DIR="build-linux"
WINDOWS_BUILD_DIR="build-windows"

print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE} $1 ${NC}"
    echo -e "${BLUE}========================================${NC}"
}

if [ ! -f "$LINUX_BUILD_DIR/build_linux.log" ] && [ ! -f "$WINDOWS_BUILD_DIR/build_windows.log" ]; then
    echo "no build logs found. run './build-all.sh' first."
    exit 1
fi

print_header "build warnings comparison"

if [ -f "$LINUX_BUILD_DIR/build_linux.log" ]; then
    LINUX_WARNINGS=$(grep "warning:" "$LINUX_BUILD_DIR/build_linux.log" | wc -l)
    echo -e "${GREEN}linux warnings: $LINUX_WARNINGS${NC}"
    if [ "$LINUX_WARNINGS" -gt 0 ]; then
        echo "linux warning details:"
        grep "warning:" "$LINUX_BUILD_DIR/build_linux.log" | sed 's/^/  /'
        echo ""
    fi
else
    echo -e "${RED}linux build log not found${NC}"
fi

if [ -f "$WINDOWS_BUILD_DIR/build_windows.log" ]; then
    WINDOWS_WARNINGS=$(grep "warning:" "$WINDOWS_BUILD_DIR/build_windows.log" | wc -l)
    echo -e "${GREEN}windows warnings: $WINDOWS_WARNINGS${NC}"
    if [ "$WINDOWS_WARNINGS" -gt 0 ]; then
        echo "windows warning details:"
        grep "warning:" "$WINDOWS_BUILD_DIR/build_windows.log" | sed 's/^/  /'
        echo ""
    fi
else
    echo -e "${RED}windows build log not found${NC}"
fi

# compare warnings
if [ -f "$LINUX_BUILD_DIR/build_linux.log" ] && [ -f "$WINDOWS_BUILD_DIR/build_windows.log" ]; then
    print_header "platform-specific warnings"
    
    grep "warning:" "$LINUX_BUILD_DIR/build_linux.log" | sed 's/.*warning: //' | sort -u > /tmp/linux_warnings.tmp
    grep "warning:" "$WINDOWS_BUILD_DIR/build_windows.log" | sed 's/.*warning: //' | sort -u > /tmp/windows_warnings.tmp
    
    # warnings only in Linux
    LINUX_ONLY=$(comm -23 /tmp/linux_warnings.tmp /tmp/windows_warnings.tmp | wc -l)
    if [ "$LINUX_ONLY" -gt 0 ]; then
        echo -e "${YELLOW}warnings only in linux build ($LINUX_ONLY):${NC}"
        comm -23 /tmp/linux_warnings.tmp /tmp/windows_warnings.tmp | sed 's/^/  /'
        echo ""
    fi
    
    # warnings only in Windows
    WINDOWS_ONLY=$(comm -13 /tmp/linux_warnings.tmp /tmp/windows_warnings.tmp | wc -l)
    if [ "$WINDOWS_ONLY" -gt 0 ]; then
        echo -e "${YELLOW}warnings only in windows build ($WINDOWS_ONLY):${NC}"
        comm -13 /tmp/linux_warnings.tmp /tmp/windows_warnings.tmp | sed 's/^/  /'
        echo ""
    fi
    
    # common warnings
    COMMON=$(comm -12 /tmp/linux_warnings.tmp /tmp/windows_warnings.tmp | wc -l)
    if [ "$COMMON" -gt 0 ]; then
        echo -e "${GREEN}common warnings in both builds ($COMMON):${NC}"
        comm -12 /tmp/linux_warnings.tmp /tmp/windows_warnings.tmp | sed 's/^/  /'
        echo ""
    fi
    
    rm -f /tmp/linux_warnings.tmp /tmp/windows_warnings.tmp
fi

print_header "error analysis"

# check for errors
if [ -f "$LINUX_BUILD_DIR/build_linux.log" ]; then
    LINUX_ERRORS=$(grep -i "error:" "$LINUX_BUILD_DIR/build_linux.log" | wc -l)
    if [ "$LINUX_ERRORS" -gt 0 ]; then
        echo -e "${RED}linux errors: $LINUX_ERRORS${NC}"
        grep -i "error:" "$LINUX_BUILD_DIR/build_linux.log" | sed 's/^/  /'
        echo ""
    else
        echo -e "${GREEN}linux: no errors${NC}"
    fi
fi

if [ -f "$WINDOWS_BUILD_DIR/build_windows.log" ]; then
    WINDOWS_ERRORS=$(grep -i "error:" "$WINDOWS_BUILD_DIR/build_windows.log" | wc -l)
    if [ "$WINDOWS_ERRORS" -gt 0 ]; then
        echo -e "${RED}windows errors: $WINDOWS_ERRORS${NC}"
        grep -i "error:" "$WINDOWS_BUILD_DIR/build_windows.log" | sed 's/^/  /'
        echo ""
    else
        echo -e "${GREEN}windows: no errors${NC}"
    fi
fi
