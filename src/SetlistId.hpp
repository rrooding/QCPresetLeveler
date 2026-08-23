#pragma once

namespace leveler {

// Distinguishes a QC setlist/collection selector value (sent as CC#32) from other same-typed
// int parameters (preset/scene index, MIDI channel) at MIDI-sending call sites. Range is the
// documented 0-127; which value maps to a user's own setlist depends on their device (0 is
// the factory presets collection).
struct SetlistId {
    explicit constexpr SetlistId(int setlistValue) : value(setlistValue) {}

    int value;
};

} // namespace leveler
