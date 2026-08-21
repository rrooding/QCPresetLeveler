#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <array>

#include "../audio/AudioEngine.hpp"

namespace leveler {

// Minimal diagnostic display confirming the duplex engine is actually capturing, so #15 can
// be verified against real hardware ahead of #16/#18 turning this into a real level meter.
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
        statusLabel_.setText("Captured " + juce::String(framesRead) + " frames this tick",
                             juce::dontSendNotification);
    }

    AudioEngine& audioEngine_;
    juce::Label statusLabel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngineStatusPanel)
};

} // namespace leveler
