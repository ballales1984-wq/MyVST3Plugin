#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Parameter definitions
const juce::String MyVST3PluginAudioProcessor::paramMasterVolume = "masterVolume";
const juce::String MyVST3PluginAudioProcessor::paramOsc1Frequency = "osc1Frequency";
const juce::String MyVST3PluginAudioProcessor::paramOsc2Frequency = "osc2Frequency";
const juce::String MyVST3PluginAudioProcessor::paramOsc2Detune = "osc2Detune";  // NEW: Detune parameter
const juce::String MyVST3PluginAudioProcessor::paramOsc1Waveform = "osc1Waveform";  // NEW: Waveform for Osc1
const juce::String MyVST3PluginAudioProcessor::paramOsc2Waveform = "osc2Waveform";  // NEW: Waveform for Osc2
const juce::String MyVST3PluginAudioProcessor::paramSquarePWM = "squarePWM";  // NEW: Pulse Width Modulation for square wave
const juce::String MyVST3PluginAudioProcessor::paramAttack = "attack";
const juce::String MyVST3PluginAudioProcessor::paramDecay = "decay";
const juce::String MyVST3PluginAudioProcessor::paramSustain = "sustain";
const juce::String MyVST3PluginAudioProcessor::paramRelease = "release";
const juce::String MyVST3PluginAudioProcessor::paramTestMode = "testMode";

// Filter parameters
const juce::String MyVST3PluginAudioProcessor::paramFilterCutoff = "filterCutoff";
const juce::String MyVST3PluginAudioProcessor::paramFilterResonance = "filterResonance";

// LFO parameters
const juce::String MyVST3PluginAudioProcessor::paramLfoRate = "lfoRate";
const juce::String MyVST3PluginAudioProcessor::paramLfoWaveform = "lfoWaveform";
const juce::String MyVST3PluginAudioProcessor::paramLfoAmount = "lfoAmount";
const juce::String MyVST3PluginAudioProcessor::paramLfoToOsc1 = "lfoToOsc1";
const juce::String MyVST3PluginAudioProcessor::paramLfoToAmp = "lfoToAmp";

// Parameter ranges
const float MyVST3PluginAudioProcessor::masterVolumeMin = 0.0f;
const float MyVST3PluginAudioProcessor::masterVolumeMax = 1.0f;
const float MyVST3PluginAudioProcessor::masterVolumeDefault = 0.8f;

const float MyVST3PluginAudioProcessor::oscFrequencyMin = 20.0f;
const float MyVST3PluginAudioProcessor::oscFrequencyMax = 20000.0f;
const float MyVST3PluginAudioProcessor::osc1FrequencyDefault = 440.0f;
const float MyVST3PluginAudioProcessor::osc2FrequencyDefault = 220.0f;

// Osc2 detune parameters (in cents, for chorus effect)
const float MyVST3PluginAudioProcessor::osc2DetuneMin = -50.0f;      // -50 cents (down)
const float MyVST3PluginAudioProcessor::osc2DetuneMax = 50.0f;       // +50 cents (up)
const float MyVST3PluginAudioProcessor::osc2DetuneDefault = 0.0f;    // No detune by default

// Waveform parameters (0=sine, 1=square, 2=saw, 3=triangle)
const float MyVST3PluginAudioProcessor::osc1WaveformMin = 0.0f;
const float MyVST3PluginAudioProcessor::osc1WaveformMax = 3.0f;
const float MyVST3PluginAudioProcessor::osc1WaveformDefault = 0.0f;  // Sine wave default

const float MyVST3PluginAudioProcessor::osc2WaveformMin = 0.0f;
const float MyVST3PluginAudioProcessor::osc2WaveformMax = 3.0f;
const float MyVST3PluginAudioProcessor::osc2WaveformDefault = 1.0f;  // Square wave default

// Square PWM parameters (0.1 = 10% duty cycle, 0.9 = 90% duty cycle)
const float MyVST3PluginAudioProcessor::squarePWMMin = 0.1f;      // 10% minimum
const float MyVST3PluginAudioProcessor::squarePWMax = 0.9f;       // 90% maximum
const float MyVST3PluginAudioProcessor::squarePWMDefault = 0.5f;  // 50% default (standard square)

