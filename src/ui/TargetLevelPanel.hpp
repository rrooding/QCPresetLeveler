#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../TargetLevel.hpp"

namespace leveler {

// One shared target level for the whole session (matching how leveling actually works — every
// preset gets levelled to the same reference), editable here and read by every preset column's
// raise/lower instruction.
class TargetLevelPanel final : public juce::Component {
public:
    explicit TargetLevelPanel(TargetLevel& targetLevel) : targetLevel_(targetLevel) {
        label_.setText("Target Level", juce::dontSendNotification);
        addAndMakeVisible(label_);

        slider_.setSliderStyle(juce::Slider::IncDecButtons);
        slider_.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 70, 24);
        slider_.setRange(-40.0, 0.0, 0.1);
        slider_.setValue(targetLevel_.getTargetDb(), juce::dontSendNotification);
        slider_.setTextValueSuffix(" dBFS");
        slider_.onValueChange = [this] { targetLevel_.setTargetDb((float)slider_.getValue()); };
        addAndMakeVisible(slider_);
    }

    void resized() override {
        auto area = getLocalBounds().reduced(8);
        label_.setBounds(area.removeFromLeft(90));
        slider_.setBounds(area.removeFromLeft(180));
    }

private:
    TargetLevel& targetLevel_;

    juce::Label label_;
    juce::Slider slider_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TargetLevelPanel)
};

} // namespace leveler
