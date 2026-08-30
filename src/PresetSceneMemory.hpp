#pragma once

#include <unordered_map>

namespace leveler {

// Remembers which scene (0-7 = A-H) was last active on each QC preset number, so switching
// back to a preset can recall it instead of always landing on scene A (see #13). Shared across
// all preset columns — QC preset identity, and therefore "the last scene used on it", is
// device-wide, not scoped to whichever column happens to be displaying that preset right now.
class PresetSceneMemory {
public:
    void recordScene(int presetNumber, int sceneIndex) { scenes_[presetNumber] = sceneIndex; }

    // Returns the remembered scene for presetNumber, or 0 (scene A) if it's never been visited.
    [[nodiscard]] int getScene(int presetNumber) const {
        const auto it = scenes_.find(presetNumber);
        return it != scenes_.end() ? it->second : 0;
    }

private:
    std::unordered_map<int, int> scenes_;
};

} // namespace leveler
