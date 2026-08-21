#pragma once

#include <random>

namespace leveler {

// Pink noise via the Paul Kellet "refined" filter method (public domain, widely used): sums
// several differently-weighted low-pass-filtered white noise taps to approximate a
// -3dB/octave (pink) spectrum. Deterministic given a seed, so it's directly testable;
// nextSample() does no allocation and is safe to call from the audio thread.
class PinkNoiseGenerator {
public:
    explicit PinkNoiseGenerator(unsigned seed = 1) : rng_(seed) {}

    float nextSample() {
        const float white = distribution_(rng_);

        b0_ = 0.99886f * b0_ + white * 0.0555179f;
        b1_ = 0.99332f * b1_ + white * 0.0750759f;
        b2_ = 0.96900f * b2_ + white * 0.1538520f;
        b3_ = 0.86650f * b3_ + white * 0.3104856f;
        b4_ = 0.55000f * b4_ + white * 0.5329522f;
        b5_ = -0.7616f * b5_ - white * 0.0168980f;
        const float pink = b0_ + b1_ + b2_ + b3_ + b4_ + b5_ + b6_ + white * 0.5362f;
        b6_ = white * 0.115926f;

        return pink * 0.11f;
    }

private:
    std::minstd_rand rng_;
    std::uniform_real_distribution<float> distribution_{-1.0f, 1.0f};

    float b0_ = 0.0f;
    float b1_ = 0.0f;
    float b2_ = 0.0f;
    float b3_ = 0.0f;
    float b4_ = 0.0f;
    float b5_ = 0.0f;
    float b6_ = 0.0f;
};

} // namespace leveler
