#include <catch2/catch_test_macros.hpp>

#include "../src/SceneChangeMessage.hpp"

TEST_CASE("sceneChangeMessage sends CC#43 with the scene's 0-based index", "[midi]") {
    auto messageA = leveler::sceneChangeMessage(0, leveler::MidiChannel{1});
    REQUIRE(messageA.isController());
    REQUIRE(messageA.getControllerNumber() == 43);
    REQUIRE(messageA.getControllerValue() == 0);

    auto messageH = leveler::sceneChangeMessage(7, leveler::MidiChannel{1});
    REQUIRE(messageH.getControllerValue() == 7);
}

TEST_CASE("sceneChangeMessage targets the requested MIDI channel", "[midi]") {
    auto message = leveler::sceneChangeMessage(0, leveler::MidiChannel{5});
    REQUIRE(message.getChannel() == 5);
}
