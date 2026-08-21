#include <catch2/catch_test_macros.hpp>

#include "../src/ChannelPair.hpp"

TEST_CASE("channelPairForIndex maps pair index to adjacent channel indices", "[audio]") {
    const auto pair0 = leveler::channelPairForIndex(0);
    REQUIRE(pair0.leftIndex == 0);
    REQUIRE(pair0.rightIndex == 1);

    const auto pair3 = leveler::channelPairForIndex(3);
    REQUIRE(pair3.leftIndex == 6);
    REQUIRE(pair3.rightIndex == 7);
}

TEST_CASE("numChannelPairs covers all capture channels in stereo pairs", "[audio]") {
    REQUIRE(leveler::numChannelPairs == leveler::maxCaptureChannels / 2);
}

TEST_CASE("combineStereoSample picks the louder channel regardless of side", "[audio]") {
    const leveler::ChannelPair pair{0, 1};

    leveler::AudioFrame louderLeft{};
    louderLeft[0] = 0.8f;
    louderLeft[1] = 0.2f;
    REQUIRE(leveler::combineStereoSample(louderLeft, pair) == 0.8f);

    leveler::AudioFrame louderRight{};
    louderRight[0] = 0.1f;
    louderRight[1] = 0.9f;
    REQUIRE(leveler::combineStereoSample(louderRight, pair) == 0.9f);
}

TEST_CASE("combineStereoSample uses absolute value, so a negative sample can still win", "[audio]") {
    const leveler::ChannelPair pair{0, 1};

    leveler::AudioFrame frame{};
    frame[0] = -0.9f;
    frame[1] = 0.3f;

    REQUIRE(leveler::combineStereoSample(frame, pair) == 0.9f);
}

TEST_CASE("combineStereoSample reads the selected pair's indices, not always channels 0-1", "[audio]") {
    const leveler::ChannelPair pair{4, 5};

    leveler::AudioFrame frame{};
    frame[0] = 1.0f; // not part of this pair, must be ignored
    frame[4] = 0.4f;
    frame[5] = 0.6f;

    REQUIRE(leveler::combineStereoSample(frame, pair) == 0.6f);
}
