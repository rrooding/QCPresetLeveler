#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace leveler {

class MainComponent final : public juce::Component {
public:
    MainComponent() { setSize(900, 600); }

    void paint(juce::Graphics& g) override {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        g.setColour(juce::Colours::white);
        g.setFont(24.0f);
        g.drawFittedText("QC Preset Leveler", getLocalBounds(), juce::Justification::centred, 1);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace leveler