const float MyVST3PluginAudioProcessor::adsrMin = 0.001f;
const float MyVST3PluginAudioProcessor::adsrMax = 5.0f;
const float MyVST3PluginAudioProcessor::attackDefault = 0.01f;
const float MyVST3PluginAudioProcessor::decayDefault = 0.2f;
const float MyVST3PluginAudioProcessor::sustainDefault = 0.8f;
const float MyVST3PluginAudioProcessor::releaseDefault = 0.01f;
const float MyVST3PluginAudioProcessor::testModeDefault = 0.0f;

// Filter parameter ranges
const float MyVST3PluginAudioProcessor::filterCutoffMin = 20.0f;
const float MyVST3PluginAudioProcessor::filterCutoffMax = 20000.0f;
const float MyVST3PluginAudioProcessor::filterCutoffDefault = 10000.0f;

const float MyVST3PluginAudioProcessor::filterResonanceMin = 0.1f;
const float MyVST3PluginAudioProcessor::filterResonanceMax = 10.0f;
const float MyVST3PluginAudioProcessor::filterResonanceDefault = 0.707f;

// LFO parameter ranges
const float MyVST3PluginAudioProcessor::lfoRateMin = 0.1f;      // 0.1 Hz (very slow)
const float MyVST3PluginAudioProcessor::lfoRateMax = 20.0f;     // 20.0 Hz (fast)
const float MyVST3PluginAudioProcessor::lfoRateDefault = 1.0f;  // 1.0 Hz default

const int MyVST3PluginAudioProcessor::lfoWaveformMin = 0;      // 0 = Sine
const int MyVST3PluginAudioProcessor::lfoWaveformMax = 3;      // 3 = Saw
const int MyVST3PluginAudioProcessor::lfoWaveformDefault = 0;  // Sine default

const float MyVST3PluginAudioProcessor::lfoAmountMin = 0.0f;      // 0% modulation
const float MyVST3PluginAudioProcessor::lfoAmountMax = 1.0f;     // 100% modulation
const float MyVST3PluginAudioProcessor::lfoAmountDefault = 0.0f; // No modulation default

//==============================================================================
MyVST3PluginAudioProcessor::MyVST3PluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
        ),
    parameters(*this, nullptr, juce::Identifier("MyVST3Plugin_FINAL"),
               {
                   std::make_unique<juce::AudioParameterFloat>(paramMasterVolume, "Master Volume",
                                                               masterVolumeMin, masterVolumeMax, masterVolumeDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramOsc1Frequency, "Osc1 Frequency",
                                                               oscFrequencyMin, oscFrequencyMax, osc1FrequencyDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramOsc2Frequency, "Osc2 Frequency",
                                                               oscFrequencyMin, oscFrequencyMax, osc2FrequencyDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramOsc2Detune, "Osc2 Detune",
                                                               osc2DetuneMin, osc2DetuneMax, osc2DetuneDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramOsc1Waveform, "Osc1 Waveform",
                                                               osc1WaveformMin, osc1WaveformMax, osc1WaveformDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramOsc2Waveform, "Osc2 Waveform",
                                                               osc2WaveformMin, osc2WaveformMax, osc2WaveformDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramSquarePWM, "Square PWM",
                                                               squarePWMMin, squarePWMax, squarePWMDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramAttack, "Attack",
                                                               adsrMin, adsrMax, attackDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramDecay, "Decay",
                                                               adsrMin, adsrMax, decayDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramSustain, "Sustain",
                                                               0.0f, 1.0f, sustainDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramRelease, "Release",
                                                               adsrMin, adsrMax, releaseDefault),
                   std::make_unique<juce::AudioParameterBool>(paramTestMode, "Test Mode",
                                                              testModeDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramFilterCutoff, "Filter Cutoff",
                                                               filterCutoffMin, filterCutoffMax, filterCutoffDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramFilterResonance, "Filter Resonance",
                                                               filterResonanceMin, filterResonanceMax, filterResonanceDefault),
                   // LFO Parameters
                   std::make_unique<juce::AudioParameterFloat>(paramLfoRate, "LFO Rate",
                                                               lfoRateMin, lfoRateMax, lfoRateDefault),
                   std::make_unique<juce::AudioParameterChoice>(paramLfoWaveform, "LFO Waveform",
                                                               juce::StringArray("Sine", "Triangle", "Square", "Saw"),
                                                               lfoWaveformDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramLfoAmount, "LFO Amount",
                                                               lfoAmountMin, lfoAmountMax, lfoAmountDefault),
                   std::make_unique<juce::AudioParameterBool>(paramLfoToOsc1, "LFO to Osc1", false),
                   std::make_unique<juce::AudioParameterBool>(paramLfoToAmp, "LFO to Amp", false)
               }),
    keyboardState(),
    midiCollector()
