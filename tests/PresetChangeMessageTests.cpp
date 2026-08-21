#include <catch2/catch_test_macros.hpp>

#include "../src/PresetChangeMessage.hpp"

TEST_CASE("presetChangeMessages selects bank 0 for presets 0-127", "[midi]") {
    auto messages = leveler::presetChangeMessages(0, 1);
    REQUIRE(messages.size() == 3);
    REQUIRE(messages[0].isController());
    REQUIRE(messages[0].getControllerNumber() == 0);
    REQUIRE(messages[0].getControllerValue() == 0);
    REQUIRE(messages[1].isController());
    REQUIRE(messages[1].getControllerNumber() == 32);
    REQUIRE(messages[1].getControllerValue() == 0);
    REQUIRE(messages[2].isProgramChange());
    REQUIRE(messages[2].getProgramChangeNumber() == 0);
}

TEST_CASE("presetChangeMessages selects bank 1 and wraps the program number for presets 128-255", "[midi]") {
    auto messages = leveler::presetChangeMessages(128, 1);
    REQUIRE(messages[0].getControllerValue() == 1);
    REQUIRE(messages[2].getProgramChangeNumber() == 0);

    auto lastMessages = leveler::presetChangeMessages(255, 1);
    REQUIRE(lastMessages[0].getControllerValue() == 1);
    REQUIRE(lastMessages[2].getProgramChangeNumber() == 127);
}

TEST_CASE("presetChangeMessages targets the requested MIDI channel", "[midi]") {
    auto messages = leveler::presetChangeMessages(0, 5);
    REQUIRE(messages[0].getChannel() == 5);
    REQUIRE(messages[1].getChannel() == 5);
    REQUIRE(messages[2].getChannel() == 5);
}
