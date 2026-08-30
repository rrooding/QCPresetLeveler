#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <array>
#include <functional>
#include <utility>

#include "../LevelingInstruction.hpp"
#include "../PresetSceneMemory.hpp"
#include "../SetlistSelection.hpp"
#include "../TargetLevel.hpp"
#include "ChannelLevelMonitor.hpp"
#include "MidiPortManager.hpp"
#include "VerticalLevelMeterComponent.hpp"

namespace leveler {

// The core per-preset column: name, dB readout, VU meter, and scene buttons A-H, mirroring
// the original mockup's card layout, plus the raise/lower instruction readout (#20) driven by
// the shared target level, and the QC preset number + current scene this column tracks for
// arrow-key navigation (#11). Whenever the preset number changes, the scene is recalled from
// presetSceneMemory rather than left as-is or reset to A (#13). The in-tolerance visual state
// (#22) is a separate story — this component only displays what's currently measured, and
// states the instruction in plain text.
class PresetColumnComponent final : public juce::Component, private juce::Timer {
public:
    PresetColumnComponent(int presetSlotNumber, MidiPortManager& midiPortManager,
                          const ChannelLevelMonitor& levelMonitor, const TargetLevel& targetLevel,
                          const SetlistSelection& setlistSelection, PresetSceneMemory& presetSceneMemory)
        : midiPortManager_(midiPortManager), levelMonitor_(levelMonitor), targetLevel_(targetLevel),
          setlistSelection_(setlistSelection), presetSceneMemory_(presetSceneMemory), meter_(levelMonitor) {
        headerLabel_.setText("Preset " + juce::String(presetSlotNumber), juce::dontSendNotification);
        headerLabel_.setJustificationType(juce::Justification::centred);
        headerLabel_.setFont(juce::Font(juce::FontOptions(20.0f, juce::Font::bold)));
        headerLabel_.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(headerLabel_);

        removeButton_.setButtonText("x");
        removeButton_.onClick = [this] {
            if (onRemoveRequested)
                onRemoveRequested();
        };
        addAndMakeVisible(removeButton_);

        presetNumberLabel_.setText("QC #", juce::dontSendNotification);
        presetNumberLabel_.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        addAndMakeVisible(presetNumberLabel_);

        presetNumberSlider_.setSliderStyle(juce::Slider::IncDecButtons);
        presetNumberSlider_.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 22);
        presetNumberSlider_.setRange(1, 256, 1);
        presetNumberSlider_.setValue(presetSlotNumber);
        // Depends on presetNumberSlider_'s value (set just above in this same body), so can't
        // move earlier into the member-initializer list.
        // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer): see above
        currentSceneIndex_ = presetSceneMemory_.getScene(getPresetNumber());
        presetNumberSlider_.onValueChange = [this] {
            triggerPresetChange();
            setCurrentScene(presetSceneMemory_.getScene(getPresetNumber()));
        };
        addAndMakeVisible(presetNumberSlider_);

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

        instructionLabel_.setJustificationType(juce::Justification::centred);
        instructionLabel_.setFont(juce::Font(juce::FontOptions(14.0f)));
        instructionLabel_.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        addAndMakeVisible(instructionLabel_);

        addAndMakeVisible(meter_);

        for (size_t i = 0; i < sceneButtons_.size(); ++i) {
            auto& button = sceneButtons_.at(i);
            const int sceneIndex = (int)i;
            button.setButtonText(juce::String::charToString((juce::juce_wchar)('A' + sceneIndex)));
            button.onClick = [this, sceneIndex] { setCurrentScene(sceneIndex); };
            addAndMakeVisible(button);
        }
        updateSceneButtonColours();

        startTimerHz(15);
    }

    ~PresetColumnComponent() override { stopTimer(); }

    // Set by whoever owns a dynamic collection of columns (#21); invoked when the user clicks
    // this column's remove button. Left empty when there's no such owner (e.g. in isolation).
    // Public callback member matches JUCE's own idiom (Button::onClick, ComboBox::onChange).
    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::function<void()> onRemoveRequested;

    void setSlotNumber(int presetSlotNumber) {
        headerLabel_.setText("Preset " + juce::String(presetSlotNumber), juce::dontSendNotification);
    }

    // The QC preset number this column tracks (0-based, matching MidiPortManager's convention).
    [[nodiscard]] int getPresetNumber() const { return (int)presetNumberSlider_.getValue() - 1; }

