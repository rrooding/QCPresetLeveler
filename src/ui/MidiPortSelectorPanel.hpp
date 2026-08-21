#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <utility>

#include "../DeviceNameMatch.hpp"
#include "MidiPortManager.hpp"

namespace leveler {

// Auto-selects a port matching "Quad Cortex" by name, same as the audio device panel — this
// covers the common case of a direct USB connection, where the QC's MIDI ports carry its
// name. It's still just a best-effort: if the QC is reached through a Bluetooth MIDI bridge
// (e.g. a WIDI Jack) instead, the port name reflects the bridge product, no match is found,
// and the user picks manually.
class MidiPortSelectorPanel final : public juce::Component {
public:
    explicit MidiPortSelectorPanel(MidiPortManager& midiPortManager) : midiPortManager_(midiPortManager) {
        setUpComboBox(inputLabel_, inputBox_, "Input");
        setUpComboBox(outputLabel_, outputBox_, "Output");

        addAndMakeVisible(inputLabel_);
        addAndMakeVisible(inputBox_);
        addAndMakeVisible(outputLabel_);
        addAndMakeVisible(outputBox_);

        populate(inputBox_, inputDevices_, juce::MidiInput::getAvailableDevices());
        populate(outputBox_, outputDevices_, juce::MidiOutput::getAvailableDevices());

        selectPreferredDevice(inputBox_, inputDevices_,
                              [this](auto& info) { midiPortManager_.setInput(info); });
        selectPreferredDevice(outputBox_, outputDevices_,
                              [this](auto& info) { midiPortManager_.setOutput(info); });

        inputBox_.onChange = [this] { selectInput(); };
        outputBox_.onChange = [this] { selectOutput(); };
    }

    void resized() override {
        auto area = getLocalBounds().reduced(8);

        auto outputRow = area.removeFromTop(24);
        outputLabel_.setBounds(outputRow.removeFromLeft(80));
        outputBox_.setBounds(outputRow);

        area.removeFromTop(8);

        auto inputRow = area.removeFromTop(24);
        inputLabel_.setBounds(inputRow.removeFromLeft(80));
        inputBox_.setBounds(inputRow);
    }

private:
    static void setUpComboBox(juce::Label& label, juce::ComboBox& box, const juce::String& labelText) {
        label.setText(labelText, juce::dontSendNotification);
        box.setTextWhenNothingSelected("(none)");
    }

    static void populate(juce::ComboBox& box, std::vector<juce::MidiDeviceInfo>& devices,
                         const juce::Array<juce::MidiDeviceInfo>& available) {
        box.clear();
        devices.clear();

        int itemId = 1;
        for (auto& device : available) {
            box.addItem(device.name, itemId);
            devices.push_back(device);
            ++itemId;
        }
    }

    template <typename SetterFn>
    static void selectPreferredDevice(juce::ComboBox& box, const std::vector<juce::MidiDeviceInfo>& devices,
                                      SetterFn&& setter) {
        for (size_t i = 0; i < devices.size(); ++i) {
            if (looksLikeQuadCortex(devices[i].name)) {
                box.setSelectedId((int)i + 1, juce::dontSendNotification);
                std::forward<SetterFn>(setter)(devices[i]);
                return;
            }
        }
    }

    void selectInput() {
        auto index = inputBox_.getSelectedItemIndex();
        if (index >= 0 && (size_t)index < inputDevices_.size())
            midiPortManager_.setInput(inputDevices_[(size_t)index]);
    }

    void selectOutput() {
        auto index = outputBox_.getSelectedItemIndex();
        if (index >= 0 && (size_t)index < outputDevices_.size())
            midiPortManager_.setOutput(outputDevices_[(size_t)index]);
    }

    MidiPortManager& midiPortManager_;

    juce::Label inputLabel_;
    juce::ComboBox inputBox_;
    std::vector<juce::MidiDeviceInfo> inputDevices_;

    juce::Label outputLabel_;
    juce::ComboBox outputBox_;
    std::vector<juce::MidiDeviceInfo> outputDevices_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPortSelectorPanel)
};

} // namespace leveler
