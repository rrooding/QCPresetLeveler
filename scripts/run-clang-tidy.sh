#!/usr/bin/env bash
# Runs clang-tidy against our own sources (src/Main.cpp pulls in every header
# transitively, so this is sufficient to cover all of src/).
#
# On macOS, clang-tidy (Homebrew/LLVM) doesn't resolve the SDK sysroot the way
# AppleClang does implicitly, so it fails to find <TargetConditionals.h> and
# similar SDK headers unless told explicitly where to look.
set -euo pipefail

BUILD_DIR="${1:-build}"

if [[ ! -f "$BUILD_DIR/compile_commands.json" ]]; then
    echo "error: $BUILD_DIR/compile_commands.json not found — configure with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON first" >&2
    exit 1
fi

EXTRA_ARGS=()
if [[ "$(uname -s)" == "Darwin" ]]; then
    EXTRA_ARGS+=(--extra-arg=-isysroot --extra-arg="$(xcrun --show-sdk-path)")
fi

clang-tidy -p "$BUILD_DIR" "${EXTRA_ARGS[@]}" src/Main.cpp
