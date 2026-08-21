#include <catch2/catch_test_macros.hpp>

#include "../src/PinkNoiseGenerator.hpp"

TEST_CASE("PinkNoiseGenerator stays within a bounded range", "[audio]") {
    leveler::PinkNoiseGenerator generator;

    for (int i = 0; i < 10000; ++i) {
        const auto sample = generator.nextSample();
        REQUIRE(sample >= -1.5f);
        REQUIRE(sample <= 1.5f);
    }
}

TEST_CASE("PinkNoiseGenerator is deterministic for a given seed", "[audio]") {
    leveler::PinkNoiseGenerator generatorA(42);
    leveler::PinkNoiseGenerator generatorB(42);

    for (int i = 0; i < 100; ++i)
        REQUIRE(generatorA.nextSample() == generatorB.nextSample());
}

TEST_CASE("PinkNoiseGenerator produces different sequences for different seeds", "[audio]") {
    leveler::PinkNoiseGenerator generatorA(1);
    leveler::PinkNoiseGenerator generatorB(2);

    bool anyDifferent = false;
    for (int i = 0; i < 100; ++i)
        if (generatorA.nextSample() != generatorB.nextSample())
            anyDifferent = true;

    REQUIRE(anyDifferent);
}

TEST_CASE("PinkNoiseGenerator output actually varies, not silence or a constant", "[audio]") {
    leveler::PinkNoiseGenerator generator;

    const auto first = generator.nextSample();
    bool anyDifferentFromFirst = false;
    for (int i = 0; i < 100; ++i)
        if (generator.nextSample() != first)
            anyDifferentFromFirst = true;

    REQUIRE(anyDifferentFromFirst);
}
