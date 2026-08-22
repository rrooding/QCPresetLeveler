#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../src/LevelingInstruction.hpp"

TEST_CASE("computeLevelingInstruction says onTarget when measured equals target", "[leveling]") {
    const auto instruction = leveler::computeLevelingInstruction(-18.0f, -18.0f);
    REQUIRE(instruction.direction == leveler::LevelingDirection::onTarget);
    REQUIRE(instruction.adjustmentDb == 0.0f);
}

TEST_CASE("computeLevelingInstruction says raise when measured is below target beyond the deadband",
          "[leveling]") {
    const auto instruction = leveler::computeLevelingInstruction(-21.2f, -18.0f);
    REQUIRE(instruction.direction == leveler::LevelingDirection::raise);
    REQUIRE(instruction.adjustmentDb == Catch::Approx(3.2f));
}

TEST_CASE("computeLevelingInstruction says lower when measured is above target beyond the deadband",
          "[leveling]") {
    const auto instruction = leveler::computeLevelingInstruction(-14.5f, -18.0f);
    REQUIRE(instruction.direction == leveler::LevelingDirection::lower);
    REQUIRE(instruction.adjustmentDb == Catch::Approx(3.5f));
}

TEST_CASE("computeLevelingInstruction stays onTarget within the deadband on either side", "[leveling]") {
    const auto belowInstruction = leveler::computeLevelingInstruction(-18.4f, -18.0f);
    REQUIRE(belowInstruction.direction == leveler::LevelingDirection::onTarget);

    const auto aboveInstruction = leveler::computeLevelingInstruction(-17.6f, -18.0f);
    REQUIRE(aboveInstruction.direction == leveler::LevelingDirection::onTarget);
}

TEST_CASE("computeLevelingInstruction treats the deadband boundary itself as onTarget", "[leveling]") {
    const auto instruction = leveler::computeLevelingInstruction(-18.5f, -18.0f);
    REQUIRE(instruction.direction == leveler::LevelingDirection::onTarget);
}

TEST_CASE("computeLevelingInstruction adjustment magnitude is always non-negative", "[leveling]") {
    REQUIRE(leveler::computeLevelingInstruction(-30.0f, -18.0f).adjustmentDb >= 0.0f);
    REQUIRE(leveler::computeLevelingInstruction(0.0f, -18.0f).adjustmentDb >= 0.0f);
}
