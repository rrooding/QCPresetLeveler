#pragma once

#include <cmath>
#include <numbers>

namespace leveler {

// Simple sine generator used as a real-time-safe placeholder output signal for #15's duplex
// engine, ahead of #41's pink noise generator replacing it. nextSample() does no allocation
// and is safe to call from the audio thread.
class ToneGenerator {
public:
    ToneGenerator(double frequencyHz, double sampleRate) : phaseIncrement_(frequencyHz / sampleRate) {}

    float nextSample() {
        const auto sample = (float)std::sin(phase_ * 2.0 * std::numbers::pi);
        phase_ += phaseIncrement_;
        if (phase_ >= 1.0)
            phase_ -= 1.0;
        return sample;
    }

private:
    double phaseIncrement_;
    double phase_ = 0.0;
};

} // namespace leveler
