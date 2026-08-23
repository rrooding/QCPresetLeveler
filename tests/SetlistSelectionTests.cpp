#include <catch2/catch_test_macros.hpp>

#include "../src/SetlistSelection.hpp"

TEST_CASE("SetlistSelection defaults to the factory presets collection (0)", "[midi]") {
    const leveler::SetlistSelection selection;
    REQUIRE(selection.getSetlist().value == 0);
}

TEST_CASE("SetlistSelection round-trips a set value", "[midi]") {
    leveler::SetlistSelection selection;
    selection.setSetlist(leveler::SetlistId{1});
    REQUIRE(selection.getSetlist().value == 1);
}
