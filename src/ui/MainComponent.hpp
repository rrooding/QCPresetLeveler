#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "ChannelLevelMonitor.hpp"
#include "ChannelPairSelectorPanel.hpp"
#include "MidiPortSelectorPanel.hpp"
#include "PresetChangeTriggerPanel.hpp"
#include "PresetColumnContainer.hpp"

namespace leveler {

class MainComponent final : public juce::Component {
public:
    MainComponent(juce::AudioDeviceManager& deviceManager, MidiPortManager& midiPortManager,
                  AudioEngine& audioEngine)
        : deviceSelector_(deviceManager, 0, 8, 0, 8, false, false, true, true), midiPanel_(midiPortManager),
          presetChangePanel_(midiPortManager), levelMonitor_(audioEngine, channelPairSelector_),
          presetColumns_(midiPortManager, levelMonitor_) {
        addAndMakeVisible(deviceSelector_);
        addAndMakeVisible(midiPanel_);
        addAndMakeVisible(presetChangePanel_);
        addAndMakeVisible(channelPairSelector_);
        addAndMakeVisible(presetColumns_);
        setSize(1280, 1180);
    }

    void resized() override {
        auto area = getLocalBounds();
        channelPairSelector_.setBounds(area.removeFromBottom(40));
        presetChangePanel_.setBounds(area.removeFromBottom(40));
        midiPanel_.setBounds(area.removeFromBottom(72));

        presetColumns_.setBounds(area.removeFromBottom(520));

        deviceSelector_.setBounds(area);
    }

private:
    juce::AudioDeviceSelectorComponent deviceSelector_;
    MidiPortSelectorPanel midiPanel_;
    PresetChangeTriggerPanel presetChangePanel_;
    ChannelPairSelectorPanel channelPairSelector_;
    ChannelLevelMonitor levelMonitor_;
    PresetColumnContainer presetColumns_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace leveler
