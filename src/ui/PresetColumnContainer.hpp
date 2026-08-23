#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "../SetlistSelection.hpp"
#include "../TargetLevel.hpp"
#include "ChannelLevelMonitor.hpp"
#include "MidiPortManager.hpp"
#include "PresetColumnComponent.hpp"

namespace leveler {

// Lets the user add and remove preset columns at runtime (the mockup's "+ Add Preset"), and
// drives #11's arrow-key navigation: Left/Right move which column is selected (sending a
// Program Change for its preset number); Up/Down step the selected column's scene. Known
// limitation: arrow keys only reach here while this component (not a child text field/slider)
// has keyboard focus — clicking into a name field or a slider steals it, same as most simple
// JUCE apps; there's no global key interception here.
//
// Scroll/pagination for more columns than fit on screen is #43, not this — columns simply
// keep laying out left to right.
class PresetColumnContainer final : public juce::Component {
public:
    PresetColumnContainer(MidiPortManager& midiPortManager, const ChannelLevelMonitor& levelMonitor,
                          const TargetLevel& targetLevel, const SetlistSelection& setlistSelection)
        : midiPortManager_(midiPortManager), levelMonitor_(levelMonitor), targetLevel_(targetLevel),
          setlistSelection_(setlistSelection) {
        addButton_.setButtonText("+ Add Preset");
        addButton_.onClick = [this] { addColumn(); };
        addAndMakeVisible(addButton_);

        addColumn();

        setWantsKeyboardFocus(true);
    }

    void visibilityChanged() override {
        if (isVisible())
            grabKeyboardFocus();
    }

    bool keyPressed(const juce::KeyPress& key) override {
        if (columns_.empty())
            return false;

        if (key.isKeyCode(juce::KeyPress::leftKey)) {
            selectColumn(selectedIndex_ - 1);
            return true;
        }
        if (key.isKeyCode(juce::KeyPress::rightKey)) {
            selectColumn(selectedIndex_ + 1);
            return true;
        }
        if (key.isKeyCode(juce::KeyPress::upKey)) {
            columns_.at((size_t)selectedIndex_)->stepScene(-1);
            return true;
        }
        if (key.isKeyCode(juce::KeyPress::downKey)) {
            columns_.at((size_t)selectedIndex_)->stepScene(1);
            return true;
        }
        return false;
    }

    void resized() override {
        auto area = getLocalBounds().reduced(8);
        for (auto& column : columns_) {
            column->setBounds(area.removeFromLeft(columnWidth));
            area.removeFromLeft(columnSpacing);
        }
        addButton_.setBounds(area.removeFromLeft(columnWidth).withSizeKeepingCentre(140, 40));
    }

private:
    static constexpr int columnWidth = 280;
    static constexpr int columnSpacing = 16;

    void addColumn() {
        auto column = std::make_unique<PresetColumnComponent>((int)columns_.size() + 1, midiPortManager_,
                                                              levelMonitor_, targetLevel_, setlistSelection_);
        auto* rawColumn = column.get();
        // Deferred via callAsync: the click that triggers this originates from inside
        // rawColumn's own remove button, so destroying it synchronously here would delete the
        // button out from under its still-unwinding click-handling call stack.
        column->onRemoveRequested = [this, rawColumn] {
            juce::MessageManager::callAsync([this, rawColumn] { removeColumn(rawColumn); });
        };
        addAndMakeVisible(*column);
        columns_.push_back(std::move(column));

        selectedIndex_ = (int)columns_.size() - 1;
        renumberColumns();
        updateSelectionHighlight();
        resized();
    }

    void removeColumn(PresetColumnComponent* target) {
        std::erase_if(columns_, [target](const auto& column) { return column.get() == target; });

        if (!columns_.empty())
            selectedIndex_ = juce::jlimit(0, (int)columns_.size() - 1, selectedIndex_);
        renumberColumns();
        updateSelectionHighlight();
        resized();
    }

    void renumberColumns() {
        for (size_t i = 0; i < columns_.size(); ++i)
            columns_.at(i)->setSlotNumber((int)i + 1);
    }

    // Moves the selection (clamped, no wraparound) and, unlike just adding/removing a column,
    // actually jumps the QC to the newly selected column's preset — this is the "step between
    // loaded presets" half of #11.
    void selectColumn(int index) {
        selectedIndex_ = juce::jlimit(0, (int)columns_.size() - 1, index);
        updateSelectionHighlight();
        columns_.at((size_t)selectedIndex_)->triggerPresetChange();
    }

    void updateSelectionHighlight() {
        for (size_t i = 0; i < columns_.size(); ++i)
            columns_.at(i)->setSelected(std::cmp_equal(i, selectedIndex_));
    }

    MidiPortManager& midiPortManager_;
    const ChannelLevelMonitor& levelMonitor_;
    const TargetLevel& targetLevel_;
    const SetlistSelection& setlistSelection_;
    std::vector<std::unique_ptr<PresetColumnComponent>> columns_;
    int selectedIndex_ = 0;
    juce::TextButton addButton_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetColumnContainer)
};

} // namespace leveler
