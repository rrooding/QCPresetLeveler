#include <catch2/catch_test_macros.hpp>

// Placeholder proving the Catch2/CTest/coverage harness works end-to-end before any
// real logic (MIDI, level measurement, session state) exists to test against.
TEST_CASE("test harness is wired up", "[sanity]") {
    REQUIRE(1 + 1 == 2);
}
