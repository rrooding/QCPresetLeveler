#pragma once

#include <juce_audio_devices/juce_audio_devices.h>

namespace leveler {

class MidiPortManager final : private juce::MidiInputCallback {
public:
    MidiPortManager() = default;

    void setOutput(const juce::MidiDeviceInfo& info) {
        midiOutput_ = juce::MidiOutput::openDevice(info.identifier);
    }

    void setInput(const juce::MidiDeviceInfo& info) {
        midiInput_ = juce::MidiInput::openDevice(info.identifier, this);
        if (midiInput_ != nullptr)
            midiInput_->start();
    }

    [[nodiscard]] juce::MidiOutput* getOutput() const { return midiOutput_.get(); }
    [[nodiscard]] juce::MidiInput* getInput() const { return midiInput_.get(); }

private:
    void handleIncomingMidiMessage(juce::MidiInput*, const juce::MidiMessage&) override {}

    std::unique_ptr<juce::MidiOutput> midiOutput_;
    std::unique_ptr<juce::MidiInput> midiInput_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPortManager)
};

} // namespace leveler
