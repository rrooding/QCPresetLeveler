#pragma once

#include <cmath>
#include <cstdint>

namespace leveler {

enum class LevelingDirection : std::uint8_t { onTarget, raise, lower };

struct LevelingInstruction {
    LevelingDirection direction;
    float adjustmentDb; // magnitude; always >= 0, and 0 when direction is onTarget
};

// A small deadband around the target so the instruction doesn't flicker between "raise" and
// "lower" as the measured level's ballistics settle by fractions of a dB.
inline constexpr float levelingDeadbandDb = 0.5f;

// What the user should do to the QC's hardware Level control to bring the measured level to
// target, since it can't be set remotely over MIDI (see #20).
inline LevelingInstruction computeLevelingInstruction(float measuredDb, float targetDb) {
    const float delta = targetDb - measuredDb;
    if (std::abs(delta) <= levelingDeadbandDb)
        return {.direction = LevelingDirection::onTarget, .adjustmentDb = 0.0f};
    if (delta > 0.0f)
        return {.direction = LevelingDirection::raise, .adjustmentDb = delta};
    return {.direction = LevelingDirection::lower, .adjustmentDb = -delta};
}

} // namespace leveler
