#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <array>

#include "ChannelLevelMonitor.hpp"
#include "MidiPortManager.hpp"
#include "VerticalLevelMeterComponent.hpp"

namespace leveler {

// The core per-preset column: name, dB readout, VU meter, and scene buttons A-H, mirroring
// the original mockup's card layout. Preset navigation (#11/#12) and the target/tolerance
// readout (#20/#22) are separate stories — this component only displays what's currently
// measured for whichever preset is active on the QC.
class PresetColumnComponent final : public juce::Component, private juce::Timer {
public:
    PresetColumnComponent(int presetSlotNumber, MidiPortManager& midiPortManager,
                          const ChannelLevelMonitor& levelMonitor)
        : midiPortManager_(midiPortManager), levelMonitor_(levelMonitor), meter_(levelMonitor) {
        headerLabel_.setText("Preset " + juce::String(presetSlotNumber), juce::dontSendNotification);
        headerLabel_.setJustificationType(juce::Justification::centred);
        headerLabel_.setFont(juce::Font(juce::FontOptions(20.0f, juce::Font::bold)));
        headerLabel_.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(headerLabel_);

        nameEditor_.setText("Preset " + juce::String(presetSlotNumber), false);
        nameEditor_.setJustification(juce::Justification::centred);
        nameEditor_.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff2a2f36));
        nameEditor_.setColour(juce::TextEditor::textColourId, juce::Colours::white);
        nameEditor_.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xff3a4048));
        addAndMakeVisible(nameEditor_);

        dbReadoutLabel_.setJustificationType(juce::Justification::centred);
        dbReadoutLabel_.setFont(juce::Font(juce::FontOptions(28.0f, juce::Font::bold)));
        dbReadoutLabel_.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(dbReadoutLabel_);

        addAndMakeVisible(meter_);

        for (size_t i = 0; i < sceneButtons_.size(); ++i) {
            auto& button = sceneButtons_.at(i);
            const int sceneIndex = (int)i;
            button.setButtonText(juce::String::charToString((juce::juce_wchar)('A' + sceneIndex)));
            button.onClick = [this, sceneIndex] { midiPortManager_.sendSceneChange(sceneIndex); };
            addAndMakeVisible(button);
        }

        startTimerHz(15);
    }

    ~PresetColumnComponent() override { stopTimer(); }

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        g.setColour(juce::Colour(0xff1c2128));
        g.fillRoundedRectangle(bounds, 8.0f);

        g.setColour(juce::Colour(0xfff5a623));
        g.drawRoundedRectangle(bounds, 8.0f, 2.0f);
    }

    void resized() override {
        auto area = getLocalBounds().reduced(12);
        headerLabel_.setBounds(area.removeFromTop(28));
        area.removeFromTop(6);
        nameEditor_.setBounds(area.removeFromTop(26));
        area.removeFromTop(10);
        dbReadoutLabel_.setBounds(area.removeFromTop(40));
        area.removeFromTop(6);

        auto sceneArea = area.removeFromBottom(2 * sceneButtonHeight + sceneButtonSpacing);
        area.removeFromBottom(10);
        meter_.setBounds(area);

        layoutSceneButtons(sceneArea);
    }

private:
    static constexpr int sceneButtonHeight = 44;
    static constexpr int sceneButtonSpacing = 6;

    void layoutSceneButtons(juce::Rectangle<int> area) {
        auto topRow = area.removeFromTop(sceneButtonHeight);
        area.removeFromTop(sceneButtonSpacing);
        auto& bottomRow = area;

        const auto buttonWidth = topRow.getWidth() / 4;
        for (int i = 0; i < 4; ++i)
            sceneButtons_.at((size_t)i).setBounds(topRow.removeFromLeft(buttonWidth).reduced(3));
        for (int i = 0; i < 4; ++i)
            sceneButtons_.at((size_t)4 + (size_t)i)
                .setBounds(bottomRow.removeFromLeft(buttonWidth).reduced(3));
    }

    void timerCallback() override {
        dbReadoutLabel_.setText(juce::String(levelMonitor_.getRmsDb(), 1) + " dB",
                                juce::dontSendNotification);
    }

    MidiPortManager& midiPortManager_;
    const ChannelLevelMonitor& levelMonitor_;

    juce::Label headerLabel_;
    juce::TextEditor nameEditor_;
    juce::Label dbReadoutLabel_;
    VerticalLevelMeterComponent meter_;
    std::array<juce::TextButton, 8> sceneButtons_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetColumnComponent)
};

} // namespace leveler
