#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
/**
    Dual Oscillator VST3 Plugin Processor

    Features:
    - Two independent oscillators (sine + square wave)
    - Master volume control
    - Individual oscillator frequency controls
    - ADSR envelope (Attack, Decay, Sustain, Release)
    - MIDI keyboard support
*/
class MyVST3PluginAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    MyVST3PluginAudioProcessor();
    ~MyVST3PluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    // Public access to parameters and MIDI state
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }

    // MIDI state access for GUI
    bool noteOn = false;
    int currentMidiNote = -1;

    // ADSR envelope level for GUI display
    float currentEnvelopeLevel = 0.0f;

    // Keyboard state for UI interaction
    juce::MidiKeyboardState keyboardState;
    juce::MidiMessageCollector midiCollector;

    //==============================================================================
    // Filter parameters
    static const juce::String paramFilterCutoff;
    static const juce::String paramFilterResonance;

    static const float filterCutoffMin;
    static const float filterCutoffMax;
    static const float filterCutoffDefault;

    static const float filterResonanceMin;
    static const float filterResonanceMax;
    static const float filterResonanceDefault;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter IDs
    static const juce::String paramMasterVolume;
    static const juce::String paramOsc1Frequency;
    static const juce::String paramOsc2Frequency;
    static const juce::String paramOsc2Detune;  // NEW: Detune parameter for Osc2
    static const juce::String paramAttack;
    static const juce::String paramDecay;
    static const juce::String paramSustain;
    static const juce::String paramRelease;
    static const juce::String paramTestMode;

    // Parameter ranges
    static const float masterVolumeMin;
    static const float masterVolumeMax;
    static const float masterVolumeDefault;

    static const float oscFrequencyMin;
    static const float oscFrequencyMax;
    static const float osc1FrequencyDefault;
    static const float osc2FrequencyDefault;
    static const float osc2DetuneMin;
    static const float osc2DetuneMax;
    static const float osc2DetuneDefault;

    static const float adsrMin;
    static const float adsrMax;
    static const float attackDefault;
    static const float decayDefault;
    static const float sustainDefault;
    static const float releaseDefault;
    static const float testModeDefault;

private:
    //==============================================================================
    // Audio processor value tree state for managing parameters
    juce::AudioProcessorValueTreeState parameters;

    //==============================================================================
    // MIDI and synthesis state
    double currentFrequency = 440.0;
    float osc1Mix = 1.0f; // Impostato a pieno volume
    float osc2Mix = 1.0f; // Impostato a pieno volume

    //==============================================================================
    // Helper methods for MIDI
    double midiNoteToFrequency(int midiNote);
    void handleMidiMessage(const juce::MidiMessage& message);

    // Oscillators
    juce::dsp::Oscillator<float> oscillator1; // Sine wave
    juce::dsp::Oscillator<float> oscillator2; // Square wave

    // ADSR envelope
    juce::ADSR adsr;

    // Low-pass filter
    juce::dsp::IIR::Filter<float> lowPassFilter;
    juce::dsp::IIR::Filter<float> lowPassFilterR;

    // Audio processing state
    double currentSampleRate = 44100.0;

    //==============================================================================
    // Helper methods
    void updateOscillators();
    void updateADSR();
    void updateFilter();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyVST3PluginAudioProcessor)
};