#include <catch2/catch_test_macros.hpp>

#include "../src/PresetSceneMemory.hpp"

TEST_CASE("PresetSceneMemory defaults to scene A (0) for a never-visited preset", "[midi]") {
    const leveler::PresetSceneMemory memory;
    REQUIRE(memory.getScene(42) == 0);
}

TEST_CASE("PresetSceneMemory recalls a recorded scene", "[midi]") {
    leveler::PresetSceneMemory memory;
    memory.recordScene(42, 3);
    REQUIRE(memory.getScene(42) == 3);
}

TEST_CASE("PresetSceneMemory remembers distinct presets independently", "[midi]") {
    leveler::PresetSceneMemory memory;
    memory.recordScene(1, 2);
    memory.recordScene(2, 5);
    REQUIRE(memory.getScene(1) == 2);
    REQUIRE(memory.getScene(2) == 5);
    REQUIRE(memory.getScene(3) == 0);
}

TEST_CASE("PresetSceneMemory overwrites a previously recorded scene", "[midi]") {
    leveler::PresetSceneMemory memory;
    memory.recordScene(7, 1);
    memory.recordScene(7, 6);
    REQUIRE(memory.getScene(7) == 6);
}
