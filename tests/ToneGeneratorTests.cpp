#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <vector>

#include "../src/ToneGenerator.hpp"

TEST_CASE("ToneGenerator starts at zero and stays within [-1, 1]", "[audio]") {
    leveler::ToneGenerator generator(1000.0, 48000.0);

    REQUIRE(generator.nextSample() == Catch::Approx(0.0f).margin(0.0001));

    for (int i = 0; i < 1000; ++i) {
        const auto sample = generator.nextSample();
        REQUIRE(sample >= -1.0f);
        REQUIRE(sample <= 1.0f);
    }
}

TEST_CASE("ToneGenerator repeats after one full period", "[audio]") {
    // 1kHz at 48kHz sample rate: exactly 48 samples per period.
    leveler::ToneGenerator generator(1000.0, 48000.0);

    std::vector<float> firstPeriod;
    for (int i = 0; i < 48; ++i)
        firstPeriod.push_back(generator.nextSample());

    for (int i = 0; i < 48; ++i)
        REQUIRE(generator.nextSample() == Catch::Approx(firstPeriod[(size_t)i]).margin(0.001));
}
