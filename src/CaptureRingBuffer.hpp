#pragma once

#include <juce_core/juce_core.h>

#include <array>
#include <vector>

namespace leveler {

inline constexpr int maxCaptureChannels = 8;

using AudioFrame = std::array<float, maxCaptureChannels>;

// Lock-free ring buffer for multi-channel captured audio. write() is real-time-safe (no
// allocation, no locks) and is the only method meant to be called from the audio thread;
// read() drains it from elsewhere (the message thread). Channels beyond what the source
// provides are left at 0; channels beyond maxCaptureChannels are dropped.
//
// Raw pointer/array indexing throughout is inherent to buffer manipulation like this — JUCE's
// audio callback hands you raw float* arrays with no bounds-checked alternative — so
// cppcoreguidelines-pro-bounds-* is disabled project-wide rather than fought line by line
// (see .clang-tidy). write()/writeSegment()/readSegment() take several same-typed offset/count
// ints each; they're small, fully tested, single-purpose private helpers with few call sites,
// so a strong-type refactor (like MidiChannel) isn't worth it the way it was for public
// multi-call-site MIDI functions — suppressed with NOLINT instead.
class CaptureRingBuffer {
public:
    explicit CaptureRingBuffer(int capacityInFrames)
        : fifo_(capacityInFrames), storage_((size_t)capacityInFrames) {}

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    void write(const float* const* channelData, int numChannelsAvailable, int numFrames) {
        int start1 = 0, size1 = 0, start2 = 0, size2 = 0;
        fifo_.prepareToWrite(numFrames, start1, size1, start2, size2);
        writeSegment(channelData, numChannelsAvailable, 0, start1, size1);
        writeSegment(channelData, numChannelsAvailable, size1, start2, size2);
        fifo_.finishedWrite(size1 + size2);
    }

    int read(AudioFrame* dest, int maxFrames) {
        int start1 = 0, size1 = 0, start2 = 0, size2 = 0;
        fifo_.prepareToRead(maxFrames, start1, size1, start2, size2);
        readSegment(dest, 0, start1, size1);
        readSegment(dest, size1, start2, size2);
        fifo_.finishedRead(size1 + size2);
        return size1 + size2;
    }

    [[nodiscard]] int getNumReady() const { return fifo_.getNumReady(); }

private:
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    void writeSegment(const float* const* channelData, int numChannelsAvailable, int sourceOffset,
                      int destStart, int count) {
        for (int i = 0; i < count; ++i) {
            AudioFrame frame{};
            for (int ch = 0; ch < numChannelsAvailable && ch < maxCaptureChannels; ++ch)
                frame[(size_t)ch] = channelData[ch][(size_t)sourceOffset + (size_t)i];
            storage_[(size_t)destStart + (size_t)i] = frame;
        }
    }

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    void readSegment(AudioFrame* dest, int destOffset, int sourceStart, int count) {
        for (int i = 0; i < count; ++i)
            dest[(size_t)destOffset + (size_t)i] = storage_[(size_t)sourceStart + (size_t)i];
    }

    juce::AbstractFifo fifo_;
    std::vector<AudioFrame> storage_;
};

} // namespace leveler
