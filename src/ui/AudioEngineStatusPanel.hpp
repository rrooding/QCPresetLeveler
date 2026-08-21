#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <array>
#include <cmath>
#include <optional>

#include "../ChannelPair.hpp"
#include "../LevelMeter.hpp"
#include "../audio/AudioEngine.hpp"
#include "ChannelPairSelectorPanel.hpp"

namespace leveler {

// Diagnostic display showing live peak/RMS dBFS from the capture buffer's user-selected
// channel pair, so #15's duplex engine, #16's level measurement, and #17's channel selection
// can be verified against real hardware ahead of #18 (a proper VU meter component) replacing
// this.
class AudioEngineStatusPanel final : public juce::Component, private juce::Timer {
public:
    AudioEngineStatusPanel(AudioEngine& audioEngine, const ChannelPairSelectorPanel& channelSelector)
        : audioEngine_(audioEngine), channelSelector_(channelSelector) {
        addAndMakeVisible(statusLabel_);
        startTimerHz(10);
    }

    ~AudioEngineStatusPanel() override { stopTimer(); }

    void resized() override { statusLabel_.setBounds(getLocalBounds()); }

private:
    void timerCallback() override {
        static constexpr size_t scratchCapacity = 4096;
        std::array<AudioFrame, scratchCapacity> scratch{};
        const auto framesRead = audioEngine_.getCaptureBuffer().read(scratch.data(), (int)scratch.size());

        const auto sampleRate = audioEngine_.getSampleRate();
        if (sampleRate <= 0.0) {
            statusLabel_.setText("Waiting for audio device...", juce::dontSendNotification);
            return;
        }

        if (!levelMeter_.has_value() || std::abs(lastSampleRate_ - sampleRate) > 0.5) {
            levelMeter_.emplace(sampleRate);
            lastSampleRate_ = sampleRate;
        }

        const auto pair = channelPairForIndex(channelSelector_.getSelectedPairIndex());
        for (int i = 0; i < framesRead; ++i)
            levelMeter_->processSample(combineStereoSample(scratch[(size_t)i], pair));

        statusLabel_.setText(juce::String::formatted("Peak: %.1f dBFS   RMS: %.1f dBFS",
                                                     levelMeter_->getPeakDb(), levelMeter_->getRmsDb()),
                             juce::dontSendNotification);
    }

    AudioEngine& audioEngine_;
    const ChannelPairSelectorPanel& channelSelector_;
    juce::Label statusLabel_;
    std::optional<LevelMeter> levelMeter_;
    double lastSampleRate_ = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngineStatusPanel)
};

} // namespace leveler
