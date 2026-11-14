#pragma once

#include <JuceHeader.h>

/**
 * Processore audio del plugin: possiede il sintetizzatore, il ValueTreeState
 * dei parametri esposti all'host e la tastiera MIDI usata dall'editor.
 * Include una catena di effetti: EQ 3 bande, Delay stereo, Riverbero.
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

    // --- Catena effetti ---
    void prepareEffects (double sampleRate, int samplesPerBlock);
    void processEffects (juce::AudioBuffer<float>& buffer);

    /** EQ 3 bande: low shelf, mid peak, high shelf. */
    struct EqBand
    {
        juce::dsp::IIR::Filter<float> filter;
        // Guardia: ricalcola i coefficienti solo se guadagno o SR cambiano.
        float appliedGainDb = 1.0e9f;
        double appliedFs = 0.0;

        void prepare (double sampleRate);
        void setLowShelf  (double sampleRate, float freq, float gainDb);
        void setPeak      (double sampleRate, float freq, float gainDb, float q);
        void setHighShelf (double sampleRate, float freq, float gainDb);
    };

    EqBand eqLow, eqMid, eqHigh;

    // Smoothing dei parametri FX (anti-zipper): one-pole a blocco per EQ/Reverb,
    // SmoothedValue per-campione per il mix/feedback del delay.
    float eqLowSmoothDb = 0.0f, eqMidSmoothDb = 0.0f, eqHighSmoothDb = 0.0f;
    float reverbMixSmooth = 0.0f, reverbSizeSmooth = 0.5f, reverbDampSmooth = 0.5f;
    float delayTimeSmooth = 0.0f;
    juce::SmoothedValue<float> delayMixSmooth, delayFeedbackSmooth;

    /** Delay stereo con feedback e cross-feedback per width. */
    juce::dsp::DelayLine<float> delayL, delayR;
    float delayFeedback = 0.3f;
    float delayMix = 0.0f;
    float delayTimeSamples = 0.0f;
    int maxDelaySamples = 96000; // ~2s a 48kHz

    /** Riverbero Schroeder (JUCE Reverb). */
    juce::Reverb reverb;
    float reverbMix = 0.0f;

    double currentSampleRate = 44100.0;

    // Compensazione di guadagno in funzione del numero di voci attive.
    juce::SmoothedValue<float> polyGain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyVST3PluginAudioProcessor)
};