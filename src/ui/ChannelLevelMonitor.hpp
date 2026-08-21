#pragma once

#include <juce_events/juce_events.h>

#include <array>
#include <cmath>
#include <optional>

#include "../ChannelPair.hpp"
#include "../LevelMeter.hpp"
#include "../audio/AudioEngine.hpp"
#include "ChannelPairSelectorPanel.hpp"

namespace leveler {

// Owns the single reader of AudioEngine's capture ring buffer. CaptureRingBuffer is a
// single-consumer FIFO (read() drains what it returns), so two independent components each
// calling read() would steal frames from one another rather than both seeing the same audio —
// this class exists so there's exactly one drainer, feeding a LevelMeter for the currently
// selected channel pair on a UI timer. Other UI components (the preset column's VU meter, etc.)
// read the resulting dB values from here instead of draining the buffer themselves.
class ChannelLevelMonitor final : private juce::Timer {
public:
    ChannelLevelMonitor(AudioEngine& audioEngine, const ChannelPairSelectorPanel& channelSelector)
        : audioEngine_(audioEngine), channelSelector_(channelSelector) {
        startTimerHz(30);
    }

    ~ChannelLevelMonitor() override { stopTimer(); }

    [[nodiscard]] bool isReady() const { return levelMeter_.has_value(); }
    [[nodiscard]] float getPeakDb() const { return levelMeter_ ? levelMeter_->getPeakDb() : -100.0f; }
    [[nodiscard]] float getRmsDb() const { return levelMeter_ ? levelMeter_->getRmsDb() : -100.0f; }

private:
    void timerCallback() override {
        static constexpr size_t scratchCapacity = 4096;
        std::array<AudioFrame, scratchCapacity> scratch{};
        const auto framesRead = audioEngine_.getCaptureBuffer().read(scratch.data(), (int)scratch.size());

        const auto sampleRate = audioEngine_.getSampleRate();
        if (sampleRate <= 0.0) {
            levelMeter_.reset();
            return;
        }

        if (!levelMeter_.has_value() || std::abs(lastSampleRate_ - sampleRate) > 0.5) {
            levelMeter_.emplace(sampleRate);
            lastSampleRate_ = sampleRate;
        }

        const auto pair = channelPairForIndex(channelSelector_.getSelectedPairIndex());
        for (int i = 0; i < framesRead; ++i)
            levelMeter_->processSample(combineStereoSample(scratch[(size_t)i], pair));
    }

    AudioEngine& audioEngine_;
    const ChannelPairSelectorPanel& channelSelector_;
    std::optional<LevelMeter> levelMeter_;
    double lastSampleRate_ = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelLevelMonitor)
};

} // namespace leveler
