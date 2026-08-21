#pragma once

#include <algorithm>
#include <cmath>

namespace leveler {

// Running peak + RMS level in dBFS. Metering is live/continuous while the reference loops, not
// one-shot, so the two envelopes use realistic, distinct ballistics rather than raw
// instantaneous values (see #16): peak uses fast-attack/slow-release (it should catch a
// transient immediately, then hold near it briefly, like a PPM) while RMS uses a single
// symmetric averaging time constant (it represents average power, so snapping to every
// transient like the peak detector would defeat the point). Decoupled from
// AudioDeviceManager/JUCE's callback plumbing: pure sample-in, dBFS-out, so it's directly
// unit-testable.
class LevelMeter {
public:
    explicit LevelMeter(double sampleRate, float peakAttackSeconds = 0.001f, float peakReleaseSeconds = 0.3f,
                        float rmsAveragingSeconds = 0.1f)
        : peakAttackCoefficient_(coefficientFor(sampleRate, peakAttackSeconds)),
          peakReleaseCoefficient_(coefficientFor(sampleRate, peakReleaseSeconds)),
          rmsCoefficient_(coefficientFor(sampleRate, rmsAveragingSeconds)) {}

    void processSample(float sample) {
        const float absSample = std::abs(sample);
        const float peakCoefficient =
            absSample > peakEnvelope_ ? peakAttackCoefficient_ : peakReleaseCoefficient_;
        peakEnvelope_ += peakCoefficient * (absSample - peakEnvelope_);

        const float squaredSample = sample * sample;
        meanSquareEnvelope_ += rmsCoefficient_ * (squaredSample - meanSquareEnvelope_);
    }

    [[nodiscard]] float getPeakDb() const { return linearToDb(peakEnvelope_); }

    [[nodiscard]] float getRmsDb() const { return linearToDb(std::sqrt(meanSquareEnvelope_)); }

private:
    static constexpr float silenceFloorDb = -100.0f;

    static float coefficientFor(double sampleRate, float timeSeconds) {
        if (timeSeconds <= 0.0f)
            return 1.0f;
        return 1.0f - std::exp(-1.0f / (static_cast<float>(sampleRate) * timeSeconds));
    }

    static float linearToDb(float linear) {
        if (linear <= 0.0f)
            return silenceFloorDb;
        return std::max(silenceFloorDb, 20.0f * std::log10(linear));
    }

    float peakAttackCoefficient_;
    float peakReleaseCoefficient_;
    float rmsCoefficient_;
    float peakEnvelope_ = 0.0f;
    float meanSquareEnvelope_ = 0.0f;
};

} // namespace leveler
