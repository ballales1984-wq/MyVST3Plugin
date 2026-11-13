#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthVoice.h"

//==============================================================================
// Costruttore: uscite stereo e 16 voci di sintesi polifoniche.
MyVST3PluginAudioProcessor::MyVST3PluginAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, juce::Identifier ("PARAMETERS"), createParameterLayout())
{
    for (int i = 0; i < 16; ++i)
        synth.addVoice (new SynthVoice (parameters));
    synth.addSound (new SynthSound());
}

MyVST3PluginAudioProcessor::~MyVST3PluginAudioProcessor() = default;

// Definizione dei parametri esposti all'host.
juce::AudioProcessorValueTreeState::ParameterLayout MyVST3PluginAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    using APF = juce::AudioParameterFloat;
    using APC = juce::AudioParameterChoice;
    auto id = [] (const char* s) { return juce::ParameterID (s, 1); };

    // Forme d'onda disponibili per entrambi gli oscillatori.
    juce::StringArray waves { "Sine", "Square", "Saw", "Triangle" };
    // Destinazioni modulabili dal LFO.
    juce::StringArray lfoTargets { "Pitch", "Filter", "Amp" };

    // --- Oscillatori ---
    p.push_back (std::make_unique<APC> (id ("osc1Wave"), "Osc1 Wave", waves, 0));
    p.push_back (std::make_unique<APC> (id ("osc2Wave"), "Osc2 Wave", waves, 2));
    p.push_back (std::make_unique<APF> (id ("osc1Level"), "Osc1 Level", juce::NormalisableRange<float> (0.0f, 1.0f), 0.8f));
    p.push_back (std::make_unique<APF> (id ("osc2Level"), "Osc2 Level", juce::NormalisableRange<float> (0.0f, 1.0f), 0.6f));
    p.push_back (std::make_unique<APF> (id ("osc2Detune"), "Osc2 Detune (cents)", juce::NormalisableRange<float> (-50.0f, 50.0f), 5.0f));

    // --- Unisono ---
    p.push_back (std::make_unique<juce::AudioParameterInt> (id ("unisonCount"), "Unison Voices", 1, 8, 2));
    p.push_back (std::make_unique<APF> (id ("unisonDetune"), "Unison Detune (cents)", juce::NormalisableRange<float> (0.0f, 50.0f), 7.0f));

    // --- Inviluppo ADSR (tempi in secondi, sustain in frazione lineare) ---
    p.push_back (std::make_unique<APF> (id ("attack"), "Attack", juce::NormalisableRange<float> (0.001f, 2.0f, 0.0f, 0.35f), 0.01f));
    p.push_back (std::make_unique<APF> (id ("decay"), "Decay", juce::NormalisableRange<float> (0.001f, 2.0f, 0.0f, 0.35f), 0.2f));
    p.push_back (std::make_unique<APF> (id ("sustain"), "Sustain", juce::NormalisableRange<float> (0.0f, 1.0f), 0.7f));
    p.push_back (std::make_unique<APF> (id ("release"), "Release", juce::NormalisableRange<float> (0.001f, 4.0f, 0.0f, 0.35f), 0.3f));

    // --- Filtro lowpass: cutoff in Hz, risonanza e quantita di inviluppo ---
    p.push_back (std::make_unique<APF> (id ("cutoff"), "Filter Cutoff", juce::NormalisableRange<float> (20.0f, 18000.0f, 0.0f, 0.3f), 8000.0f));
    p.push_back (std::make_unique<APF> (id ("resonance"), "Filter Resonance", juce::NormalisableRange<float> (0.0f, 1.0f), 0.2f));
    p.push_back (std::make_unique<APF> (id ("filterEnv"), "Filter Env Amount", juce::NormalisableRange<float> (0.0f, 8000.0f, 0.0f, 0.35f), 0.0f));

    // --- LFO ---
    p.push_back (std::make_unique<APF> (id ("lfoRate"), "LFO Rate", juce::NormalisableRange<float> (0.1f, 20.0f, 0.0f, 0.35f), 2.0f));
    p.push_back (std::make_unique<APF> (id ("lfoDepth"), "LFO Depth", juce::NormalisableRange<float> (0.0f, 1.0f), 0.0f));
    p.push_back (std::make_unique<APC> (id ("lfoTarget"), "LFO Target", lfoTargets, 1));

    // --- Uscita ---
    p.push_back (std::make_unique<APF> (id ("gain"), "Master Gain", juce::NormalisableRange<float> (0.0f, 1.0f), 0.7f));

    // --- EQ 3 bande (dB, -12..+12) ---
    p.push_back (std::make_unique<APF> (id ("eqLow"), "EQ Low", juce::NormalisableRange<float> (-12.0f, 12.0f), 0.0f));
    p.push_back (std::make_unique<APF> (id ("eqMid"), "EQ Mid", juce::NormalisableRange<float> (-12.0f, 12.0f), 0.0f));
    p.push_back (std::make_unique<APF> (id ("eqHigh"), "EQ High", juce::NormalisableRange<float> (-12.0f, 12.0f), 0.0f));

    // --- Delay stereo ---
    p.push_back (std::make_unique<APF> (id ("delayTime"), "Delay Time (s)", juce::NormalisableRange<float> (0.01f, 1.0f, 0.0f, 0.35f), 0.25f));
    p.push_back (std::make_unique<APF> (id ("delayFeedback"), "Delay Feedback", juce::NormalisableRange<float> (0.0f, 0.9f), 0.3f));
    p.push_back (std::make_unique<APF> (id ("delayMix"), "Delay Mix", juce::NormalisableRange<float> (0.0f, 1.0f), 0.0f));

    // --- Riverbero ---
    p.push_back (std::make_unique<APF> (id ("reverbSize"), "Reverb Size", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    p.push_back (std::make_unique<APF> (id ("reverbDamping"), "Reverb Damping", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    p.push_back (std::make_unique<APF> (id ("reverbMix"), "Reverb Mix", juce::NormalisableRange<float> (0.0f, 1.0f), 0.0f));

    return { p.begin(), p.end() };
}

//==============================================================================
void MyVST3PluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    synth.setCurrentPlaybackSampleRate (sampleRate);
    // Ogni voce riceve sample rate e numero di canali d'uscita.
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<SynthVoice*> (synth.getVoice (i)))
            v->prepareToPlay (sampleRate, samplesPerBlock, getTotalNumOutputChannels());

    prepareEffects (sampleRate, samplesPerBlock);
}

void MyVST3PluginAudioProcessor::releaseResources()
{
    reverb.reset();
}

// Supporto bus: solo mono o stereo.
bool MyVST3PluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

// =============================================================================
// EQ 3 bande - implementazione
// =============================================================================
void MyVST3PluginAudioProcessor::EqBand::prepare (double sampleRate)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.numChannels = 2;
    spec.maximumBlockSize = 512;

    filter.prepare (spec);
    filter.reset();
}

