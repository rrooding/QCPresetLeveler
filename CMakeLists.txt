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
)

target_sources(QCPresetLeveler
    PRIVATE
        src/Main.cpp
        src/App.hpp
        src/MainWindow.hpp
        src/MainComponent.hpp
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
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
