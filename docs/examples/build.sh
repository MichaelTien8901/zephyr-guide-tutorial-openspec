#!/bin/bash
#
# Local build script for Zephyr examples using Docker
#
# Usage:
#   ./build.sh                    # Build all examples
#   ./build.sh part1/hello-world  # Build specific example
#   ./build.sh --list             # List all examples
#   ./build.sh --clean            # Clean build artifacts
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IMAGE_NAME="zephyr-tutorial-dev"
DEFAULT_BOARD="stm32f769i_disco"
BLE_BOARD="nrf52840dk_nrf52840"

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# All examples with their boards
declare -A EXAMPLES=(
    ["part1/hello-world"]="$DEFAULT_BOARD"
    ["part1/blinky"]="$DEFAULT_BOARD"
    ["part3/threads"]="$DEFAULT_BOARD"
    ["part3/timers"]="$DEFAULT_BOARD"
    ["part3/workqueue"]="$DEFAULT_BOARD"
    ["part4/mutex"]="$DEFAULT_BOARD"
    ["part4/semaphore"]="$DEFAULT_BOARD"
    ["part4/msgq"]="$DEFAULT_BOARD"
    ["part4/zbus"]="$DEFAULT_BOARD"
    ["part5/gpio"]="$DEFAULT_BOARD"
    ["part5/i2c-sensor"]="$DEFAULT_BOARD"
    ["part5/uart"]="$DEFAULT_BOARD"
    ["part6/tcp-client"]="$DEFAULT_BOARD"
    ["part6/mqtt"]="$DEFAULT_BOARD"
    ["part6/ble-peripheral"]="$BLE_BOARD"
)

print_usage() {
    echo "Usage: $0 [OPTIONS] [EXAMPLE]"
    echo ""
    echo "Options:"
    echo "  --list          List all available examples"
    echo "  --clean         Clean all build artifacts"
    echo "  --build-image   Build/rebuild the Docker image"
    echo "  --help          Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                       # Build all examples"
    echo "  $0 part1/hello-world     # Build specific example"
    echo "  $0 part3/threads         # Build threads example"
}

list_examples() {
    echo "Available examples:"
    echo ""
    for example in "${!EXAMPLES[@]}"; do
        printf "  %-25s (board: %s)\n" "$example" "${EXAMPLES[$example]}"
    done | sort
}

check_docker() {
    if ! command -v docker &> /dev/null; then
        echo -e "${RED}Error: Docker is not installed${NC}"
        exit 1
    fi
}

build_image() {
    echo -e "${YELLOW}Building Docker image...${NC}"
    docker build -t "$IMAGE_NAME" "$SCRIPT_DIR"
    echo -e "${GREEN}Docker image built successfully${NC}"
}

ensure_image() {
    if ! docker image inspect "$IMAGE_NAME" &> /dev/null; then
        echo -e "${YELLOW}Docker image not found. Building...${NC}"
        build_image
    fi
}

build_example() {
    local example="$1"
    local board="${EXAMPLES[$example]}"

    if [ -z "$board" ]; then
        echo -e "${RED}Error: Unknown example '$example'${NC}"
        echo "Use --list to see available examples"
        return 1
    fi

    echo -e "${YELLOW}Building $example for $board...${NC}"

    # Create build directory
    local build_dir="$SCRIPT_DIR/build/$example"
    mkdir -p "$build_dir"

    # Run build in Docker
    docker run --rm \
        -v "$SCRIPT_DIR:/workdir" \
        -w /home/zephyr/zephyrproject \
        "$IMAGE_NAME" \
        west build -b "$board" "/workdir/$example" \
            --build-dir "/workdir/build/$example" \
            -- -DBOARD="$board"

    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ $example built successfully${NC}"
        return 0
    else
        echo -e "${RED}✗ $example build failed${NC}"
        return 1
    fi
}

build_all() {
    local failed=()
    local passed=()

    echo -e "${YELLOW}Building all examples...${NC}"
    echo ""

    for example in "${!EXAMPLES[@]}"; do
        if build_example "$example"; then
            passed+=("$example")
        else
            failed+=("$example")
        fi
        echo ""
    done

    echo "=========================================="
    echo -e "${GREEN}Passed: ${#passed[@]}${NC}"
    for ex in "${passed[@]}"; do
        echo -e "  ${GREEN}✓${NC} $ex"
    done

    if [ ${#failed[@]} -gt 0 ]; then
        echo -e "${RED}Failed: ${#failed[@]}${NC}"
        for ex in "${failed[@]}"; do
            echo -e "  ${RED}✗${NC} $ex"
        done
        exit 1
    fi
}

clean_builds() {
    echo -e "${YELLOW}Cleaning build artifacts...${NC}"
    rm -rf "$SCRIPT_DIR/build"
    echo -e "${GREEN}Build artifacts cleaned${NC}"
}

# Main
check_docker

case "${1:-}" in
    --help|-h)
        print_usage
        exit 0
        ;;
    --list|-l)
        list_examples
        exit 0
        ;;
    --clean|-c)
        clean_builds
        exit 0
        ;;
    --build-image)
        build_image
        exit 0
        ;;
    "")
        ensure_image
        build_all
        ;;
    *)
        ensure_image
        build_example "$1"
        ;;
esac
