#pragma once

#include <juce_audio_devices/juce_audio_devices.h>

#include "../CaptureRingBuffer.hpp"
#include "../PinkNoiseGenerator.hpp"

namespace leveler {

// Registers itself as an AudioIODeviceCallback: plays pink noise out while capturing all
// available input channels concurrently, so #16 can read the capture buffer for level
// analysis. The reference signal itself doesn't depend on sample rate, so it's a plain member
// rather than something reconstructed per device start/stop.
class AudioEngine final : public juce::AudioIODeviceCallback {
public:
    explicit AudioEngine(juce::AudioDeviceManager& deviceManager) : deviceManager_(deviceManager) {
        deviceManager_.addAudioCallback(this);
    }

    ~AudioEngine() override { deviceManager_.removeAudioCallback(this); }

    [[nodiscard]] CaptureRingBuffer& getCaptureBuffer() { return captureBuffer_; }

    // NOLINTBEGIN(bugprone-easily-swappable-parameters): fixed JUCE interface, not ours to reshape
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                                          float* const* outputChannelData, int numOutputChannels,
                                          int numSamples,
                                          const juce::AudioIODeviceCallbackContext&) override {
        for (int i = 0; i < numSamples; ++i) {
            const float sample = pinkNoiseGenerator_.nextSample();
            for (int ch = 0; ch < numOutputChannels; ++ch)
                if (outputChannelData[ch] != nullptr)
                    outputChannelData[ch][i] = sample;
        }

        captureBuffer_.write(inputChannelData, numInputChannels, numSamples);
    }
    // NOLINTEND(bugprone-easily-swappable-parameters)

    void audioDeviceAboutToStart(juce::AudioIODevice*) override {}

    void audioDeviceStopped() override {}

private:
    static constexpr int captureCapacityInFrames = 48000 * 4;

    juce::AudioDeviceManager& deviceManager_;
    CaptureRingBuffer captureBuffer_{captureCapacityInFrames};
    PinkNoiseGenerator pinkNoiseGenerator_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};

} // namespace leveler
