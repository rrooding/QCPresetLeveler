#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <algorithm>
#include <memory>
#include <vector>

#include "../TargetLevel.hpp"
#include "ChannelLevelMonitor.hpp"
#include "MidiPortManager.hpp"
#include "PresetColumnComponent.hpp"

namespace leveler {

// Lets the user add and remove preset columns at runtime (the mockup's "+ Add Preset").
// Scroll/pagination for more columns than fit on screen is #43, not this — columns simply
// keep laying out left to right.
class PresetColumnContainer final : public juce::Component {
public:
    PresetColumnContainer(MidiPortManager& midiPortManager, const ChannelLevelMonitor& levelMonitor,
                          const TargetLevel& targetLevel)
        : midiPortManager_(midiPortManager), levelMonitor_(levelMonitor), targetLevel_(targetLevel) {
        addButton_.setButtonText("+ Add Preset");
        addButton_.onClick = [this] { addColumn(); };
        addAndMakeVisible(addButton_);

        addColumn();
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
                                                              levelMonitor_, targetLevel_);
        auto* rawColumn = column.get();
        // Deferred via callAsync: the click that triggers this originates from inside
        // rawColumn's own remove button, so destroying it synchronously here would delete the
        // button out from under its still-unwinding click-handling call stack.
        column->onRemoveRequested = [this, rawColumn] {
            juce::MessageManager::callAsync([this, rawColumn] { removeColumn(rawColumn); });
        };
        addAndMakeVisible(*column);
        columns_.push_back(std::move(column));

        renumberColumns();
        resized();
    }

    void removeColumn(PresetColumnComponent* target) {
        std::erase_if(columns_, [target](const auto& column) { return column.get() == target; });

        renumberColumns();
        resized();
    }

    void renumberColumns() {
        for (size_t i = 0; i < columns_.size(); ++i)
            columns_.at(i)->setSlotNumber((int)i + 1);
    }

    MidiPortManager& midiPortManager_;
    const ChannelLevelMonitor& levelMonitor_;
    const TargetLevel& targetLevel_;
    std::vector<std::unique_ptr<PresetColumnComponent>> columns_;
    juce::TextButton addButton_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetColumnContainer)
};

} // namespace leveler
