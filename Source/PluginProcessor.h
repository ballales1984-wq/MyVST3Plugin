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
    static const juce::String paramFilterType;
    static const juce::String paramFilterDrive;

    //==============================================================================
    // LFO parameters (simplified version)
    static const juce::String paramLfoRate;
    static const juce::String paramLfoWaveform;
    static const juce::String paramLfoAmount;
    static const juce::String paramLfoToOsc1;
    static const juce::String paramLfoToAmp;

    static const float filterCutoffMin;
    static const float filterCutoffMax;
    static const float filterCutoffDefault;

    static const float filterResonanceMin;
    static const float filterResonanceMax;
    static const float filterResonanceDefault;

    // Advanced filter parameters
    static const float filterTypeMin;
    static const float filterTypeMax;
    static const float filterTypeDefault;

    static const float filterDriveMin;
    static const float filterDriveMax;
    static const float filterDriveDefault;

    static const float lfoRateMin;      // 0.1 Hz
    static const float lfoRateMax;      // 20.0 Hz
    static const float lfoRateDefault;  // 1.0 Hz

    static const int lfoWaveformMin;      // 0 = Sine
    static const int lfoWaveformMax;      // 3 = Triangle
    static const int lfoWaveformDefault;  // 0 = Sine

    static const float lfoAmountMin;      // 0.0
    static const float lfoAmountMax;      // 1.0
    static const float lfoAmountDefault;  // 0.0 (no modulation)

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
    static const juce::String paramOsc1Waveform;  // NEW: Waveform selection for Osc1
    static const juce::String paramOsc2Waveform;  // NEW: Waveform selection for Osc2
    static const juce::String paramSquarePWM;     // NEW: Pulse Width Modulation for square wave
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

    // Waveform parameters
    static const float osc1WaveformMin;
    static const float osc1WaveformMax;
    static const float osc1WaveformDefault;
    static const float osc2WaveformMin;
    static const float osc2WaveformMax;
    static const float osc2WaveformDefault;

    // Square PWM parameters
    static const float squarePWMMin;
    static const float squarePWMax;
    static const float squarePWMDefault;

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

    // LFO for modulation
    juce::dsp::Oscillator<float> lfoOscillator;
    int lfoWaveform = 0; // 0: Sine, 1: Triangle, 2: Square, 3: Saw

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
    void updateOscillatorWaveform(juce::dsp::Oscillator<float>& oscillator, int waveformType, float pwmValue = 0.5f);

    // LFO helper methods
    void updateLFO();
    void setLFOWaveform(int waveformType);
    float getLFOValue();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyVST3PluginAudioProcessor)
};