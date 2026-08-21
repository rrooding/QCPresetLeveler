#include <catch2/catch_test_macros.hpp>

#include <vector>

#include "../src/CaptureRingBuffer.hpp"

namespace {

// Builds a fake two-channel block where channel 0 holds ascending values and channel 1 holds
// descending values, so read-back order and per-channel routing are both verifiable.
struct FakeBlock {
    std::vector<float> channel0;
    std::vector<float> channel1;
    std::array<const float*, 2> pointers;

    explicit FakeBlock(int numFrames) {
        for (int i = 0; i < numFrames; ++i) {
            channel0.push_back((float)i);
            channel1.push_back((float)-i);
        }
        pointers = {channel0.data(), channel1.data()};
    }
};

} // namespace

TEST_CASE("CaptureRingBuffer round-trips written frames in order", "[audio]") {
    leveler::CaptureRingBuffer buffer(64);
    FakeBlock block(10);

    buffer.write(block.pointers.data(), 2, 10);
    REQUIRE(buffer.getNumReady() == 10);

    std::vector<leveler::AudioFrame> readBack(10);
    const auto framesRead = buffer.read(readBack.data(), 10);

    REQUIRE(framesRead == 10);
    for (int i = 0; i < 10; ++i) {
        REQUIRE(readBack[(size_t)i][0] == (float)i);
        REQUIRE(readBack[(size_t)i][1] == (float)-i);
    }
}

TEST_CASE("CaptureRingBuffer zero-fills channels beyond what the source provides", "[audio]") {
    leveler::CaptureRingBuffer buffer(16);
    FakeBlock block(4);

    buffer.write(block.pointers.data(), 2, 4);

    std::vector<leveler::AudioFrame> readBack(4);
    buffer.read(readBack.data(), 4);

    for (auto& frame : readBack)
        for (int ch = 2; ch < leveler::maxCaptureChannels; ++ch)
            REQUIRE(frame[(size_t)ch] == 0.0f);
}

TEST_CASE("CaptureRingBuffer read drains what was written and no more", "[audio]") {
    leveler::CaptureRingBuffer buffer(64);
    FakeBlock block(5);

    buffer.write(block.pointers.data(), 2, 5);

    std::vector<leveler::AudioFrame> readBack(20);
    const auto framesRead = buffer.read(readBack.data(), 20);

    REQUIRE(framesRead == 5);
    REQUIRE(buffer.getNumReady() == 0);
}
