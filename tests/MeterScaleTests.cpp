#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../src/MeterScale.hpp"

TEST_CASE("dbToMeterFraction clamps a reading below the range to 0", "[audio]") {
    REQUIRE(leveler::dbToMeterFraction(-100.0f, -60.0f, 0.0f) == 0.0f);
}

TEST_CASE("dbToMeterFraction clamps a reading above the range to 1", "[audio]") {
    REQUIRE(leveler::dbToMeterFraction(6.0f, -60.0f, 0.0f) == 1.0f);
}

TEST_CASE("dbToMeterFraction maps the range endpoints to 0 and 1", "[audio]") {
    REQUIRE(leveler::dbToMeterFraction(-60.0f, -60.0f, 0.0f) == 0.0f);
    REQUIRE(leveler::dbToMeterFraction(0.0f, -60.0f, 0.0f) == 1.0f);
}

TEST_CASE("dbToMeterFraction is linear in dB, not linear in amplitude", "[audio]") {
    REQUIRE(leveler::dbToMeterFraction(-30.0f, -60.0f, 0.0f) == Catch::Approx(0.5f));
}

TEST_CASE("dbToMeterFraction defaults to the standard -60..0 dBFS meter range", "[audio]") {
    REQUIRE(leveler::dbToMeterFraction(-60.0f) == 0.0f);
    REQUIRE(leveler::dbToMeterFraction(0.0f) == 1.0f);
}

TEST_CASE("dbToMeterFraction returns 0 for a degenerate (non-positive) range", "[audio]") {
    REQUIRE(leveler::dbToMeterFraction(-10.0f, 0.0f, 0.0f) == 0.0f);
    REQUIRE(leveler::dbToMeterFraction(-10.0f, 0.0f, -60.0f) == 0.0f);
}
