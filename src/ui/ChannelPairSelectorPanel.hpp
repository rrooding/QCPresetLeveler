#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../ChannelPair.hpp"

namespace leveler {

// Lets the user choose which of the Quad Cortex's USB return channel pairs to monitor (e.g.
// processed/wet output vs. FX return vs. reamp) — the QC exposes multiple stereo pairs over
// its 8-in USB interface, and which one carries the signal worth leveling depends on the
// user's own routing, not something this app can infer.
class ChannelPairSelectorPanel final : public juce::Component {
public:
    ChannelPairSelectorPanel() {
        label_.setText("Monitor", juce::dontSendNotification);
        addAndMakeVisible(label_);

        for (int pairIndex = 0; pairIndex < numChannelPairs; ++pairIndex) {
            const auto pair = channelPairForIndex(pairIndex);
            box_.addItem("Channels " + juce::String(pair.leftIndex + 1) + "-" +
                             juce::String(pair.rightIndex + 1),
                         pairIndex + 1);
        }
        box_.setSelectedId(1, juce::dontSendNotification);
        addAndMakeVisible(box_);
    }

    void resized() override {
        auto area = getLocalBounds().reduced(8);
        label_.setBounds(area.removeFromLeft(80));
        box_.setBounds(area);
    }

    [[nodiscard]] int getSelectedPairIndex() const {
        return juce::jlimit(0, numChannelPairs - 1, box_.getSelectedItemIndex());
    }

private:
    juce::Label label_;
    juce::ComboBox box_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelPairSelectorPanel)
};

} // namespace leveler
