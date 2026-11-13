#pragma once

#include <JuceHeader.h>

struct SynthSound : public juce::SynthesiserSound
{
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

class SynthVoice : public juce::SynthesiserVoice
{
public:
    explicit SynthVoice (juce::AudioProcessorValueTreeState& apvts) : params (apvts) {}

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override
    {
        currentNote = midiNoteNumber;
        level = juce::jlimit (0.0f, 1.0f, velocity);
        frequency = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        phase1 = 0.0;
        phase2 = 0.0;
        lfoPhase = 0.0;

        updateParams();
        adsr.noteOn();
    }

    void stopNote (float, bool allowTailOff) override
    {
        if (allowTailOff)
            adsr.noteOff();
        else
        {
            clearCurrentNote();
            adsr.reset();
        }
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void prepareToPlay (double sampleRate, int, int)
    {
        currentSampleRate = sampleRate;
        adsr.setSampleRate (sampleRate);

        juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (512), static_cast<juce::uint32> (1) };
        filter.prepare (spec);
        filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        filter.reset();

        updateParams();
    }

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (! isVoiceActive())
            return;

        updateParams();

        auto* outL = outputBuffer.getWritePointer (0, startSample);
        auto* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;

        for (int i = 0; i < numSamples; ++i)
        {
            if (currentSampleRate <= 0.0)
                break;

            const float lfo = static_cast<float> (std::sin (lfoPhase)) * lfoDepth;
            lfoPhase += (2.0 * juce::MathConstants<double>::pi * lfoRate) / currentSampleRate;
            if (lfoPhase >= 2.0 * juce::MathConstants<double>::pi)
                lfoPhase -= 2.0 * juce::MathConstants<double>::pi;

            const double freqMod = frequency * std::pow (2.0, (static_cast<double> (osc2DetuneCents) / 1200.0) + (lfoToPitch ? static_cast<double> (lfo) * 0.05 : 0.0));

            const float s1 = oscSample (phase1, osc1Wave) * osc1Level;
            const float s2 = oscSample (phase2, osc2Wave) * osc2Level;
            float sample = (s1 + s2) * 0.5f;

            advancePhase (phase1, frequency, currentSampleRate);
            advancePhase (phase2, freqMod, currentSampleRate);

            float env = adsr.getNextSample();
            if (lfoToAmp)
                sample *= juce::jlimit (0.0f, 2.0f, 1.0f + lfo);

            // Filter modulation
            auto cutoff = filterCutoff;
            if (lfoToFilter)
                cutoff = juce::jlimit (20.0f, 18000.0f, cutoff * (1.0f + lfo * 0.5f));
            cutoff = juce::jlimit (20.0f, 18000.0f, cutoff + env * filterEnvAmount);
            filter.setCutoffFrequency (cutoff);
            filter.setResonance (filterResonance);

            auto filtered = filter.processSample (0, sample);
            const float out = filtered * env * level * masterGain;

            outL[i] += out;
            if (outR != nullptr)
                outR[i] += out;

            if (! adsr.isActive())
            {
                clearCurrentNote();
                break;
            }
        }
    }

private:
    static float oscSample (double phase, int wave)
    {
        const auto p = phase / (2.0 * juce::MathConstants<double>::pi);
        switch (wave)
        {
            case 1: return p < 0.5 ? 1.0f : -1.0f; // square
            case 2: return static_cast<float> (2.0 * p - 1.0); // saw
            case 3: return static_cast<float> (2.0 * std::abs (2.0 * p - 1.0) - 1.0); // triangle
            default: return static_cast<float> (std::sin (phase)); // sine
        }
    }

    static void advancePhase (double& phase, double freq, double sr)
    {
        phase += (2.0 * juce::MathConstants<double>::pi * freq) / sr;
        const auto twoPi = 2.0 * juce::MathConstants<double>::pi;
        if (phase >= twoPi)
            phase -= twoPi;
    }

    void updateParams()
    {
        auto get = [this] (const char* id) { return params.getRawParameterValue (id)->load(); };

        osc1Wave = static_cast<int> (get ("osc1Wave"));
        osc2Wave = static_cast<int> (get ("osc2Wave"));
        osc1Level = get ("osc1Level");
        osc2Level = get ("osc2Level");
        osc2DetuneCents = get ("osc2Detune");

        juce::ADSR::Parameters a;
        a.attack = get ("attack");
        a.decay = get ("decay");
        a.sustain = get ("sustain");
        a.release = get ("release");
        adsr.setParameters (a);

        filterCutoff = get ("cutoff");
        filterResonance = juce::jlimit (0.0f, 1.0f, get ("resonance"));
        filterEnvAmount = get ("filterEnv");

        lfoRate = get ("lfoRate");
        lfoDepth = get ("lfoDepth");
        const int target = static_cast<int> (get ("lfoTarget"));
        lfoToPitch = target == 0;
        lfoToFilter = target == 1;
        lfoToAmp = target == 2;

        masterGain = get ("gain");
    }

    juce::AudioProcessorValueTreeState& params;
    juce::ADSR adsr;
    juce::dsp::StateVariableTPTFilter<float> filter;

    double currentSampleRate = 44100.0;
    double frequency = 440.0;
    double phase1 = 0.0;
    double phase2 = 0.0;
    double lfoPhase = 0.0;
    float level = 0.0f;
    int currentNote = -1;

    int osc1Wave = 0, osc2Wave = 2;
    float osc1Level = 0.8f, osc2Level = 0.6f, osc2DetuneCents = 5.0f;
    float filterCutoff = 8000.0f, filterResonance = 0.2f, filterEnvAmount = 0.0f;
    float lfoRate = 2.0f, lfoDepth = 0.0f;
    bool lfoToPitch = false, lfoToFilter = true, lfoToAmp = false;
    float masterGain = 0.7f;
};