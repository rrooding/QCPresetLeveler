#pragma once

#include "../audio/AudioEngine.hpp"
#include "MainComponent.hpp"

namespace leveler {

class MainWindow final : public juce::DocumentWindow {
public:
    MainWindow(const juce::String& name, juce::AudioDeviceManager& deviceManager,
               MidiPortManager& midiPortManager, AudioEngine& audioEngine)
        : DocumentWindow(name,
                         juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(
                             juce::ResizableWindow::backgroundColourId),
                         DocumentWindow::allButtons) {
        setUsingNativeTitleBar(true);
        setContentOwned(new MainComponent(deviceManager, midiPortManager, audioEngine), true);

#if JUCE_IOS || JUCE_ANDROID
        setFullScreen(true);
#else
        setResizable(true, true);
        centreWithSize(getWidth(), getHeight());
#endif

        setVisible(true);
    }

    void closeButtonPressed() override { juce::JUCEApplication::getInstance()->systemRequestedQuit(); }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};

} // namespace leveler
