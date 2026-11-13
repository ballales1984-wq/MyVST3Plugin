#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthVoice.h"

//==============================================================================
MyVST3PluginAudioProcessor::MyVST3PluginAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, juce::Identifier ("PARAMETERS"), createParameterLayout())
{
    for (int i = 0; i < 8; ++i)
        synth.addVoice (new SynthVoice (parameters));
    synth.addSound (new SynthSound());
}

MyVST3PluginAudioProcessor::~MyVST3PluginAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout MyVST3PluginAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    using APF = juce::AudioParameterFloat;
    using APC = juce::AudioParameterChoice;
    auto id = [] (const char* s) { return juce::ParameterID (s, 1); };

    juce::StringArray waves { "Sine", "Square", "Saw", "Triangle" };
    juce::StringArray lfoTargets { "Pitch", "Filter", "Amp" };

    p.push_back (std::make_unique<APC> (id ("osc1Wave"), "Osc1 Wave", waves, 0));
    p.push_back (std::make_unique<APC> (id ("osc2Wave"), "Osc2 Wave", waves, 2));
    p.push_back (std::make_unique<APF> (id ("osc1Level"), "Osc1 Level", juce::NormalisableRange<float> (0.0f, 1.0f), 0.8f));
    p.push_back (std::make_unique<APF> (id ("osc2Level"), "Osc2 Level", juce::NormalisableRange<float> (0.0f, 1.0f), 0.6f));
    p.push_back (std::make_unique<APF> (id ("osc2Detune"), "Osc2 Detune (cents)", juce::NormalisableRange<float> (-50.0f, 50.0f), 5.0f));

    p.push_back (std::make_unique<APF> (id ("attack"), "Attack", juce::NormalisableRange<float> (0.001f, 2.0f, 0.0f, 0.35f), 0.01f));
    p.push_back (std::make_unique<APF> (id ("decay"), "Decay", juce::NormalisableRange<float> (0.001f, 2.0f, 0.0f, 0.35f), 0.2f));
    p.push_back (std::make_unique<APF> (id ("sustain"), "Sustain", juce::NormalisableRange<float> (0.0f, 1.0f), 0.7f));
    p.push_back (std::make_unique<APF> (id ("release"), "Release", juce::NormalisableRange<float> (0.001f, 4.0f, 0.0f, 0.35f), 0.3f));

    p.push_back (std::make_unique<APF> (id ("cutoff"), "Filter Cutoff", juce::NormalisableRange<float> (20.0f, 18000.0f, 0.0f, 0.3f), 8000.0f));
    p.push_back (std::make_unique<APF> (id ("resonance"), "Filter Resonance", juce::NormalisableRange<float> (0.0f, 1.0f), 0.2f));
    p.push_back (std::make_unique<APF> (id ("filterEnv"), "Filter Env Amount", juce::NormalisableRange<float> (0.0f, 8000.0f, 0.0f, 0.35f), 0.0f));

    p.push_back (std::make_unique<APF> (id ("lfoRate"), "LFO Rate", juce::NormalisableRange<float> (0.1f, 20.0f, 0.0f, 0.35f), 2.0f));
    p.push_back (std::make_unique<APF> (id ("lfoDepth"), "LFO Depth", juce::NormalisableRange<float> (0.0f, 1.0f), 0.0f));
    p.push_back (std::make_unique<APC> (id ("lfoTarget"), "LFO Target", lfoTargets, 1));

    p.push_back (std::make_unique<APF> (id ("gain"), "Master Gain", juce::NormalisableRange<float> (0.0f, 1.0f), 0.7f));

    return { p.begin(), p.end() };
}

//==============================================================================
void MyVST3PluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<SynthVoice*> (synth.getVoice (i)))
            v->prepareToPlay (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void MyVST3PluginAudioProcessor::releaseResources()
{
}

bool MyVST3PluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void MyVST3PluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
juce::AudioProcessorEditor* MyVST3PluginAudioProcessor::createEditor()
{
    return new MyVST3PluginAudioProcessorEditor (*this);
}

bool MyVST3PluginAudioProcessor::hasEditor() const { return true; }

//==============================================================================
const juce::String MyVST3PluginAudioProcessor::getName() const { return JucePlugin_Name; }
bool MyVST3PluginAudioProcessor::acceptsMidi() const { return true; }
bool MyVST3PluginAudioProcessor::producesMidi() const { return false; }
bool MyVST3PluginAudioProcessor::isMidiEffect() const { return false; }
double MyVST3PluginAudioProcessor::getTailLengthSeconds() const { return 2.0; }

int MyVST3PluginAudioProcessor::getNumPrograms() { return 1; }
int MyVST3PluginAudioProcessor::getCurrentProgram() { return 0; }
void MyVST3PluginAudioProcessor::setCurrentProgram (int) {}
const juce::String MyVST3PluginAudioProcessor::getProgramName (int) { return {}; }
void MyVST3PluginAudioProcessor::changeProgramName (int, const juce::String&) {}

//==============================================================================
void MyVST3PluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = parameters.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void MyVST3PluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyVST3PluginAudioProcessor();
}
