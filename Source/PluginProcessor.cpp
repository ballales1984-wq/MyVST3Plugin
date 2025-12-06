#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyVST3PluginAudioProcessor::MyVST3PluginAudioProcessor()
    : parameters(*this, nullptr, "PARAMETERS",
        {
            std::make_unique<juce::AudioParameterBool>("testMode", "Test Mode", false)
        })
{
    synth.addSound(new juce::SamplerSound("Default", nullptr, 0, 0, 0, 0));
}

MyVST3PluginAudioProcessor::~MyVST3PluginAudioProcessor()
{
}

//==============================================================================
void MyVST3PluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void MyVST3PluginAudioProcessor::releaseResources()
{
}

void MyVST3PluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Process MIDI messages from virtual keyboard
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    // Process incoming MIDI messages
    for (const auto metadata : midiMessages)
    {
        const auto message = metadata.getMessage();
        if (message.isNoteOn())
        {
            noteOn = true;
            currentMidiNote = message.getNoteNumber();
        }
        else if (message.isNoteOff())
        {
            noteOn = false;
            currentMidiNote = -1;
        }
    }

    // Clear buffer
    buffer.clear();

    // Simple test tone if test mode is enabled
    auto* testModeParam = parameters.getRawParameterValue("testMode");
    if (testModeParam && testModeParam->load() > 0.0f)
    {
        static double phase = 0.0;
        const double sampleRate = getSampleRate();
        const double frequency = 440.0;

        if (sampleRate > 0.0)
        {
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    channelData[sample] = (float)std::sin(phase) * 0.1f;
                    phase += (frequency * 2.0 * juce::MathConstants<double>::pi) / sampleRate;
                }
            }

            if (phase >= 2.0 * juce::MathConstants<double>::pi)
                phase -= 2.0 * juce::MathConstants<double>::pi;
        }
    }
}

//==============================================================================
juce::AudioProcessorEditor* MyVST3PluginAudioProcessor::createEditor()
{
    return new MyVST3PluginAudioProcessorEditor (*this);
}

bool MyVST3PluginAudioProcessor::hasEditor() const
{
    return true;
}

//==============================================================================
const juce::String MyVST3PluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MyVST3PluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
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
}

const juce::String MyVST3PluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void MyVST3PluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MyVST3PluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void MyVST3PluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto xml = getXmlFromBinary (data, sizeInBytes);
    if (xml != nullptr)
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyVST3PluginAudioProcessor();
}
