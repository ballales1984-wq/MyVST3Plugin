#pragma once

#include "PluginProcessor.h"

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_utils/juce_audio_utils.h>

//==============================================================================
/**
    Dual Oscillator VST3 Plugin Editor

    Features:
    - Master volume slider
    - Oscillator 1 frequency slider
    - Oscillator 2 frequency slider
    - ADSR controls (Attack, Decay, Sustain, Release)
    - Real-time parameter display
    - Virtual MIDI keyboard
*/
class MyVST3PluginAudioProcessorEditor : public juce::AudioProcessorEditor,
                                         public juce::Timer
{
public:
    MyVST3PluginAudioProcessorEditor (MyVST3PluginAudioProcessor&);
    ~MyVST3PluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void timerCallback() override;

private:
    //==============================================================================
    // Reference to the processor
    MyVST3PluginAudioProcessor& audioProcessor;

    // GUI Components
    juce::Label titleLabel;

    // Simplified single-view layout
    // Removed complex tabbed interface for now

    // Keyboard Component
    juce::MidiKeyboardComponent keyboardComponent;

    // Sliders
    juce::Slider masterVolumeSlider;
    juce::Slider osc1FrequencySlider;
    juce::Slider osc2FrequencySlider;
    juce::Slider osc2DetuneSlider;  // NEW: Detune slider for Osc2
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    // Waveform selectors (ComboBox)
    juce::ComboBox osc1WaveformSelector;  // NEW: Waveform selector for Osc1
    juce::ComboBox osc2WaveformSelector;  // NEW: Waveform selector for Osc2

    // PWM Control
    juce::Slider squarePWMSlider;         // NEW: PWM slider for square wave

    // Filter sliders
    juce::Slider filterCutoffSlider;
    juce::Slider filterResonanceSlider;

    // LFO Controls
    juce::Slider lfoRateSlider;
    juce::ComboBox lfoWaveformSelector;
    juce::Slider lfoAmountSlider;
    juce::ToggleButton lfoToOsc1Button;
    juce::ToggleButton lfoToAmpButton;

    // Labels
    juce::Label masterVolumeLabel;
    juce::Label osc1FrequencyLabel;
    juce::Label osc2FrequencyLabel;
    juce::Label osc2DetuneLabel;  // NEW: Detune label
    juce::Label squarePWMLabel;     // NEW: PWM label
    juce::Label osc1WaveformLabel;  // NEW: Waveform label for Osc1
    juce::Label osc2WaveformLabel;  // NEW: Waveform label for Osc2
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    // Filter labels
    juce::Label filterCutoffLabel;
    juce::Label filterResonanceLabel;

    // LFO Labels
    juce::Label lfoRateLabel;
    juce::Label lfoWaveformLabel;
    juce::Label lfoAmountLabel;
    juce::Label lfoRoutingLabel; // Label for routing section

    // Value labels (for displaying current values)
    juce::Label masterVolumeValueLabel;
    juce::Label osc1FrequencyValueLabel;
    juce::Label osc2FrequencyValueLabel;
    juce::Label osc2DetuneValueLabel;  // NEW: Detune value label
    juce::Label squarePWMValueLabel; // NEW: PWM value label
    juce::Label attackValueLabel;
    juce::Label decayValueLabel;
    juce::Label sustainValueLabel;
    juce::Label releaseValueLabel;

    // LFO Value Labels (simplified)
    juce::Label lfoRateValueLabel;
    juce::Label lfoAmountValueLabel;

    // Filter value labels
    juce::Label filterCutoffValueLabel;
    juce::Label filterResonanceValueLabel;

    // MIDI status display
    juce::Label midiStatusLabel;
    juce::Label currentNoteLabel;

    // ADSR envelope level display
    juce::Label envelopeLabel;

    // Test mode toggle
    juce::ToggleButton testModeButton;

    // Envelope status
    juce::Label envelopeStatusLabel;

    // Attachments for connecting sliders to parameters
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolumeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1FrequencyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2FrequencyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2DetuneAttachment;  // NEW: Detune attachment
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> squarePWMAttachment;      // NEW: PWM attachment
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc1WaveformAttachment;  // NEW: Waveform attachment for Osc1
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc2WaveformAttachment;  // NEW: Waveform attachment for Osc2
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResonanceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> testModeAttachment;

    // LFO Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfoWaveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoAmountAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lfoToOsc1Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lfoToAmpAttachment;

    // Simplified layout - back to basics

    //==============================================================================
    // Helper methods
    void setupWaveformSelector(juce::ComboBox& comboBox, juce::Label& label, const juce::String& paramName);
    void setupSlider(juce::Slider& slider, juce::Label& label, juce::Label& valueLabel,
                    const juce::String& paramName, float minValue, float maxValue,
                    float defaultValue, const juce::String& unit = "");
    void updateValueLabels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyVST3PluginAudioProcessorEditor)
};
