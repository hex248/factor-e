#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

LINUX_BUILD_DIR="build-linux"
WINDOWS_BUILD_DIR="build-windows"

LINUX_SUCCESS=0
WINDOWS_SUCCESS=0
TOTAL_ERRORS_LINUX=0
TOTAL_ERRORS_WINDOWS=0
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
    if ./build.sh; then
        LINUX_SUCCESS=1
        # extract warning count from build.sh output
        if [ -f "$LINUX_BUILD_DIR/build_linux.log" ]; then
            TOTAL_ERRORS_LINUX=$(grep "error:" $LINUX_BUILD_DIR/build_linux.log | grep -v "_deps/" | wc -l || echo "0")
            TOTAL_WARNINGS_LINUX=$(grep "warning:" $LINUX_BUILD_DIR/build_linux.log | grep -v "_deps/" | wc -l || echo "0")
        else
            TOTAL_ERRORS_LINUX=0
            TOTAL_WARNINGS_LINUX=0
        fi
    else
        LINUX_SUCCESS=0
        # even on failure, count warnings and errors
        if [ -f "$LINUX_BUILD_DIR/build_linux.log" ]; then
            TOTAL_ERRORS_LINUX=$(grep "error:" $LINUX_BUILD_DIR/build_linux.log | grep -v "_deps/" | wc -l || echo "0")
            TOTAL_WARNINGS_LINUX=$(grep "warning:" $LINUX_BUILD_DIR/build_linux.log | grep -v "_deps/" | wc -l || echo "0")
        else
            TOTAL_ERRORS_LINUX=0
            TOTAL_WARNINGS_LINUX=0
        fi
        return 1
    fi
    
    return 0
}

build_windows() {
    print_header "building for windows (cross-compile)"
    
    mkdir -p "$WINDOWS_BUILD_DIR"
    cd "$WINDOWS_BUILD_DIR"
    
    echo "running cmake for windows..."
    if cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
        -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
        -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres \
        -DCMAKE_FIND_ROOT_PATH=/usr/x86_64-w64-mingw32 \
        -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
        -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
        -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
        2>&1 | tee cmake_windows.log; then
        print_success "cmake configuration successful for windows"
    else
        print_error "cmake configuration failed for windows"
        cd ..
        return 1
    fi
    
    echo "building for windows..."
    if make -j$(nproc) 2>&1 | awk '
        /_deps/ { skip_mode = 1; next }
        /Building.*_deps/ { next }
        /^\[.*%\].*_deps/ { next }
        /^In file included.*_deps/ { skip_mode = 1; next }
        /^In file included/ && skip_mode { next }
        skip_mode && /^\/.*\.(c|cpp|h|hpp):/ { skip_mode = 0; next }
        skip_mode && /^\s*\|\s*\^/ { next }
        skip_mode && /note:/ { next }
        skip_mode && /warning:/ { next }
        skip_mode && /error:/ { next }
        skip_mode && /^\s*[0-9]+\s*\|/ { next }
        skip_mode && /^\s*\|\s*/ { next }
        skip_mode { next }
        /^\s*[0-9]+\s*\|/ { next }
        /^\s*\|\s*\^/ { next }
        { print; skip_mode = 0 }
    ' | tee build_windows.log; then
        WINDOWS_SUCCESS=1
    else
        print_error "windows build failed"
        cd ..
        return 1
    fi
    
    TOTAL_ERRORS_WINDOWS=$(grep "error:" build_windows.log | grep -v "_deps/" | wc -l || echo "0")
    TOTAL_WARNINGS_WINDOWS=$(grep "warning:" build_windows.log | grep -v "_deps/" | wc -l || echo "0")
    
    if [ "$TOTAL_ERRORS_WINDOWS" -gt 0 ]; then
        print_error "windows build failed with $TOTAL_ERRORS_WINDOWS errors"
        WINDOWS_SUCCESS=0
        cd ..
        return 1
    elif [ "$WINDOWS_SUCCESS" -eq 1 ]; then
        print_success "windows build successful"
    fi

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
        if [ "$TOTAL_ERRORS_LINUX" -gt 0 ] || [ "$TOTAL_WARNINGS_LINUX" -gt 0 ]; then
            print_success "linux: success ($TOTAL_ERRORS_LINUX errors, $TOTAL_WARNINGS_LINUX warnings)"
        else
            print_success "linux: success (0 errors, 0 warnings)"
        fi
    else
        print_error "linux: failed ($TOTAL_ERRORS_LINUX errors, $TOTAL_WARNINGS_LINUX warnings)"
    fi
    
    if [ "$WINDOWS_SUCCESS" -eq 1 ]; then
        if [ "$TOTAL_ERRORS_WINDOWS" -gt 0 ] || [ "$TOTAL_WARNINGS_WINDOWS" -gt 0 ]; then
            print_success "windows: success ($TOTAL_ERRORS_WINDOWS errors, $TOTAL_WARNINGS_WINDOWS warnings)"
        else
            print_success "windows: success (0 errors, 0 warnings)"
        fi
    else
        print_error "windows: failed ($TOTAL_ERRORS_WINDOWS errors, $TOTAL_WARNINGS_WINDOWS warnings)"
    fi
    
    echo ""
    echo "build artifacts:"
    [ -f "$LINUX_BUILD_DIR/factor-e" ] && echo "  linux executable: $LINUX_BUILD_DIR/factor-e"
    [ -f "$WINDOWS_BUILD_DIR/factor-e.exe" ] && echo "  windows executable: $WINDOWS_BUILD_DIR/factor-e.exe"
    
    echo ""
    echo "build logs:"
    [ -f "$LINUX_BUILD_DIR/build_linux.log" ] && echo "  linux build log: $LINUX_BUILD_DIR/build_linux.log"
    [ -f "$WINDOWS_BUILD_DIR/build_windows.log" ] && echo "  windows build log: $WINDOWS_BUILD_DIR/build_windows.log"
    
    if  [ "$TOTAL_ERRORS_LINUX" -gt 0 ] || [ "$TOTAL_ERRORS_WINDOWS" -gt 0 ] || [ "$TOTAL_WARNINGS_LINUX" -gt 0 ] || [ "$TOTAL_WARNINGS_WINDOWS" -gt 0 ]; then
        echo ""
        print_warning "to view details:"
        [ "$TOTAL_ERRORS_LINUX" -gt 0 ] && echo "  linux errors: grep 'error:' $LINUX_BUILD_DIR/build_linux.log | grep -v '_deps/'"
        [ "$TOTAL_ERRORS_WINDOWS" -gt 0 ] && echo "  windows errors: grep 'error:' $WINDOWS_BUILD_DIR/build_windows.log | grep -v '_deps/'"
        [ "$TOTAL_WARNINGS_LINUX" -gt 0 ] && echo "  linux warnings: grep 'warning:' $LINUX_BUILD_DIR/build_linux.log | grep -v '_deps/'"
        [ "$TOTAL_WARNINGS_WINDOWS" -gt 0 ] && echo "  windows warnings: grep 'warning:' $WINDOWS_BUILD_DIR/build_windows.log | grep -v '_deps/'"
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