void MyVST3PluginAudioProcessor::EqBand::setLowShelf (double sampleRate, float freq, float gainDb)
{
    const double Q = 0.707;
    const double A = std::pow (10.0, gainDb * 0.05);
    const double w0 = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
    const double alpha = std::sin (w0) / (2.0 * Q);
    const double cosW0 = std::cos (w0);
    const double sqrtA = std::sqrt (A);

    const double b0 = A * ((A + 1) - (A - 1) * cosW0 + 2 * sqrtA * alpha);
    const double b1 = 2 * A * ((A - 1) - (A + 1) * cosW0);
    const double b2 = A * ((A + 1) - (A - 1) * cosW0 - 2 * sqrtA * alpha);
    const double a0 = (A + 1) + (A - 1) * cosW0 + 2 * sqrtA * alpha;
    const double a1 = -2 * ((A - 1) + (A + 1) * cosW0);
    const double a2 = (A + 1) + (A - 1) * cosW0 - 2 * sqrtA * alpha;

    filter.coefficients = new juce::dsp::IIR::Coefficients<float> (
        static_cast<float>(b0), static_cast<float>(b1), static_cast<float>(b2),
        static_cast<float>(a0), static_cast<float>(a1), static_cast<float>(a2));
}

void MyVST3PluginAudioProcessor::EqBand::setPeak (double sampleRate, float freq, float gainDb, float q)
{
    const double A = std::pow (10.0, gainDb * 0.05);
    const double w0 = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
    const double alpha = std::sin (w0) / (2.0 * q);
    const double cosW0 = std::cos (w0);

    const double b0 = 1 + alpha * A;
    const double b1 = -2 * cosW0;
    const double b2 = 1 - alpha * A;
    const double a0 = 1 + alpha / A;
    const double a1 = -2 * cosW0;
    const double a2 = 1 - alpha / A;

    filter.coefficients = new juce::dsp::IIR::Coefficients<float> (
        static_cast<float>(b0), static_cast<float>(b1), static_cast<float>(b2),
        static_cast<float>(a0), static_cast<float>(a1), static_cast<float>(a2));
}

