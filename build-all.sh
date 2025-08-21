#!/bin/bash

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

LINUX_BUILD_DIR="build-linux"
WINDOWS_BUILD_DIR="build-windows"

LINUX_SUCCESS=0
WINDOWS_SUCCESS=0
TOTAL_WARNINGS_LINUX=0
TOTAL_WARNINGS_WINDOWS=0

print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE} $1 ${NC}"
    echo -e "${BLUE}========================================${NC}"
}

print_success() {
    echo -e "${GREEN}$1${NC}"
}

print_error() {
    echo -e "${RED}$1${NC}"
}

print_warning() {
    echo -e "${YELLOW}$1${NC}"
}

cleanup_builds() {
    echo "cleaning old build directories..."
    [ -d "$LINUX_BUILD_DIR" ] && rm -rf "$LINUX_BUILD_DIR"
    [ -d "$WINDOWS_BUILD_DIR" ] && rm -rf "$WINDOWS_BUILD_DIR"
    echo "build directories cleaned"
}

build_linux() {
    print_header "building for linux"
    
    mkdir -p "$LINUX_BUILD_DIR"
    cd "$LINUX_BUILD_DIR"
    
    echo "running cmake for linux..."
    if cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-Wall -Wextra -Wpedantic" -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic" 2>&1 | tee cmake_linux.log; then
        print_success "cmake configuration successful for linux"
    else
        print_error "cmake configuration failed for linux"
        cd ..
        return 1
    fi
    
    echo "building for linux..."
    if make -j$(nproc) 2>&1 | tee build_linux.log; then
        print_success "linux build successful"
        LINUX_SUCCESS=1
    else
        print_error "linux build failed"
        cd ..
        return 1
    fi
    
    TOTAL_WARNINGS_LINUX=$(grep -c "warning:" build_linux.log || echo "0")
    if [ "$TOTAL_WARNINGS_LINUX" -gt 0 ]; then
        print_warning "linux build completed with $TOTAL_WARNINGS_LINUX warnings"
    fi
    
    cd ..
    return 0
}

build_windows() {
    print_header "building for windows (cross-compile)"
    
    mkdir -p "$WINDOWS_BUILD_DIR"
    cd "$WINDOWS_BUILD_DIR"
    
    echo "running cmake for windows..."
    if cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/windows-toolchain.cmake -DCMAKE_BUILD_TYPE=Release 2>&1 | tee cmake_windows.log; then
        print_success "cmake configuration successful for windows"
    else
        print_error "cmake configuration failed for windows"
        cd ..
        return 1
    fi
    
    echo "building for windows..."
    if make -j$(nproc) 2>&1 | tee build_windows.log; then
        print_success "windows build successful"
        WINDOWS_SUCCESS=1
    else
        print_error "windows build failed"
        cd ..
        return 1
    fi
    
    TOTAL_WARNINGS_WINDOWS=$(grep -c "warning:" build_windows.log || echo "0")
    if [ "$TOTAL_WARNINGS_WINDOWS" -gt 0 ]; then
        print_warning "windows build completed with $TOTAL_WARNINGS_WINDOWS warnings"
    fi
    
    cd ..
    return 0
}

show_summary() {
    print_header "build summary"
    
    echo "platform results:"
    if [ "$LINUX_SUCCESS" -eq 1 ]; then
        print_success "linux: success ($TOTAL_WARNINGS_LINUX warnings)"
    else
        print_error "linux: failed"
    fi
    
    if [ "$WINDOWS_SUCCESS" -eq 1 ]; then
        print_success "windows: success ($TOTAL_WARNINGS_WINDOWS warnings)"
    else
        print_error "windows: failed"
    fi
    
    echo ""
    echo "build artifacts:"
    [ -f "$LINUX_BUILD_DIR/factor-e" ] && echo "  linux executable: $LINUX_BUILD_DIR/factor-e"
    [ -f "$WINDOWS_BUILD_DIR/factor-e.exe" ] && echo "  windows executable: $WINDOWS_BUILD_DIR/factor-e.exe"
    
    echo ""
    echo "build logs:"
    [ -f "$LINUX_BUILD_DIR/build_linux.log" ] && echo "  linux build log: $LINUX_BUILD_DIR/build_linux.log"
    [ -f "$WINDOWS_BUILD_DIR/build_windows.log" ] && echo "  windows build log: $WINDOWS_BUILD_DIR/build_windows.log"
    
    if [ "$TOTAL_WARNINGS_LINUX" -gt 0 ] || [ "$TOTAL_WARNINGS_WINDOWS" -gt 0 ]; then
        echo ""
        print_warning "to view warnings in detail:"
        [ "$TOTAL_WARNINGS_LINUX" -gt 0 ] && echo "  linux warnings: grep 'warning:' $LINUX_BUILD_DIR/build_linux.log"
        [ "$TOTAL_WARNINGS_WINDOWS" -gt 0 ] && echo "  windows warnings: grep 'warning:' $WINDOWS_BUILD_DIR/build_windows.log"
    fi
}

case "$1" in
    "clean")
        cleanup_builds
        exit 0
        ;;
    "linux")
        build_linux
        exit $?
        ;;
    "windows")
        build_windows
        exit $?
        ;;
    "")
        print_header "cross-platform build for factor-e"
        echo "building for both linux and windows..."
        echo ""
        
        build_linux
        LINUX_RESULT=$?
        
        echo ""
        
        build_windows
        WINDOWS_RESULT=$?
        
        echo ""
        show_summary
        
        if [ "$LINUX_RESULT" -ne 0 ] || [ "$WINDOWS_RESULT" -ne 0 ]; then
            exit 1
        else
            print_success "all builds completed successfully"
            exit 0
        fi
        ;;
    *)
        echo "usage: $0 [clean|linux|windows]"
        echo "  clean   - clean build directories"
        echo "  linux   - build only for linux"
        echo "  windows - build only for windows"
        echo "  (no arg) - build for both platforms"
        exit 1
        ;;
esac
