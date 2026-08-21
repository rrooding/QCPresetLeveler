#include <catch2/catch_test_macros.hpp>

#include "../src/DeviceNameMatch.hpp"

TEST_CASE("looksLikeQuadCortex matches the QC's USB device name", "[audio]") {
    REQUIRE(leveler::looksLikeQuadCortex("Quad Cortex"));
    REQUIRE(leveler::looksLikeQuadCortex("Neural DSP Quad Cortex"));
    REQUIRE(leveler::looksLikeQuadCortex("quad cortex"));
}

TEST_CASE("looksLikeQuadCortex rejects unrelated device names", "[audio]") {
    REQUIRE_FALSE(leveler::looksLikeQuadCortex("Built-in Microphone"));
    REQUIRE_FALSE(leveler::looksLikeQuadCortex(""));
}
