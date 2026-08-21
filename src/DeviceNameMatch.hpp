#pragma once

#include <juce_core/juce_core.h>

namespace leveler {

inline bool looksLikeQuadCortex(const juce::String& deviceName) {
    return deviceName.containsIgnoreCase("Quad Cortex");
}

} // namespace leveler
