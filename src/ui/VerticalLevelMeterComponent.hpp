#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <array>

#include "../MeterScale.hpp"
#include "ChannelLevelMonitor.hpp"

namespace leveler {

// A vertical output meter matching the original mockup's style: RMS fills the bar from the
// bottom, a peak cap line marks the current peak, and green/yellow/red zones warn as the
// signal approaches full scale. Ticks stop at 0 dBFS (not the mockup's "+12") because a float
// sample meter has no headroom above digital full scale to show.
class VerticalLevelMeterComponent final : public juce::Component, private juce::Timer {
public:
    explicit VerticalLevelMeterComponent(const ChannelLevelMonitor& levelMonitor)
        : levelMonitor_(levelMonitor) {
        startTimerHz(30);
    }

    ~VerticalLevelMeterComponent() override { stopTimer(); }

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();

        g.setColour(juce::Colour(0xff141a20));
        g.fillRect(bounds);

        constexpr float tickInsetLeft = 26.0f;
        auto barBounds = bounds.withLeft(bounds.getX() + tickInsetLeft).reduced(4.0f);

        paintTicks(g, bounds, barBounds);
        paintZones(g, barBounds);

        if (!levelMonitor_.isReady())
            return;

        const auto rmsFraction = dbToMeterFraction(levelMonitor_.getRmsDb());
        auto fillBounds = barBounds.withTop(barBounds.getBottom() - barBounds.getHeight() * rmsFraction);
        g.setColour(juce::Colour(0xff2ecc71).withAlpha(0.85f));
        g.fillRect(fillBounds);

        const auto peakFraction = dbToMeterFraction(levelMonitor_.getPeakDb());
        const auto peakY = barBounds.getBottom() - barBounds.getHeight() * peakFraction;
        g.setColour(juce::Colours::white);
        g.drawLine(barBounds.getX(), peakY, barBounds.getRight(), peakY, 2.0f);
    }

private:
    static void paintTicks(juce::Graphics& g, juce::Rectangle<float> bounds,
                           juce::Rectangle<float> barBounds) {
        static constexpr std::array<float, 6> tickDbs{0.0f, -12.0f, -20.0f, -30.0f, -40.0f, -60.0f};

        g.setColour(juce::Colours::lightgrey);
        g.setFont(juce::Font(juce::FontOptions(10.0f)));

        for (const auto tickDb : tickDbs) {
            const auto fraction = dbToMeterFraction(tickDb);
            const auto y = barBounds.getBottom() - barBounds.getHeight() * fraction;
            auto textBounds = juce::Rectangle<float>(bounds.getX(), y - 6.0f,
                                                     barBounds.getX() - bounds.getX() - 2.0f, 12.0f);
            g.drawText(juce::String((int)tickDb), textBounds, juce::Justification::centredRight);
        }
    }

    static void paintZones(juce::Graphics& g, juce::Rectangle<float> barBounds) {
        const auto redTop = barBounds.getBottom() - barBounds.getHeight() * dbToMeterFraction(0.0f);
        const auto yellowTop = barBounds.getBottom() - barBounds.getHeight() * dbToMeterFraction(-3.0f);
        const auto greenTop = barBounds.getBottom() - barBounds.getHeight() * dbToMeterFraction(-12.0f);

        g.setColour(juce::Colour(0xff2a2f36));
        g.fillRect(barBounds);

        g.setColour(juce::Colours::darkred.withAlpha(0.3f));
        g.fillRect(barBounds.withTop(redTop).withBottom(yellowTop));

        g.setColour(juce::Colours::darkgoldenrod.withAlpha(0.3f));
        g.fillRect(barBounds.withTop(yellowTop).withBottom(greenTop));
    }

    void timerCallback() override { repaint(); }

    const ChannelLevelMonitor& levelMonitor_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VerticalLevelMeterComponent)
};

} // namespace leveler
