#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <numbers>

#include "../src/LevelMeter.hpp"

namespace {

constexpr double sampleRate = 48000.0;

} // namespace

TEST_CASE("LevelMeter reports the silence floor before any samples arrive", "[audio]") {
    const leveler::LevelMeter meter(sampleRate);

    REQUIRE(meter.getPeakDb() == Catch::Approx(-100.0f));
    REQUIRE(meter.getRmsDb() == Catch::Approx(-100.0f));
}

TEST_CASE("LevelMeter reports the silence floor for actual silence, not -inf/nan", "[audio]") {
    leveler::LevelMeter meter(sampleRate);

    for (int i = 0; i < 1000; ++i)
        meter.processSample(0.0f);

    REQUIRE(meter.getPeakDb() == Catch::Approx(-100.0f));
    REQUIRE(meter.getRmsDb() == Catch::Approx(-100.0f));
}

TEST_CASE("LevelMeter with zero attack/averaging follows a full-scale sample instantly", "[audio]") {
    leveler::LevelMeter meter(sampleRate, 0.0f, 0.3f, 0.0f);

    meter.processSample(1.0f);

    REQUIRE(meter.getPeakDb() == Catch::Approx(0.0f).margin(0.01f));
    REQUIRE(meter.getRmsDb() == Catch::Approx(0.0f).margin(0.01f));
}

TEST_CASE("LevelMeter peak uses absolute value, so a negative sample reads as full-scale too", "[audio]") {
    leveler::LevelMeter meter(sampleRate, 0.0f, 0.3f, 0.0f);

    meter.processSample(-1.0f);

    REQUIRE(meter.getPeakDb() == Catch::Approx(0.0f).margin(0.01f));
    REQUIRE(meter.getRmsDb() == Catch::Approx(0.0f).margin(0.01f));
}

TEST_CASE("LevelMeter converges to the correct dBFS for a full-scale sine wave", "[audio]") {
    leveler::LevelMeter meter(sampleRate, 0.001f, 0.3f, 0.1f);

    const float frequency = 1000.0f;
    for (int i = 0; i < 48000; ++i) {
        const float phase = 2.0f * std::numbers::pi_v<float> * frequency * static_cast<float>(i) /
                            static_cast<float>(sampleRate);
        meter.processSample(std::sin(phase));
    }

    REQUIRE(meter.getPeakDb() == Catch::Approx(0.0f).margin(0.2f));
    REQUIRE(meter.getRmsDb() == Catch::Approx(-3.01f).margin(0.2f));
}

TEST_CASE("LevelMeter peak attack coefficient reaches ~63% of a step after one time constant", "[audio]") {
    leveler::LevelMeter meter(sampleRate, 0.1f, 0.1f, 0.1f);

    const int samplesPerTimeConstant = static_cast<int>(sampleRate * 0.1);
    for (int i = 0; i < samplesPerTimeConstant; ++i)
        meter.processSample(1.0f);

    const float expectedLinear = 1.0f - std::exp(-1.0f);
    const float expectedDb = 20.0f * std::log10(expectedLinear);
    REQUIRE(meter.getPeakDb() == Catch::Approx(expectedDb).margin(0.5f));
}

TEST_CASE("LevelMeter peak attacks faster than it releases", "[audio]") {
    leveler::LevelMeter meter(sampleRate, 0.001f, 1.0f, 0.1f);

    const int attackSamples = 500; // ~10 attack time constants: fully settled
    for (int i = 0; i < attackSamples; ++i)
        meter.processSample(1.0f);
    const float afterAttack = meter.getPeakDb();

    const int releaseSamples = 24000; // half a release time constant: partially decayed
    for (int i = 0; i < releaseSamples; ++i)
        meter.processSample(0.0f);
    const float afterRelease = meter.getPeakDb();

    REQUIRE(afterAttack == Catch::Approx(0.0f).margin(0.1f));
    REQUIRE(afterRelease < afterAttack);
    REQUIRE(afterRelease > -10.0f);
}

TEST_CASE("LevelMeter RMS reflects average power, not the instantaneous peak, for a varying signal",
          "[audio]") {
    leveler::LevelMeter meter(sampleRate, 0.001f, 0.3f, 0.1f);

    const float frequency = 1000.0f;
    for (int i = 0; i < 48000; ++i) {
        const float phase = 2.0f * std::numbers::pi_v<float> * frequency * static_cast<float>(i) /
                            static_cast<float>(sampleRate);
        meter.processSample(std::sin(phase));
    }

    REQUIRE(meter.getRmsDb() < meter.getPeakDb());
}
