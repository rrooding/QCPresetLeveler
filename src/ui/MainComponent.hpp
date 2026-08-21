#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

namespace leveler {

class MainComponent final : public juce::Component {
public:
    explicit MainComponent(juce::AudioDeviceManager& deviceManager)
        : deviceSelector_(deviceManager, 0, 8, 0, 8, false, false, true, true) {
        addAndMakeVisible(deviceSelector_);
        setSize(900, 600);
    }

    void resized() override { deviceSelector_.setBounds(getLocalBounds()); }

private:
    juce::AudioDeviceSelectorComponent deviceSelector_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace leveler
