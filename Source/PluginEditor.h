#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

/** Interfaccia grafica: controlli rotativi, selettori di forma d'onda,
 *  selettore del target del LFO e tastiera MIDI a video. */
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

    // Selettori di forma d'onda e destinazione del LFO.
    juce::ComboBox osc1Wave, osc2Wave, lfoTarget;
    juce::Slider osc1Level, osc2Level, osc2Detune;
    // Inviluppo ADSR.
    juce::Slider attack, decay, sustain, release;
    // Filtro e LFO.
    juce::Slider cutoff, resonance, filterEnv;
    juce::Slider lfoRate, lfoDepth, gain;

    // Attachment che collegano i controlli ai parametri del processore.
    using SliderAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAtt = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::vector<std::unique_ptr<SliderAtt>> sliderAtts;
    std::vector<std::unique_ptr<ComboAtt>> comboAtts;

    /** Crea uno slider rotativo con etichetta e lo connette al parametro. */
    void setupSlider (juce::Slider& s, const juce::String& name, const char* paramId);
    /** Popola una combo box e la connette al parametro. */
    void setupCombo (juce::ComboBox& c, const juce::StringArray& items, const char* paramId);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyVST3PluginAudioProcessorEditor)
};
