#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <array>
#include <cmath>
#include <optional>

#include "../ChannelPair.hpp"
#include "../LevelMeter.hpp"
#include "../MeterScale.hpp"
#include "../audio/AudioEngine.hpp"
#include "ChannelPairSelectorPanel.hpp"

namespace leveler {

// Live output meter (per the original mockup): a horizontal bar filled to the current RMS
// level, with a thin cap line marking the current peak, plus the numeric dBFS readout that
// made #16/#17's hardware verification legible. Drains the capture buffer and feeds the
// user-selected channel pair through its own LevelMeter on a UI timer, the same way
// AudioEngineStatusPanel (now retired) did.
class VuMeterComponent final : public juce::Component, private juce::Timer {
public:
    VuMeterComponent(AudioEngine& audioEngine, const ChannelPairSelectorPanel& channelSelector)
        : audioEngine_(audioEngine), channelSelector_(channelSelector) {
        startTimerHz(30);
    }

    ~VuMeterComponent() override { stopTimer(); }

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colours::black);
        g.fillRect(bounds);

        if (!levelMeter_.has_value()) {
            g.setColour(juce::Colours::white);
            g.drawText("Waiting for audio device...", bounds, juce::Justification::centred);
            return;
        }

        const auto peakDb = levelMeter_->getPeakDb();
        const auto rmsDb = levelMeter_->getRmsDb();

        g.setColour(juce::Colours::green);
        g.fillRect(bounds.withWidth(bounds.getWidth() * dbToMeterFraction(rmsDb)));

        const auto peakX = bounds.getX() + bounds.getWidth() * dbToMeterFraction(peakDb);
        g.setColour(juce::Colours::white);
        g.drawLine(peakX, bounds.getY(), peakX, bounds.getBottom(), 2.0f);

        g.setColour(juce::Colours::white);
        g.drawText(juce::String::formatted("Peak: %.1f dBFS   RMS: %.1f dBFS", peakDb, rmsDb), bounds,
                   juce::Justification::centred);
    }

private:
    void timerCallback() override {
        static constexpr size_t scratchCapacity = 4096;
        std::array<AudioFrame, scratchCapacity> scratch{};
        const auto framesRead = audioEngine_.getCaptureBuffer().read(scratch.data(), (int)scratch.size());

        const auto sampleRate = audioEngine_.getSampleRate();
        if (sampleRate <= 0.0) {
            levelMeter_.reset();
            repaint();
            return;
        }

        if (!levelMeter_.has_value() || std::abs(lastSampleRate_ - sampleRate) > 0.5) {
            levelMeter_.emplace(sampleRate);
            lastSampleRate_ = sampleRate;
        }

        const auto pair = channelPairForIndex(channelSelector_.getSelectedPairIndex());
        for (int i = 0; i < framesRead; ++i)
            levelMeter_->processSample(combineStereoSample(scratch[(size_t)i], pair));

        repaint();
    }

    AudioEngine& audioEngine_;
    const ChannelPairSelectorPanel& channelSelector_;
    std::optional<LevelMeter> levelMeter_;
    double lastSampleRate_ = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VuMeterComponent)
};

} // namespace leveler
