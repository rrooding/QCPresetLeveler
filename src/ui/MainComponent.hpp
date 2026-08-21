#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "ChannelLevelMonitor.hpp"
#include "ChannelPairSelectorPanel.hpp"
#include "MidiPortSelectorPanel.hpp"
#include "PresetChangeTriggerPanel.hpp"
#include "PresetColumnComponent.hpp"

namespace leveler {

class MainComponent final : public juce::Component {
public:
    MainComponent(juce::AudioDeviceManager& deviceManager, MidiPortManager& midiPortManager,
                  AudioEngine& audioEngine)
        : deviceSelector_(deviceManager, 0, 8, 0, 8, false, false, true, true), midiPanel_(midiPortManager),
          presetChangePanel_(midiPortManager), levelMonitor_(audioEngine, channelPairSelector_),
          presetColumn_(1, midiPortManager, levelMonitor_) {
        addAndMakeVisible(deviceSelector_);
        addAndMakeVisible(midiPanel_);
        addAndMakeVisible(presetChangePanel_);
        addAndMakeVisible(channelPairSelector_);
        addAndMakeVisible(presetColumn_);
        setSize(900, 1180);
    }

    void resized() override {
        auto area = getLocalBounds();
        channelPairSelector_.setBounds(area.removeFromBottom(40));
        presetChangePanel_.setBounds(area.removeFromBottom(40));
        midiPanel_.setBounds(area.removeFromBottom(72));

        auto columnArea = area.removeFromBottom(520).removeFromLeft(280);
        presetColumn_.setBounds(columnArea);

        deviceSelector_.setBounds(area);
    }

private:
    juce::AudioDeviceSelectorComponent deviceSelector_;
    MidiPortSelectorPanel midiPanel_;
    PresetChangeTriggerPanel presetChangePanel_;
    ChannelPairSelectorPanel channelPairSelector_;
    ChannelLevelMonitor levelMonitor_;
    PresetColumnComponent presetColumn_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace leveler
