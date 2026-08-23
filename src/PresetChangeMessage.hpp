#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include <vector>

#include "MidiChannel.hpp"
#include "SetlistId.hpp"

namespace leveler {

// presetNumber is 0-based across the QC's full 0-255 preset range. The QC splits that range
// into two 128-preset groups selected via Bank Select MSB (CC#0: 0 or 1). Bank Select LSB
// (CC#32) turned out not to be a spec-compliant no-op on the QC: it's the setlist/collection
// select (confirmed against real hardware — see #48), so which setlist presetNumber resolves
// against depends on it. Program Change then picks the preset within the bank.
inline std::vector<juce::MidiMessage> presetChangeMessages(int presetNumber, MidiChannel channel,
                                                           SetlistId setlist = SetlistId{0}) {
    const int bankMsb = (presetNumber >= 128) ? 1 : 0;
    const int programNumber = presetNumber % 128;

    return {
        juce::MidiMessage::controllerEvent(channel.value, 0, bankMsb),
        juce::MidiMessage::controllerEvent(channel.value, 32, setlist.value),
        juce::MidiMessage::programChange(channel.value, programNumber),
    };
}

} // namespace leveler
