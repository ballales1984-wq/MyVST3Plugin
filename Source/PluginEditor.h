#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

class MyVST3PluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit MyVST3PluginAudioProcessorEditor (MyVST3PluginAudioProcessor&);
    ~MyVST3PluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MyVST3PluginAudioProcessor& audioProcessor;

    juce::Label titleLabel;
    juce::MidiKeyboardComponent keyboardComponent;

    juce::ComboBox osc1Wave, osc2Wave, lfoTarget;
    juce::Slider osc1Level, osc2Level, osc2Detune;
    juce::Slider attack, decay, sustain, release;
    juce::Slider cutoff, resonance, filterEnv;
    juce::Slider lfoRate, lfoDepth, gain;

    using SliderAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAtt = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::vector<std::unique_ptr<SliderAtt>> sliderAtts;
    std::vector<std::unique_ptr<ComboAtt>> comboAtts;

    void setupSlider (juce::Slider& s, const juce::String& name, const char* paramId);
    void setupCombo (juce::ComboBox& c, const juce::StringArray& items, const char* paramId);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyVST3PluginAudioProcessorEditor)
};
