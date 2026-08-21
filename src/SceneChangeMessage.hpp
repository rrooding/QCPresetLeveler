#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include "MidiChannel.hpp"

namespace leveler {

// sceneIndex is 0-based (0=A ... 7=H), matching CC#43's documented value range 0-7 in the
// QC's Incoming MIDI Reserved CC List.
inline juce::MidiMessage sceneChangeMessage(int sceneIndex, MidiChannel channel) {
    return juce::MidiMessage::controllerEvent(channel.value, 43, sceneIndex);
}

} // namespace leveler