void MyVST3PluginAudioProcessor::EqBand::setHighShelf (double sampleRate, float freq, float gainDb)
{
    const double Q = 0.707;
    const double A = std::pow (10.0, gainDb * 0.05);
    const double w0 = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
    const double alpha = std::sin (w0) / (2.0 * Q);
    const double cosW0 = std::cos (w0);
    const double sqrtA = std::sqrt (A);

    const double b0 = A * ((A + 1) + (A - 1) * cosW0 + 2 * sqrtA * alpha);
    const double b1 = -2 * A * ((A - 1) + (A + 1) * cosW0);
    const double b2 = A * ((A + 1) + (A - 1) * cosW0 - 2 * sqrtA * alpha);
    const double a0 = (A + 1) - (A - 1) * cosW0 + 2 * sqrtA * alpha;
    const double a1 = 2 * ((A - 1) - (A + 1) * cosW0);
    const double a2 = (A + 1) - (A - 1) * cosW0 - 2 * sqrtA * alpha;

    filter.coefficients = new juce::dsp::IIR::Coefficients<float> (
        static_cast<float>(b0), static_cast<float>(b1), static_cast<float>(b2),
        static_cast<float>(a0), static_cast<float>(a1), static_cast<float>(a2));
}

// =============================================================================
// Preparazione effetti
// =============================================================================
void MyVST3PluginAudioProcessor::prepareEffects (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.numChannels = 2;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);

    // EQ
    eqLow.prepare  (sampleRate);
    eqMid.prepare  (sampleRate);
    eqHigh.prepare (sampleRate);

    // Delay
    maxDelaySamples = static_cast<int>(sampleRate * 1.0); // 1s max
    delayL.setMaximumDelayInSamples (maxDelaySamples);
    delayR.setMaximumDelayInSamples (maxDelaySamples);
    delayL.prepare (spec);
    delayR.prepare (spec);

    // Reverb
    reverb.setSampleRate (sampleRate);
    reverb.setParameters (juce::Reverb::Parameters());
}

// =============================================================================
// Processing effetti
// =============================================================================
void MyVST3PluginAudioProcessor::processEffects (juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Leggi parametri
    auto getParam = [this] (const char* id) { return parameters.getRawParameterValue (id)->load(); };

    // --- EQ 3 bande ---
    const float eqLowDb  = getParam ("eqLow");
    const float eqMidDb  = getParam ("eqMid");
    const float eqHighDb = getParam ("eqHigh");

    // Aggiorna i coefficienti dei filtri
    eqLow.setLowShelf  (currentSampleRate, 200.0f,  eqLowDb);
    eqMid.setPeak      (currentSampleRate, 1000.0f, eqMidDb, 1.0f);
    eqHigh.setHighShelf(currentSampleRate, 4000.0f, eqHighDb);

    // Applica EQ (sempre, anche a guadagno 0 per linearita di fase)
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        eqLow.filter.process  (context);
        eqMid.filter.process  (context);
        eqHigh.filter.process (context);
    }

    // --- Delay stereo ---
    delayTimeSamples = static_cast<int> (getParam ("delayTime") * currentSampleRate);
    delayFeedback = getParam ("delayFeedback");
    delayMix = getParam ("delayMix");

    if (delayMix > 0.001f)
    {
        delayL.setDelay (delayTimeSamples);
        delayR.setDelay (delayTimeSamples);

        for (int i = 0; i < numSamples; ++i)
        {
            const float dryL = buffer.getSample (0, i);
            const float dryR = numChannels > 1 ? buffer.getSample (1, i) : dryL;

            const float wetL = delayL.popSample (0);
            const float wetR = delayR.popSample (1);

            // Cross-feedback per stereo width
            delayL.pushSample (0, dryL + wetL * delayFeedback * 0.5f + wetR * delayFeedback * 0.5f);
            delayR.pushSample (1, dryR + wetR * delayFeedback * 0.5f + wetL * delayFeedback * 0.5f);

            buffer.setSample (0, i, dryL * (1.0f - delayMix) + wetL * delayMix);
            if (numChannels > 1)
                buffer.setSample (1, i, dryR * (1.0f - delayMix) + wetR * delayMix);
        }
    }

    // --- Riverbero ---
    reverbMix = getParam ("reverbMix");
    if (reverbMix > 0.001f)
    {
        juce::Reverb::Parameters rp;
        rp.roomSize = getParam ("reverbSize");
        rp.damping = getParam ("reverbDamping");
        rp.wetLevel = reverbMix;
        rp.dryLevel = 1.0f - reverbMix;
        rp.width = 1.0f;
        reverb.setParameters (rp);

        if (numChannels >= 2)
        {
            reverb.processStereo (buffer.getWritePointer (0),
                                  buffer.getWritePointer (1),
                                  numSamples);
        }
    }
}

// =============================================================================
// processBlock
// =============================================================================
void MyVST3PluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // Aggiorna lo stato della tastiera virtuale, poi fa suonare il synth.
    keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    // Applica la catena di effetti
    processEffects (buffer);
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
// Serializzazione: i parametri vengono salvati come XML dentro il preset.
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
// Entry point richiesto dal framework JUCE per istanziare il plugin.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyVST3PluginAudioProcessor();
}