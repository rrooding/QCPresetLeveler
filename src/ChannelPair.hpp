#pragma once

#include "CaptureRingBuffer.hpp"

#include <algorithm>
#include <cmath>

namespace leveler {

inline constexpr int numChannelPairs = maxCaptureChannels / 2;

struct ChannelPair {
    int leftIndex;
    int rightIndex;
};

inline ChannelPair channelPairForIndex(int pairIndex) {
    return ChannelPair{.leftIndex = pairIndex * 2, .rightIndex = pairIndex * 2 + 1};
}

// The single representative sample LevelMeter expects from a stereo pair: the louder of the
// two channels, so content panned or present on only one side isn't underreported.
inline float combineStereoSample(const AudioFrame& frame, ChannelPair pair) {
    return std::max(std::abs(frame[(size_t)pair.leftIndex]), std::abs(frame[(size_t)pair.rightIndex]));
}

} // namespace leveler
