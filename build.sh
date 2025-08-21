#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

BUILD_DIR="build-linux"
BUILD_SUCCESS=0
TOTAL_WARNINGS=0
TOTAL_ERRORS=0

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

cleanup_build() {
    echo "cleaning old build directory..."
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
    echo "build directory cleaned."
}

if [ "$1" = "clean" ]; then
    cleanup_build
    exit 0
fi

print_header "building for linux"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "running cmake for linux..."
if cmake .. -DCMAKE_BUILD_TYPE=Release 2>&1 | tee cmake_linux.log; then
    print_success "cmake configuration successful for linux"
else
    print_error "cmake configuration failed for linux"
    cd ..
    exit 1
fi

echo "building for linux..."
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
' | tee build_linux.log; then
    BUILD_SUCCESS=1
else
    print_error "linux build failed"
    BUILD_SUCCESS=0
fi

TOTAL_WARNINGS=$(grep "warning:" build_linux.log | grep -v "_deps/" | wc -l || echo "0")
TOTAL_ERRORS=$(grep "error:" build_linux.log | grep -v "_deps/" | wc -l || echo "0")

if [ "$TOTAL_ERRORS" -gt 0 ]; then
    print_error "linux build failed with $TOTAL_ERRORS errors"
    BUILD_SUCCESS=0
elif [ "$BUILD_SUCCESS" -eq 1 ]; then
    print_success "linux build successful"
fi

if [ "$TOTAL_WARNINGS" -gt 0 ]; then
    print_warning "linux build completed with $TOTAL_WARNINGS warnings"
fi

cd ..

print_header "build summary"
echo "platform results:"
if [ "$BUILD_SUCCESS" -eq 1 ]; then
    if [ "$TOTAL_ERRORS" -gt 0 ] || [ "$TOTAL_WARNINGS" -gt 0 ]; then
        print_success "linux: success ($TOTAL_ERRORS errors, $TOTAL_WARNINGS warnings)"
    else
        print_success "linux: success (0 errors, 0 warnings)"
    fi
else
    print_error "linux: failed ($TOTAL_ERRORS errors, $TOTAL_WARNINGS warnings)"
fi

echo ""
echo "build artifacts:"
[ -f "$BUILD_DIR/factor-e" ] && echo "  linux executable: $BUILD_DIR/factor-e"

echo ""
echo "build logs:"
echo "  linux build log: $BUILD_DIR/build_linux.log"
echo "  linux cmake log: $BUILD_DIR/cmake_linux.log"

if [ "$TOTAL_WARNINGS" -gt 0 ] || [ "$TOTAL_ERRORS" -gt 0 ]; then
    echo ""
    print_warning "to view details:"
    [ "$TOTAL_ERRORS" -gt 0 ] && echo "  linux errors: grep 'error:' $BUILD_DIR/build_linux.log | grep -v '_deps/'"
    [ "$TOTAL_WARNINGS" -gt 0 ] && echo "  linux warnings: grep 'warning:' $BUILD_DIR/build_linux.log | grep -v '_deps/'"
fi

if [ "$BUILD_SUCCESS" -eq 1 ] && [ "$TOTAL_ERRORS" -eq 0 ]; then
    print_success "build completed successfully"
    exit 0
else
    exit 1
fi