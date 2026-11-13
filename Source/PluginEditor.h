#pragma once

#include "PluginProcessor.h"
#include "SynthLookAndFeel.h"
#include <JuceHeader.h>

/** Interfaccia grafica: controlli rotativi, selettori di forma d'onda,
 *  selettore del target del LFO, tastiera MIDI a video e tastiera del PC.
 *  Usa SynthLookAndFeel per un aspetto moderno e professionale.
 *  Include sezioni per EQ, Delay e Riverbero. */
class MyVST3PluginAudioProcessorEditor : public juce::AudioProcessorEditor,
                                         private juce::Timer
{
public:
    explicit MyVST3PluginAudioProcessorEditor (MyVST3PluginAudioProcessor&);
    ~MyVST3PluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    /** Quando la finestra diventa visibile riprende il focus da tastiera. */
    void visibilityChanged() override;

private:
    // Tastiera del computer: la mappa QWERTY viene letta a intervalli regolari.
    void timerCallback() override;
    /** Rilascia tutte le note suonate dalla tastiera del PC. */
    void releaseComputerKeys();

    MyVST3PluginAudioProcessor& audioProcessor;
    SynthLookAndFeel synthLnf;

    // Etichette di sezione
    juce::Label titleLabel;
    juce::Label oscSectionLabel;
    juce::Label envSectionLabel;
    juce::Label filterSectionLabel;
    juce::Label lfoSectionLabel;
    juce::Label fxSectionLabel;
    juce::Label eqSectionLabel;
    juce::Label delaySectionLabel;
    juce::Label reverbSectionLabel;
    juce::Label osc1WaveLabel;
    juce::Label osc2WaveLabel;

    juce::MidiKeyboardComponent keyboardComponent;

    // Selettori di forma d'onda e destinazione del LFO.
    juce::ComboBox osc1Wave, osc2Wave, lfoTarget;
    juce::Slider osc1Level, osc2Level, osc2Detune;
    // Inviluppo ADSR.
    juce::Slider attack, decay, sustain, release;
    // Filtro e LFO.
    juce::Slider cutoff, resonance, filterEnv;
    juce::Slider lfoRate, lfoDepth, gain;

    // EQ 3 bande
    juce::Slider eqLow, eqMid, eqHigh;
    // Delay
    juce::Slider delayTime, delayFeedback, delayMix;
    // Riverbero
    juce::Slider reverbSize, reverbDamping, reverbMix;

    // Attachment che collegano i controlli ai parametri del processore.
    using SliderAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAtt = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::vector<std::unique_ptr<SliderAtt>> sliderAtts;
    std::vector<std::unique_ptr<ComboAtt>> comboAtts;

    /** Crea uno slider rotativo con etichetta e lo connette al parametro. */
    void setupSlider (juce::Slider& s, const juce::String& name, const char* paramId);
    /** Popola una combo box e la connette al parametro. */
    void setupCombo (juce::ComboBox& c, const juce::StringArray& items, const char* paramId);
    /** Crea una label di sezione con stile. */
    void setupSectionLabel (juce::Label& label, const juce::String& text, const juce::Colour& colour);

    // Mappatura tasti del PC -> tasti MIDI (una nota per ogni tasto).
    static constexpr int numComputerKeys = 17;
    juce::KeyPress keyMap[numComputerKeys];
    int keyOffsets[numComputerKeys] = {};
    bool keyDown[numComputerKeys] = {};

    // Shift di ottava: Z la abbassa, X la alza (base C4 = 60).
    int octaveShift = 0;
    bool octaveDownWasDown = false;
    bool octaveUpWasDown = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyVST3PluginAudioProcessorEditor)
};