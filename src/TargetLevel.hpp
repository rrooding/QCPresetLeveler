#pragma once

namespace leveler {

// Holds the user-configured target output level in dBFS. Just a value holder — the QC's Level
// parameter can't be written remotely, so this only drives the "raise/lower by X dB"
// instruction readout (see LevelingInstruction.hpp), never an actual hardware write.
class TargetLevel {
public:
    [[nodiscard]] float getTargetDb() const { return targetDb_; }
    void setTargetDb(float targetDb) { targetDb_ = targetDb; }

private:
    static constexpr float defaultTargetDb = -18.0f;

    float targetDb_ = defaultTargetDb;
};

} // namespace leveler
