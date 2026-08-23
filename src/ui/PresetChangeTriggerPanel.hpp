#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../SetlistSelection.hpp"
#include "MidiPortManager.hpp"

namespace leveler {

// Minimal manual trigger for sending a Program Change, so #9 can be validated against real
// hardware before #11 exists to drive it from arrow keys / a proper preset list.
class PresetChangeTriggerPanel final : public juce::Component {
public:
    PresetChangeTriggerPanel(MidiPortManager& midiPortManager, const SetlistSelection& setlistSelection)
        : midiPortManager_(midiPortManager), setlistSelection_(setlistSelection) {
        presetLabel_.setText("Preset #", juce::dontSendNotification);
        addAndMakeVisible(presetLabel_);

        presetNumberSlider_.setSliderStyle(juce::Slider::IncDecButtons);
        presetNumberSlider_.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 24);
        presetNumberSlider_.setRange(1, 256, 1);
        presetNumberSlider_.setValue(1);
        addAndMakeVisible(presetNumberSlider_);

        goButton_.setButtonText("Go");
        goButton_.onClick = [this] {
            const auto presetNumber = (int)presetNumberSlider_.getValue() - 1;
            midiPortManager_.sendProgramChange(presetNumber, MidiChannel{1}, setlistSelection_.getSetlist());
        };
        addAndMakeVisible(goButton_);
    }

    void resized() override {
        auto area = getLocalBounds().reduced(8);
        presetLabel_.setBounds(area.removeFromLeft(60));
        presetNumberSlider_.setBounds(area.removeFromLeft(140));
        area.removeFromLeft(8);
        goButton_.setBounds(area.removeFromLeft(60));
    }

private:
    MidiPortManager& midiPortManager_;
    const SetlistSelection& setlistSelection_;

    juce::Label presetLabel_;
    juce::Slider presetNumberSlider_;
    juce::TextButton goButton_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetChangeTriggerPanel)
};

} // namespace leveler
