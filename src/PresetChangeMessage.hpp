#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include <vector>

namespace leveler {

// presetNumber is 0-based across the QC's full 0-255 preset range. The QC splits that range
// into two 128-preset groups selected via Bank Select MSB (CC#0: 0 or 1); Bank Select LSB
// (CC#32) isn't documented as meaningful for the QC but is sent as 0 to follow the standard
// MIDI two-part Bank Select convention. Program Change then picks the preset within the group.
// Two params, few call sites; revisit with a strong MidiChannel type if more MIDI-sending
// functions accumulate the same shape.
inline std::vector<juce::MidiMessage>
presetChangeMessages(int presetNumber, int midiChannel) { // NOLINT(bugprone-easily-swappable-parameters)
    const int bankMsb = (presetNumber >= 128) ? 1 : 0;
    const int programNumber = presetNumber % 128;

    return {
        juce::MidiMessage::controllerEvent(midiChannel, 0, bankMsb),
        juce::MidiMessage::controllerEvent(midiChannel, 32, 0),
        juce::MidiMessage::programChange(midiChannel, programNumber),
    };
}

} // namespace leveler
