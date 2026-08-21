#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <array>
#include <cmath>
#include <optional>

#include "../LevelMeter.hpp"
#include "../audio/AudioEngine.hpp"

namespace leveler {

// Diagnostic display showing live peak/RMS dBFS from the capture buffer's first channel, so
// #15's duplex engine and #16's level measurement can be verified against real hardware ahead
// of #17 (channel selection) and #18 (a proper VU meter component) replacing this.
class AudioEngineStatusPanel final : public juce::Component, private juce::Timer {
public:
    explicit AudioEngineStatusPanel(AudioEngine& audioEngine) : audioEngine_(audioEngine) {
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

        for (int i = 0; i < framesRead; ++i)
            levelMeter_->processSample(scratch[(size_t)i][0]);

        statusLabel_.setText(juce::String::formatted("Peak: %.1f dBFS   RMS: %.1f dBFS",
                                                     levelMeter_->getPeakDb(), levelMeter_->getRmsDb()),
                             juce::dontSendNotification);
    }

    AudioEngine& audioEngine_;
    juce::Label statusLabel_;
    std::optional<LevelMeter> levelMeter_;
    double lastSampleRate_ = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngineStatusPanel)
};

} // namespace leveler
