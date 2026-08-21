#pragma once

#include <algorithm>

namespace leveler {

inline constexpr float meterMinDb = -60.0f;
inline constexpr float meterMaxDb = 0.0f;

// Maps a dBFS value to a normalized [0, 1] fill fraction for meter rendering, clamped at both
// ends so a reading below the floor or above full-scale doesn't under/overshoot the widget.
inline float dbToMeterFraction(float db, float minDb = meterMinDb, float maxDb = meterMaxDb) {
    if (maxDb <= minDb)
        return 0.0f;
    return std::clamp((db - minDb) / (maxDb - minDb), 0.0f, 1.0f);
}

} // namespace leveler
