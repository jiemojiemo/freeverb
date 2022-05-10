project(Freeverb3_PLUGIN VERSION 0.0.1)

juce_add_plugin(
        Freeverb3
        PLUGIN_MANUFACTURER_CODE LIBA
        PLUGIN_CODE Free
        FORMATS VST3
        PRODUCT_NAME "Freeverb3"
)

target_sources(Freeverb3
        PRIVATE
        PluginProcessor.cpp)

target_compile_definitions(Freeverb3
        PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(Freeverb3
        PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        juce::juce_audio_utils
        revmodel
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)