    // Sends a Program Change for this column's preset number, driven by #11's arrow-key
    // navigation (or by the user directly editing the preset number field).
    void triggerPresetChange() {
        midiPortManager_.sendProgramChange(getPresetNumber(), MidiChannel{1}, setlistSelection_.getSetlist());
    }

    // Moves the tracked current scene by delta (clamped to A-H) and sends the CC#43 change,
    // driven by #11's Up/Down arrow-key navigation.
    void stepScene(int delta) { setCurrentScene(juce::jlimit(0, 7, currentSceneIndex_ + delta)); }

    void setSelected(bool selected) {
        isSelected_ = selected;
        repaint();
    }

    [[nodiscard]] bool isSelected() const { return isSelected_; }

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        g.setColour(juce::Colour(0xff1c2128));
        g.fillRoundedRectangle(bounds, 8.0f);

        g.setColour(isSelected_ ? juce::Colour(0xfff5a623) : juce::Colour(0xff3a4048));
        g.drawRoundedRectangle(bounds, 8.0f, 2.0f);
    }

    void resized() override {
        auto area = getLocalBounds().reduced(12);
        auto headerRow = area.removeFromTop(28);
        removeButton_.setBounds(headerRow.removeFromRight(24));
        headerLabel_.setBounds(headerRow);
        area.removeFromTop(6);

        auto presetNumberRow = area.removeFromTop(24);
        presetNumberLabel_.setBounds(presetNumberRow.removeFromLeft(36));
        presetNumberSlider_.setBounds(presetNumberRow);
        area.removeFromTop(6);

        nameEditor_.setBounds(area.removeFromTop(26));
        area.removeFromTop(10);
        dbReadoutLabel_.setBounds(area.removeFromTop(40));
        area.removeFromTop(2);
        instructionLabel_.setBounds(area.removeFromTop(18));
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

    void setCurrentScene(int sceneIndex) {
        currentSceneIndex_ = sceneIndex;
        presetSceneMemory_.recordScene(getPresetNumber(), currentSceneIndex_);
        midiPortManager_.sendSceneChange(currentSceneIndex_);
        updateSceneButtonColours();
    }

    void updateSceneButtonColours() {
        for (size_t i = 0; i < sceneButtons_.size(); ++i) {
            const bool isCurrent = std::cmp_equal(i, currentSceneIndex_);
            sceneButtons_.at(i).setColour(juce::TextButton::buttonColourId,
                                          isCurrent ? juce::Colour(0xfff5a623) : juce::Colour(0xff2a2f36));
            sceneButtons_.at(i).setColour(juce::TextButton::textColourOffId,
                                          isCurrent ? juce::Colours::black : juce::Colours::white);
        }
    }

    void timerCallback() override {
        const auto measuredDb = levelMonitor_.getRmsDb();
        dbReadoutLabel_.setText(juce::String(measuredDb, 1) + " dB", juce::dontSendNotification);

        const auto instruction = computeLevelingInstruction(measuredDb, targetLevel_.getTargetDb());
        switch (instruction.direction) {
        case LevelingDirection::onTarget:
            instructionLabel_.setText("On target", juce::dontSendNotification);
            break;
        case LevelingDirection::raise:
            instructionLabel_.setText("Raise by " + juce::String(instruction.adjustmentDb, 1) + " dB",
                                      juce::dontSendNotification);
            break;
        case LevelingDirection::lower:
            instructionLabel_.setText("Lower by " + juce::String(instruction.adjustmentDb, 1) + " dB",
                                      juce::dontSendNotification);
            break;
        }
    }

    MidiPortManager& midiPortManager_;
    const ChannelLevelMonitor& levelMonitor_;
    const TargetLevel& targetLevel_;
    const SetlistSelection& setlistSelection_;
    PresetSceneMemory& presetSceneMemory_;

    int currentSceneIndex_ = 0;
    bool isSelected_ = false;

    juce::Label headerLabel_;
    juce::TextButton removeButton_;
    juce::Label presetNumberLabel_;
    juce::Slider presetNumberSlider_;
    juce::TextEditor nameEditor_;
    juce::Label dbReadoutLabel_;
    juce::Label instructionLabel_;
    VerticalLevelMeterComponent meter_;
    std::array<juce::TextButton, 8> sceneButtons_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetColumnComponent)
};

} // namespace leveler
