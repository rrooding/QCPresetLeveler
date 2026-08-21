#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "MidiPortSelectorPanel.hpp"
#include "PresetChangeTriggerPanel.hpp"

namespace leveler {

class MainComponent final : public juce::Component {
public:
    MainComponent(juce::AudioDeviceManager& deviceManager, MidiPortManager& midiPortManager)
        : deviceSelector_(deviceManager, 0, 8, 0, 8, false, false, true, true), midiPanel_(midiPortManager),
          presetChangePanel_(midiPortManager) {
        addAndMakeVisible(deviceSelector_);
        addAndMakeVisible(midiPanel_);
        addAndMakeVisible(presetChangePanel_);
        setSize(900, 640);
    }

    void resized() override {
        auto area = getLocalBounds();
        presetChangePanel_.setBounds(area.removeFromBottom(40));
        midiPanel_.setBounds(area.removeFromBottom(72));
        deviceSelector_.setBounds(area);
    }

private:
    juce::AudioDeviceSelectorComponent deviceSelector_;
    MidiPortSelectorPanel midiPanel_;
    PresetChangeTriggerPanel presetChangePanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace leveler
