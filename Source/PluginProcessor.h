#pragma once

#include <JuceHeader.h>

/**
 * Processore audio del plugin: possiede il sintetizzatore, il ValueTreeState
 * dei parametri esposti all'host e la tastiera MIDI usata dall'editor.
 */
class MyVST3PluginAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    MyVST3PluginAudioProcessor();
    ~MyVST3PluginAudioProcessor() override;

    //==============================================================================
    /** Chiamato dall'host prima della riproduzione: inoltra sample rate e
     *  dimensione del blocco al synth e alle singole voci. */
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    /** Il plugin accetta solo layout mono o stereo in uscita. */
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    /** Callback di rendering: elabora il buffer audio con i messaggi MIDI. */
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    /** Coda di riverbero virtuale: lascia spazio al rilascio dell'ADSR. */
    double getTailLengthSeconds() const override;

    //==============================================================================
    // Programmi non usati: il plugin espone un solo preset.
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    /** Salvataggio/caricamento dei valori dei parametri nel preset dell'host. */
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    /** Stato dei parametri (usato anche dall'editor per i binding). */
    juce::AudioProcessorValueTreeState parameters;
    /** Tastiera MIDI virtuale condivisa con l'editor. */
    juce::MidiKeyboardState keyboardState;

private:
    /** Definisce id, nome, range e valore di default di ogni parametro. */
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::Synthesiser synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyVST3PluginAudioProcessor)
};
