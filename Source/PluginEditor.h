#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

//==============================================================================
class MyVST3PluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    MyVST3PluginAudioProcessorEditor (MyVST3PluginAudioProcessor&);
    ~MyVST3PluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    MyVST3PluginAudioProcessor& audioProcessor;

    juce::Label titleLabel;
    juce::TextButton testModeButton;
    juce::MidiKeyboardComponent keyboardComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyVST3PluginAudioProcessorEditor)
};
