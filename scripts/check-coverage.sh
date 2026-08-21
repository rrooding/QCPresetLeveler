#!/usr/bin/env bash
# Measures coverage over non-UI, non-audio-engine source (everything under src/ except
# src/ui/ and src/audio/) and fails if it's below the threshold. src/ui/ (JUCE Component
# painting/layout code) and src/audio/ (AudioIODeviceCallback glue) are excluded per
# CODING_STANDARDS.md — neither can be meaningfully unit-tested without real hardware.
#
# Requires a build configured with -DQCPL_ENABLE_COVERAGE=ON and already run through
# `ctest` at least once, so .gcda files exist alongside the object files.
set -euo pipefail

BUILD_DIR="${1:-build}"
THRESHOLD="${QCPL_COVERAGE_THRESHOLD:-85}"

if ! command -v gcovr &>/dev/null; then
    echo "error: gcovr not found (pip install gcovr)" >&2
    exit 1
fi

GCOVR_ARGS=(
    --root .
    --object-directory "$BUILD_DIR"
    --filter 'src/.*'
    --exclude 'src/ui/.*'
    --exclude 'src/audio/.*'
    --exclude 'tests/.*'
    --exclude '.*_deps/.*'
)

# Determine whether any in-scope src/ file has coverage data at all *after* filtering,
# rather than just checking for the presence of any .gcda anywhere (tests/SanityTests.cpp
# itself always produces one, which would otherwise mask a genuinely empty scope).
TOTAL_LINES=$(gcovr "${GCOVR_ARGS[@]}" --json-summary -o - 2>/dev/null \
    | python3 -c "import json,sys; print(json.load(sys.stdin)['line_total'])")

if [[ "$TOTAL_LINES" == "0" ]]; then
    echo "No in-scope code under src/ (outside src/ui/ and src/audio/) has coverage data yet — nothing to gate on."
    echo "PASS (vacuous — no non-UI production code exists yet)"
    exit 0
fi

gcovr "${GCOVR_ARGS[@]}" --print-summary --fail-under-line "$THRESHOLD"
