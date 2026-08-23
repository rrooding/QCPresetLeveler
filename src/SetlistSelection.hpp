#pragma once

#include "SetlistId.hpp"

namespace leveler {

// Holds the user-configured target setlist/collection (sent as CC#32), shared globally for
// the whole session — one setlist at a time is the realistic workflow (see #48), same
// reasoning as TargetLevel. There's no documented way to query the QC's actually-active
// setlist over MIDI, so this never has a "correct" default matching hardware state; 0
// (factory presets) is just a starting point the user is expected to change.
class SetlistSelection {
public:
    [[nodiscard]] SetlistId getSetlist() const { return setlist_; }
    void setSetlist(SetlistId setlist) { setlist_ = setlist; }

private:
    SetlistId setlist_{0};
};

} // namespace leveler