#endif
{
    // Plugin initialized successfully
}

MyVST3PluginAudioProcessor::~MyVST3PluginAudioProcessor()
{
}

//==============================================================================
const juce::String MyVST3PluginAudioProcessor::getName() const
{
    return "MyVST3Plugin_FINAL";
}

bool MyVST3PluginAudioProcessor::acceptsMidi() const
{
    return true;
}

bool MyVST3PluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MyVST3PluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double MyVST3PluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MyVST3PluginAudioProcessor::getNumPrograms()
{
    return 1;
}

int MyVST3PluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MyVST3PluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String MyVST3PluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return "Default";
}

void MyVST3PluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void MyVST3PluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    midiCollector.reset (sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    oscillator1.prepare(spec);
    oscillator2.prepare(spec);

    adsr.setSampleRate(sampleRate);

    // Initialize oscillators with default waveforms
    updateOscillators();

    // Initialize filters
    lowPassFilter.prepare(spec);
    lowPassFilterR.prepare(spec);
    updateFilter();

    currentMidiNote = -1;
    noteOn = false;

    updateOscillators();
    updateADSR();
}

void MyVST3PluginAudioProcessor::releaseResources()
{
}

bool MyVST3PluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void MyVST3PluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Process MIDI messages from virtual keyboard
    keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);

    // Process incoming MIDI messages
    for (const auto metadata : midiMessages)
    {
        const auto message = metadata.getMessage();
        handleMidiMessage(message);
    }

    // Update parameters
    updateOscillators();
    updateADSR();

    // Clear buffer
    buffer.clear();

    // Generate audio when we have an active MIDI note OR when test mode is enabled
    bool shouldGenerateAudio = (noteOn && currentMidiNote >= 0) || (parameters.getRawParameterValue(paramTestMode)->load() > 0.0f);

    if (shouldGenerateAudio)
    {
        const auto masterVolume = parameters.getRawParameterValue(paramMasterVolume);

        // LFO modulation (simplified)
        updateLFO();
        float lfoValue = getLFOValue();
        float lfoAmount = parameters.getRawParameterValue(paramLfoAmount)->load();

        // Apply LFO to Osc1 frequency if enabled
        bool lfoToOsc1 = parameters.getRawParameterValue(paramLfoToOsc1)->load() > 0.5f;
        if (lfoToOsc1)
        {
            float baseFreq = parameters.getRawParameterValue(paramOsc1Frequency)->load();
            float modulatedFreq = baseFreq * (1.0f + lfoValue * lfoAmount * 0.1f); // ±10% modulation
            oscillator1.setFrequency(modulatedFreq);
        }
        else
        {
            // Normal frequency update
            updateOscillators();
        }

        // Update Osc2 (with detune but no LFO for now)
        float baseOsc2Freq = parameters.getRawParameterValue(paramOsc2Frequency)->load();
        const auto detuneParam = parameters.getRawParameterValue(paramOsc2Detune);
        float detuneCents = detuneParam->load();
        float detuneRatio = std::pow(2.0f, detuneCents / 1200.0f);
        oscillator2.setFrequency(baseOsc2Freq * detuneRatio);

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                float osc1Sample = oscillator1.processSample(0.0f) * osc1Mix;
                float osc2Sample = oscillator2.processSample(0.0f) * osc2Mix;

                float envelopeValue = adsr.getNextSample();
                // Note: ADSR doesn't have getCurrentLevel() in JUCE
                currentEnvelopeLevel = envelopeValue;

                // Apply amplitude modulation if enabled
                bool lfoToAmp = parameters.getRawParameterValue(paramLfoToAmp)->load() > 0.5f;
                float ampModulation = 1.0f;
                if (lfoToAmp)
                {
                    // LFO value (-1 to 1) -> modulation (0.5 to 1.5)
                    ampModulation = 1.0f + (lfoValue * lfoAmount * 0.5f);
                }

                float finalSample = (osc1Sample + osc2Sample) * envelopeValue * masterVolume->load() * ampModulation;
                finalSample = juce::jlimit(-0.9f, 0.9f, finalSample);

                // Apply filter
                if (channel == 0)
                    finalSample = lowPassFilter.processSample(finalSample);
                else
                    finalSample = lowPassFilterR.processSample(finalSample);

                channelData[sample] = finalSample;
            }
        }
    }
    else
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float envelopeSample = adsr.getNextSample();
            currentEnvelopeLevel = envelopeSample;
        }

        if (!adsr.isActive() && currentMidiNote >= 0)
        {
            currentMidiNote = -1;
            currentFrequency = 440.0;
        }
    }
}

