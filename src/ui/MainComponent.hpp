#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "../TargetLevel.hpp"
#include "ChannelLevelMonitor.hpp"
#include "ChannelPairSelectorPanel.hpp"
#include "MidiPortSelectorPanel.hpp"
#include "PresetChangeTriggerPanel.hpp"
#include "PresetColumnContainer.hpp"
#include "TargetLevelPanel.hpp"

namespace leveler {

class MainComponent final : public juce::Component {
public:
    MainComponent(juce::AudioDeviceManager& deviceManager, MidiPortManager& midiPortManager,
                  AudioEngine& audioEngine)
        : deviceSelector_(deviceManager, 0, 8, 0, 8, false, false, true, true), midiPanel_(midiPortManager),
          presetChangePanel_(midiPortManager), levelMonitor_(audioEngine, channelPairSelector_),
          targetLevelPanel_(targetLevel_), presetColumns_(midiPortManager, levelMonitor_, targetLevel_) {
        addAndMakeVisible(deviceSelector_);
        addAndMakeVisible(midiPanel_);
        addAndMakeVisible(presetChangePanel_);
        addAndMakeVisible(channelPairSelector_);
        addAndMakeVisible(targetLevelPanel_);
        addAndMakeVisible(presetColumns_);
        setSize(1280, 1220);
    }

    void resized() override {
        auto area = getLocalBounds();
        channelPairSelector_.setBounds(area.removeFromBottom(40));
        targetLevelPanel_.setBounds(area.removeFromBottom(40));
        presetChangePanel_.setBounds(area.removeFromBottom(40));
        midiPanel_.setBounds(area.removeFromBottom(72));

        presetColumns_.setBounds(area.removeFromBottom(560));

        deviceSelector_.setBounds(area);
    }

private:
    juce::AudioDeviceSelectorComponent deviceSelector_;
    MidiPortSelectorPanel midiPanel_;
    PresetChangeTriggerPanel presetChangePanel_;
    ChannelPairSelectorPanel channelPairSelector_;
    ChannelLevelMonitor levelMonitor_;
    TargetLevel targetLevel_;
    TargetLevelPanel targetLevelPanel_;
    PresetColumnContainer presetColumns_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace leveler
