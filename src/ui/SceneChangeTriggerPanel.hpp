#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <array>

#include "MidiPortManager.hpp"

namespace leveler {

// Minimal manual trigger for sending a scene change, so #10 can be validated against real
// hardware before #11 exists to drive it from arrow keys.
class SceneChangeTriggerPanel final : public juce::Component {
public:
    explicit SceneChangeTriggerPanel(MidiPortManager& midiPortManager) : midiPortManager_(midiPortManager) {
        for (size_t i = 0; i < sceneButtons_.size(); ++i) {
            auto& button = sceneButtons_.at(i);
            const int sceneIndex = (int)i;
            button.setButtonText(juce::String::charToString((juce::juce_wchar)('A' + sceneIndex)));
            button.onClick = [this, sceneIndex] { midiPortManager_.sendSceneChange(sceneIndex); };
            addAndMakeVisible(button);
        }
    }

    void resized() override {
        auto area = getLocalBounds().reduced(8);
        const auto buttonWidth = area.getWidth() / (int)sceneButtons_.size();

        for (auto& button : sceneButtons_)
            button.setBounds(area.removeFromLeft(buttonWidth).reduced(2));
    }

private:
    MidiPortManager& midiPortManager_;
    std::array<juce::TextButton, 8> sceneButtons_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SceneChangeTriggerPanel)
};

} // namespace leveler