//==============================================================================
bool MyVST3PluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* MyVST3PluginAudioProcessor::createEditor()
{
    return new MyVST3PluginAudioProcessorEditor (*this);
}

//==============================================================================
void MyVST3PluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MyVST3PluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
void MyVST3PluginAudioProcessor::updateOscillators()
{
    float osc1Freq;

    if (noteOn && currentMidiNote >= 0)
    {
        osc1Freq = currentFrequency;
        oscillator1.setFrequency(osc1Freq);
    }
    else
    {
        const auto osc1FreqParam = parameters.getRawParameterValue(paramOsc1Frequency);
        osc1Freq = osc1FreqParam->load();
        oscillator1.setFrequency(osc1Freq);
    }

    // Get Osc1 waveform and PWM
    const auto osc1WaveformParam = parameters.getRawParameterValue(paramOsc1Waveform);
    int osc1Waveform = static_cast<int>(osc1WaveformParam->load());

    const auto pwmParam = parameters.getRawParameterValue(paramSquarePWM);
    float pwmValue = pwmParam->load();

    updateOscillatorWaveform(oscillator1, osc1Waveform, pwmValue);

    // Get Osc2 base frequency
    const auto osc2FreqParam = parameters.getRawParameterValue(paramOsc2Frequency);
    float osc2Freq = osc2FreqParam->load();

    // Apply detune to Osc2 based on Osc1 frequency
    const auto detuneParam = parameters.getRawParameterValue(paramOsc2Detune);
    float detuneCents = detuneParam->load();

    // Convert cents to frequency ratio: ratio = 2^(cents/1200)
    float detuneRatio = std::pow(2.0f, detuneCents / 1200.0f);

    // Apply detune relative to Osc1 frequency (for chorus effect)
    // This creates a natural detuning effect
    float detunedOsc2Freq = osc1Freq * detuneRatio;

    oscillator2.setFrequency(detunedOsc2Freq);

    // Get Osc2 waveform (PWM applies to square waves only)
    const auto osc2WaveformParam = parameters.getRawParameterValue(paramOsc2Waveform);
    int osc2Waveform = static_cast<int>(osc2WaveformParam->load());
    updateOscillatorWaveform(oscillator2, osc2Waveform, pwmValue);
}

void MyVST3PluginAudioProcessor::updateADSR()
{
    const auto attack = parameters.getRawParameterValue(paramAttack);
    const auto decay = parameters.getRawParameterValue(paramDecay);
    const auto sustain = parameters.getRawParameterValue(paramSustain);
    const auto release = parameters.getRawParameterValue(paramRelease);

    juce::ADSR::Parameters adsrParams;
    adsrParams.attack = attack->load();
    adsrParams.decay = decay->load();
    adsrParams.sustain = sustain->load();
    adsrParams.release = release->load();

    adsr.setParameters(adsrParams);
}

