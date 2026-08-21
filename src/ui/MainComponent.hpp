#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "ChannelPairSelectorPanel.hpp"
#include "MidiPortSelectorPanel.hpp"
#include "PresetChangeTriggerPanel.hpp"
#include "SceneChangeTriggerPanel.hpp"
#include "VuMeterComponent.hpp"

namespace leveler {

class MainComponent final : public juce::Component {
public:
    MainComponent(juce::AudioDeviceManager& deviceManager, MidiPortManager& midiPortManager,
                  AudioEngine& audioEngine)
        : deviceSelector_(deviceManager, 0, 8, 0, 8, false, false, true, true), midiPanel_(midiPortManager),
          presetChangePanel_(midiPortManager), sceneChangePanel_(midiPortManager),
          vuMeter_(audioEngine, channelPairSelector_) {
        addAndMakeVisible(deviceSelector_);
        addAndMakeVisible(midiPanel_);
        addAndMakeVisible(presetChangePanel_);
        addAndMakeVisible(sceneChangePanel_);
        addAndMakeVisible(channelPairSelector_);
        addAndMakeVisible(vuMeter_);
        setSize(900, 760);
    }

    void resized() override {
        auto area = getLocalBounds();
        vuMeter_.setBounds(area.removeFromBottom(32));
        channelPairSelector_.setBounds(area.removeFromBottom(40));
        sceneChangePanel_.setBounds(area.removeFromBottom(40));
        presetChangePanel_.setBounds(area.removeFromBottom(40));
        midiPanel_.setBounds(area.removeFromBottom(72));
        deviceSelector_.setBounds(area);
    }

private:
    juce::AudioDeviceSelectorComponent deviceSelector_;
    MidiPortSelectorPanel midiPanel_;
    PresetChangeTriggerPanel presetChangePanel_;
    SceneChangeTriggerPanel sceneChangePanel_;
    ChannelPairSelectorPanel channelPairSelector_;
    VuMeterComponent vuMeter_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace leveler
