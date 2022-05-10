cmake_minimum_required(VERSION 3.15)

project(Freeverb3)

add_library(revmodel
        Components/allpass.cpp
        Components/comb.cpp
        Components/revmodel.cpp
        Components/tuning.h
        )
target_include_directories(revmodel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Components)

set(JUCE_SOURCE_DIR "/Users/user/Documents/develop/JUCE")
add_subdirectory(${JUCE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/juce)

add_subdirectory(juce_plugin)