cmake_minimum_required(VERSION 3.22)

project(QCPresetLeveler VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 9.0.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_gui_app(QCPresetLeveler
    PRODUCT_NAME "QC Preset Leveler"
    COMPANY_NAME "QCPresetLeveler"
    BUNDLE_ID "com.rrooding.qcpresetleveler"
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "QC Preset Leveler needs audio input access to measure levels from your Quad Cortex."
)

target_sources(QCPresetLeveler
    PRIVATE
        src/Main.cpp
        src/CaptureRingBuffer.hpp
        src/ChannelPair.hpp
        src/DeviceNameMatch.hpp
        src/LevelMeter.hpp
        src/LevelingInstruction.hpp
        src/MeterScale.hpp
        src/MidiChannel.hpp
        src/PinkNoiseGenerator.hpp
        src/PresetChangeMessage.hpp
        src/SceneChangeMessage.hpp
        src/SetlistId.hpp
        src/SetlistSelection.hpp
        src/TargetLevel.hpp
        src/audio/AudioEngine.hpp
        src/ui/App.hpp
        src/ui/ChannelLevelMonitor.hpp
        src/ui/ChannelPairSelectorPanel.hpp
        src/ui/MainWindow.hpp
        src/ui/MainComponent.hpp
        src/ui/MidiPortManager.hpp
        src/ui/MidiPortSelectorPanel.hpp
        src/ui/PresetChangeTriggerPanel.hpp
        src/ui/PresetColumnComponent.hpp
        src/ui/PresetColumnContainer.hpp
        src/ui/SetlistSelectorPanel.hpp
        src/ui/TargetLevelPanel.hpp
        src/ui/VerticalLevelMeterComponent.hpp
)

target_compile_definitions(QCPresetLeveler
    PRIVATE
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:QCPresetLeveler,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:QCPresetLeveler,JUCE_VERSION>"
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(QCPresetLeveler
    PRIVATE
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_compile_options(QCPresetLeveler PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/WX>
)

option(QCPL_BUILD_TESTS "Build the Catch2 test suite" ON)
option(QCPL_ENABLE_COVERAGE "Instrument the test suite for coverage measurement" OFF)
option(QCPL_ENABLE_SANITIZERS "Build the test suite with ASan + UBSan" OFF)

if(QCPL_BUILD_TESTS)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.11.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(QCPresetLevelerTests
        tests/SanityTests.cpp
        tests/CaptureRingBufferTests.cpp
        tests/ChannelPairTests.cpp
        tests/DeviceNameMatchTests.cpp
        tests/LevelMeterTests.cpp
        tests/LevelingInstructionTests.cpp
        tests/MeterScaleTests.cpp
        tests/PinkNoiseGeneratorTests.cpp
        tests/PresetChangeMessageTests.cpp
        tests/SceneChangeMessageTests.cpp
        tests/SetlistSelectionTests.cpp
        tests/TargetLevelTests.cpp
    )

    target_link_libraries(QCPresetLevelerTests
        PRIVATE
            Catch2::Catch2WithMain
            juce::juce_core
            juce::juce_audio_basics
    )

    target_compile_definitions(QCPresetLevelerTests
        PRIVATE
            JUCE_USE_CURL=0
    )

    target_compile_features(QCPresetLevelerTests PRIVATE cxx_std_23)

    if(QCPL_ENABLE_COVERAGE)
        target_compile_options(QCPresetLevelerTests PRIVATE --coverage -fno-inline)
        target_link_options(QCPresetLevelerTests PRIVATE --coverage)
    endif()

    if(QCPL_ENABLE_SANITIZERS)
        target_compile_options(QCPresetLevelerTests PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(QCPresetLevelerTests PRIVATE -fsanitize=address,undefined)
    endif()

    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
    include(CTest)
    include(Catch)
    catch_discover_tests(QCPresetLevelerTests)
endif()
