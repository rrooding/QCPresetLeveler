#pragma once

#include <juce_audio_devices/juce_audio_devices.h>

#include <optional>

#include "../CaptureRingBuffer.hpp"
#include "../ToneGenerator.hpp"

namespace leveler {

// Registers itself as an AudioIODeviceCallback: plays a placeholder tone out while capturing
// all available input channels concurrently, so the round trip (JUCE -> QC via USB -> back
// into JUCE) can be verified before #41 replaces the tone with real pink noise and #16 reads
// the capture buffer for level analysis.
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
            const float sample = toneGenerator_.has_value() ? toneGenerator_->nextSample() : 0.0f;
            for (int ch = 0; ch < numOutputChannels; ++ch)
                if (outputChannelData[ch] != nullptr)
                    outputChannelData[ch][i] = sample;
        }

        captureBuffer_.write(inputChannelData, numInputChannels, numSamples);
    }
    // NOLINTEND(bugprone-easily-swappable-parameters)

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override {
        constexpr double placeholderToneHz = 440.0;
        toneGenerator_.emplace(placeholderToneHz, device->getCurrentSampleRate());
    }

    void audioDeviceStopped() override { toneGenerator_.reset(); }

private:
    static constexpr int captureCapacityInFrames = 48000 * 4;

    juce::AudioDeviceManager& deviceManager_;
    CaptureRingBuffer captureBuffer_{captureCapacityInFrames};
    std::optional<ToneGenerator> toneGenerator_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};

} // namespace leveler
