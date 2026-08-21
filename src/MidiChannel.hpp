#pragma once

namespace leveler {

// Distinguishes a MIDI channel from other same-typed int parameters (preset/scene index) at
// MIDI-sending call sites, so they can't be silently swapped by mistake.
struct MidiChannel {
    explicit constexpr MidiChannel(int channelValue) : value(channelValue) {}

    int value;
};

} // namespace leveler
