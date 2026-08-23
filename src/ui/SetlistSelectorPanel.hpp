#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../SetlistSelection.hpp"

namespace leveler {

// Lets the user pick which QC setlist/collection (sent as CC#32) preset navigation targets.
// Shared globally, matching TargetLevelPanel — one setlist per leveling session. There's no
// default that matches the device's actual active setlist: no documented way to query that
// over MIDI (see #48), so the user must always pick explicitly rather than relying on
// auto-detection.
class SetlistSelectorPanel final : public juce::Component {
public:
    explicit SetlistSelectorPanel(SetlistSelection& setlistSelection) : setlistSelection_(setlistSelection) {
        label_.setText("Setlist", juce::dontSendNotification);
        addAndMakeVisible(label_);

        slider_.setSliderStyle(juce::Slider::IncDecButtons);
        slider_.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 24);
        slider_.setRange(0.0, 127.0, 1.0);
        slider_.setValue(setlistSelection_.getSetlist().value, juce::dontSendNotification);
        slider_.onValueChange = [this] { setlistSelection_.setSetlist(SetlistId{(int)slider_.getValue()}); };
        addAndMakeVisible(slider_);
    }

    void resized() override {
        auto area = getLocalBounds().reduced(8);
        label_.setBounds(area.removeFromLeft(60));
        slider_.setBounds(area.removeFromLeft(140));
    }

private:
    SetlistSelection& setlistSelection_;

    juce::Label label_;
    juce::Slider slider_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SetlistSelectorPanel)
};

} // namespace leveler