void MyVST3PluginAudioProcessor::updateOscillatorWaveform(juce::dsp::Oscillator<float>& oscillator, int waveformType, float pwmValue)
{
    switch (waveformType)
    {
        case 0: // Sine wave
            oscillator.initialise([](float x) { return std::sin(x); }, 128);
            break;
        case 1: // Square wave with PWM
            oscillator.initialise([pwmValue](float x) {
                // Normalize phase to 0-1 range
                float normalizedPhase = std::fmod(x / juce::MathConstants<float>::twoPi, 1.0f);
                // PWM: high for pwmValue portion of cycle, low for rest
                return normalizedPhase < pwmValue ? 1.0f : -1.0f;
            }, 128);
            break;
        case 2: // Saw wave
            oscillator.initialise([](float x) { return x / juce::MathConstants<float>::pi; }, 128);
            break;
        case 3: // Triangle wave
            oscillator.initialise([](float x) {
                float normalized = x / juce::MathConstants<float>::twoPi;
                float phase = normalized - std::floor(normalized);
                return phase < 0.5f ? (4.0f * phase - 1.0f) : (3.0f - 4.0f * phase);
            }, 128);
            break;
        default:
            // Default to sine wave
            oscillator.initialise([](float x) { return std::sin(x); }, 128);
            break;
    }
}


void MyVST3PluginAudioProcessor::updateFilter()
{
    const auto cutoff = parameters.getRawParameterValue(paramFilterCutoff);
    const auto resonance = parameters.getRawParameterValue(paramFilterResonance);

    // Create filter coefficients for low-pass filter
    juce::dsp::IIR::Coefficients<float>::Ptr coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, cutoff->load(), resonance->load());

    lowPassFilter.coefficients = coefficients;
    lowPassFilterR.coefficients = coefficients;
}

double MyVST3PluginAudioProcessor::midiNoteToFrequency(int midiNote)
{
    return 440.0 * std::pow(2.0, (midiNote - 69) / 12.0);
}

void MyVST3PluginAudioProcessor::handleMidiMessage(const juce::MidiMessage& message)
{
    if (message.isNoteOn() && message.getVelocity() > 0)
    {
        currentMidiNote = message.getNoteNumber();
        currentFrequency = midiNoteToFrequency(currentMidiNote);
        noteOn = true;
        adsr.noteOn();
    }
    else if (message.isNoteOff() || (message.isNoteOn() && message.getVelocity() == 0))
    {
        if (message.getNoteNumber() == currentMidiNote)
        {
            noteOn = false;
            adsr.noteOff();
        }
    }
}

//==============================================================================
// LFO Methods (Simplified)
//==============================================================================

void MyVST3PluginAudioProcessor::updateLFO()
{
    // Update LFO rate
    const auto lfoRateParam = parameters.getRawParameterValue(paramLfoRate);
    float lfoRate = lfoRateParam->load();
    lfoOscillator.setFrequency(lfoRate);

    // Update LFO waveform
    lfoWaveform = static_cast<int>(parameters.getRawParameterValue(paramLfoWaveform)->load());
    setLFOWaveform(lfoWaveform);
}

void MyVST3PluginAudioProcessor::setLFOWaveform(int waveformType)
{
    switch (waveformType)
    {
        case 0: // Sine
            lfoOscillator.initialise([](float x) { return std::sin(x); }, 128);
            break;
        case 1: // Triangle
            lfoOscillator.initialise([](float x) {
                return std::asin(std::sin(x)) * (2.0f / juce::MathConstants<float>::pi);
            }, 128);
            break;
        case 2: // Square
            lfoOscillator.initialise([](float x) {
                return x > 0.0f ? 1.0f : -1.0f;
            }, 128);
            break;
        case 3: // Saw
            lfoOscillator.initialise([](float x) {
                return x / juce::MathConstants<float>::pi;
            }, 128);
            break;
        default: // Default to Sine
            lfoOscillator.initialise([](float x) { return std::sin(x); }, 128);
            break;
    }
}

float MyVST3PluginAudioProcessor::getLFOValue()
{
    return lfoOscillator.processSample(0.0f); // Mono LFO
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyVST3PluginAudioProcessor();
}
