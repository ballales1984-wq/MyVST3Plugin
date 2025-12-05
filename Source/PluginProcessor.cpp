#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Parameter definitions
const juce::String MyVST3PluginAudioProcessor::paramMasterVolume = "masterVolume";
const juce::String MyVST3PluginAudioProcessor::paramOsc1Frequency = "osc1Frequency";
const juce::String MyVST3PluginAudioProcessor::paramOsc2Frequency = "osc2Frequency";
const juce::String MyVST3PluginAudioProcessor::paramAttack = "attack";
const juce::String MyVST3PluginAudioProcessor::paramDecay = "decay";
const juce::String MyVST3PluginAudioProcessor::paramSustain = "sustain";
const juce::String MyVST3PluginAudioProcessor::paramRelease = "release";
const juce::String MyVST3PluginAudioProcessor::paramTestMode = "testMode";

// Parameter ranges
const float MyVST3PluginAudioProcessor::masterVolumeMin = 0.0f;
const float MyVST3PluginAudioProcessor::masterVolumeMax = 1.0f;
const float MyVST3PluginAudioProcessor::masterVolumeDefault = 0.8f;

const float MyVST3PluginAudioProcessor::oscFrequencyMin = 20.0f;
const float MyVST3PluginAudioProcessor::oscFrequencyMax = 20000.0f;
const float MyVST3PluginAudioProcessor::osc1FrequencyDefault = 440.0f;
const float MyVST3PluginAudioProcessor::osc2FrequencyDefault = 220.0f;

const float MyVST3PluginAudioProcessor::adsrMin = 0.001f;
const float MyVST3PluginAudioProcessor::adsrMax = 5.0f;
const float MyVST3PluginAudioProcessor::attackDefault = 0.01f;
const float MyVST3PluginAudioProcessor::decayDefault = 0.2f;
const float MyVST3PluginAudioProcessor::sustainDefault = 0.8f;
const float MyVST3PluginAudioProcessor::releaseDefault = 0.01f;
const float MyVST3PluginAudioProcessor::testModeDefault = 0.0f;

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
                   std::make_unique<juce::AudioParameterFloat>(paramAttack, "Attack",
                                                               adsrMin, adsrMax, attackDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramDecay, "Decay",
                                                               adsrMin, adsrMax, decayDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramSustain, "Sustain",
                                                               0.0f, 1.0f, sustainDefault),
                   std::make_unique<juce::AudioParameterFloat>(paramRelease, "Release",
                                                               adsrMin, adsrMax, releaseDefault),
                   std::make_unique<juce::AudioParameterBool>(paramTestMode, "Test Mode",
                                                              testModeDefault)
               }),
    keyboardState(),
    midiCollector()
#endif
{
    DBG("MyVST3Plugin: PLUGIN CONSTRUCTOR CALLED - Plugin loaded successfully!");
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
    oscillator1.initialise([](float x) { return std::sin(x); }, 128);

    oscillator2.prepare(spec);
    oscillator2.initialise([](float x) { return x > 0.0f ? 1.0f : -1.0f; }, 128);

    adsr.setSampleRate(sampleRate);

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

                float finalSample = (osc1Sample + osc2Sample) * envelopeValue * masterVolume->load();
                finalSample = juce::jlimit(-0.9f, 0.9f, finalSample);

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
    if (noteOn && currentMidiNote >= 0)
    {
        oscillator1.setFrequency(currentFrequency);
    }
    else
    {
        const auto osc1Freq = parameters.getRawParameterValue(paramOsc1Frequency);
        oscillator1.setFrequency(osc1Freq->load());
    }

    const auto osc2Freq = parameters.getRawParameterValue(paramOsc2Frequency);
    oscillator2.setFrequency(osc2Freq->load());
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
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyVST3PluginAudioProcessor();
}
