#pragma once

#include "PluginProcessor.h"
#include "PresetManager.h"
#include "SynthLookAndFeel.h"
#include <JuceHeader.h>

/** Interfaccia grafica: controlli rotativi, selettori di forma d'onda,
 *  selettore del target del LFO, tastiera MIDI a video (disegnata
 *  dall'editor) e tastiera del PC.
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

    // Mouse sulla tastiera disegnata
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;

private:
    // Tastiera del computer: la mappa QWERTY viene letta a intervalli regolari.
    void timerCallback() override;
    /** Rilascia tutte le note suonate dalla tastiera del PC. */
    void releaseComputerKeys();

    // Tastiera a video disegnata dall'editor
    void drawKeyboard (juce::Graphics& g, juce::Rectangle<float> area);
    int noteAtPosition (float x, float y) const;
    void noteOnFromMouse (int note);
    void noteOffFromMouse (int note);
    int mouseNote = -1;
    // Nota MIDI realmente inviata (mouse + shift di ottava).
    int mouseNotePlaying = -1;

    MyVST3PluginAudioProcessor& audioProcessor;
    SynthLookAndFeel synthLnf;

    // Etichette di sezione
    juce::Label titleLabel;
    juce::Label oscSectionLabel;
    juce::Label envSectionLabel;
    juce::Label filterSectionLabel;
    juce::Label lfoSectionLabel;
    juce::Label eqSectionLabel;
    juce::Label delaySectionLabel;
    juce::Label reverbSectionLabel;
    juce::Label osc1WaveLabel;
    juce::Label osc2WaveLabel;

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

    // --- Gestione preset ---
    /** Ricarica la lista dei preset nella combo box. */
    void refreshPresetBox();
    /** Applica il preset selezionato. */
    void loadPreset (int number);
    /** Salva i parametri correnti (nuovo preset o sovrascrittura). */
    void savePreset (bool asNew);
    /** Rinomina il preset selezionato. */
    void renamePreset();
    /** Elimina il preset selezionato. */
    void deletePreset();
    /** Finestra di dialogo (asincrona) per inserire il nome. */
    void showNameDialog (const juce::String& title,
                         const juce::String& suggested,
                         std::function<void (juce::String)> onDone);

    PresetManager presetManager;
    int currentPresetNumber = 0;
    juce::Label presetLabel;
    juce::ComboBox presetBox;
    juce::TextButton presetSaveButton, presetSaveAsButton, presetRenameButton, presetDeleteButton;

    // Tastiera disegnata: rettangolo in coordinate locali
    juce::Rectangle<float> keyboardRect;

    // Mappatura tasti del PC -> note MIDI: due manuali da un'ottava,
    // con tasti bianchi e neri come un pianoforte (24 tasti).
    static constexpr int numComputerKeys = 24;
    juce::KeyPress keyMap[numComputerKeys];
    int keyOffsets[numComputerKeys] = {};
    bool keyDown[numComputerKeys] = {};

    // Shift di ottava: '-' la abbassa, '+' la alza (anche tastierino numerico).
    int octaveShift = 0;
    bool octaveDownWasDown = false;
    bool octaveUpWasDown = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyVST3PluginAudioProcessorEditor)
};