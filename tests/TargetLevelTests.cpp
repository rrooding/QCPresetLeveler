#include <catch2/catch_test_macros.hpp>

#include "../src/TargetLevel.hpp"

TEST_CASE("TargetLevel defaults to -18 dBFS", "[leveling]") {
    const leveler::TargetLevel target;
    REQUIRE(target.getTargetDb() == -18.0f);
}

TEST_CASE("TargetLevel round-trips a set value", "[leveling]") {
    leveler::TargetLevel target;
    target.setTargetDb(-12.5f);
    REQUIRE(target.getTargetDb() == -12.5f);
}
