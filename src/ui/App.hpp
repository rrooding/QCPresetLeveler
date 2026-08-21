#pragma once

#include "../DeviceNameMatch.hpp"
#include "MainWindow.hpp"

namespace leveler {

class Application final : public juce::JUCEApplication {
public:
    const juce::String getApplicationName() override { return JUCE_APPLICATION_NAME_STRING; }
    const juce::String getApplicationVersion() override { return JUCE_APPLICATION_VERSION_STRING; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String&) override {
        constexpr int maxChannels = 8;
        deviceManager_.initialiseWithDefaultDevices(maxChannels, maxChannels);
        selectPreferredDeviceIfAvailable();

        mainWindow_ = std::make_unique<MainWindow>(getApplicationName(), deviceManager_);
    }

    void shutdown() override { mainWindow_.reset(); }

    void systemRequestedQuit() override { quit(); }

    void anotherInstanceStarted(const juce::String&) override {}

private:
    void selectPreferredDeviceIfAvailable() {
        for (auto* type : deviceManager_.getAvailableDeviceTypes()) {
            for (auto& name : type->getDeviceNames()) {
                if (looksLikeQuadCortex(name)) {
                    auto setup = deviceManager_.getAudioDeviceSetup();
                    setup.outputDeviceName = name;
                    setup.inputDeviceName = name;
                    deviceManager_.setAudioDeviceSetup(setup, true);
                    return;
                }
            }
        }
    }

    juce::AudioDeviceManager deviceManager_;
    std::unique_ptr<MainWindow> mainWindow_;
};

} // namespace leveler
