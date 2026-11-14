#pragma once

#include <JuceHeader.h>

/** Sound "vuota": il synth accetta qualsiasi nota e canale. */
struct SynthSound : public juce::SynthesiserSound
{
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

/** BLEP (Band-Limited Step): integra un salto discontinuo su un campione
 *  per eliminare l'aliasing delle onde saw e square. */
inline double blep (double t)
{
    if (t <= 0.0 || t >= 1.0)
        return 0.0;
    return 2.0 * t - t * t - 1.0;
}

/** Singola voce polifonica. Con unisono > 1 somma internamente N oscillatori
 *  detunati per onda (centrati sulla nota base), simulando un coro di voci. */
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
        lfoPhase = 0.0;

        // Legge i parametri PRIMA di dimensionare le fasi: unisonCount
        // determina quante fasi servono, altrimenti si scrive fuori range.
        updateParams();
        adsr.noteOn();

        unisonPhase1.assign (static_cast<size_t> (unisonCount), 0.0);
        unisonPhase2.assign (static_cast<size_t> (unisonCount), 0.0);
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

        const double tail = 0.05;
        smoothCutoff.reset (sampleRate, tail);
        smoothResonance.reset (sampleRate, tail);
        smoothOsc1Level.reset (sampleRate, tail);
        smoothOsc2Level.reset (sampleRate, tail);
        smoothMasterGain.reset (sampleRate, tail);
        smoothLfoDepth.reset (sampleRate, tail);
        smoothFilterEnvAmount.reset (sampleRate, tail);

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

        // Numero di oscillatori unisono effettivamente allocati.
        const int voices = static_cast<int> (unisonPhase1.size());

        if (voices <= 0)
            return;

        auto* outL = outputBuffer.getWritePointer (0, startSample);
        auto* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;

        for (int i = 0; i < numSamples; ++i)
        {
            if (currentSampleRate <= 0.0)
                break;

            const float lfo = static_cast<float> (std::sin (lfoPhase)) * smoothLfoDepth.getNextValue();
            lfoPhase += (2.0 * juce::MathConstants<double>::pi * lfoRate) / currentSampleRate;
            if (lfoPhase >= 2.0 * juce::MathConstants<double>::pi)
                lfoPhase -= 2.0 * juce::MathConstants<double>::pi;

            float sum = 0.0f;
            const float osc1L = smoothOsc1Level.getNextValue();
            const float osc2L = smoothOsc2Level.getNextValue();

            for (int u = 0; u < voices; ++u)
            {
                // Detune centrato: voci distribuite simmetricamente attorno alla nota base.
                const double detuneCents = (voices > 1)
                    ? (static_cast<double> (u) - (voices - 1) / 2.0) * unisonDetuneCents
                    : 0.0;
                const double detuneRatio = std::pow (2.0, detuneCents / 1200.0);

                const double freqU = frequency * detuneRatio;
                const double dp1 = (2.0 * juce::MathConstants<double>::pi * freqU) / currentSampleRate;
                const double dpNorm1 = dp1 / (2.0 * juce::MathConstants<double>::pi);

                const double freqMod = freqU * std::pow (2.0, (static_cast<double> (osc2DetuneCents) / 1200.0) + (lfoToPitch ? static_cast<double> (lfo) * 0.05 : 0.0));
                const double dp2 = (2.0 * juce::MathConstants<double>::pi * freqMod) / currentSampleRate;
                const double dpNorm2 = dp2 / (2.0 * juce::MathConstants<double>::pi);

                const float s1 = oscSampleBandLimited (unisonPhase1[u], osc1Wave, dpNorm1) * osc1L;
                const float s2 = oscSampleBandLimited (unisonPhase2[u], osc2Wave, dpNorm2) * osc2L;
                sum += (s1 + s2) * 0.5f;

                advancePhase (unisonPhase1[u], dp1);
                advancePhase (unisonPhase2[u], dp2);
            }

            float sample = sum / static_cast<float> (voices);

            float env = adsr.getNextSample();
            if (lfoToAmp)
                sample *= juce::jlimit (0.0f, 2.0f, 1.0f + lfo);

            auto cutoff = smoothCutoff.getNextValue();
            if (lfoToFilter)
                cutoff = juce::jlimit (20.0f, 18000.0f, cutoff * (1.0f + lfo * 0.5f));
            cutoff = juce::jlimit (20.0f, 18000.0f, cutoff + env * smoothFilterEnvAmount.getNextValue());
            filter.setCutoffFrequency (cutoff);
            filter.setResonance (smoothResonance.getNextValue());

            auto filtered = filter.processSample (0, sample);
            const float out = filtered * env * level * smoothMasterGain.getNextValue();

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
    static float oscSampleBandLimited (double phase, int wave, double dpNorm)
    {
        const auto twoPi = 2.0 * juce::MathConstants<double>::pi;
        const double t = phase / twoPi;

        switch (wave)
        {
            case 0:
                return static_cast<float> (std::sin (phase));

            case 1:
            {
                float v = t < 0.5 ? 1.0f : -1.0f;
                if (t < 0.5 && t + dpNorm >= 0.5)
                    v -= static_cast<float> (2.0 * blep (t + dpNorm - 0.5));
                if (t + dpNorm >= 1.0)
                    v += static_cast<float> (2.0 * blep (t + dpNorm - 1.0));
                return v;
            }

            case 2:
            {
                float v = static_cast<float> (2.0 * t - 1.0);
                if (t + dpNorm >= 1.0)
                    v += static_cast<float> (2.0 * blep (t + dpNorm - 1.0));
                return v;
            }

            case 3:
                return static_cast<float> (2.0 * std::abs (2.0 * t - 1.0) - 1.0);

            case 4: // Pulse 25% (band-limited)
            {
                float v = t < 0.25 ? 1.0f : -1.0f;
                if (t < 0.25 && t + dpNorm >= 0.25)
                    v -= static_cast<float> (2.0 * blep (t + dpNorm - 0.25));
                if (t + dpNorm >= 1.0)
                    v += static_cast<float> (2.0 * blep (t + dpNorm - 1.0));
                return v;
            }

            case 5: // FM (phase modulation)
                return static_cast<float> (std::sin (phase + 2.0 * std::sin (phase)));

            case 6: // Organ (somma di armoniche)
                return static_cast<float> (std::sin (phase)
                                         + 0.5  * std::sin (2.0 * phase)
                                         + 0.25 * std::sin (3.0 * phase));

            default:
                return static_cast<float> (std::sin (phase));
        }
    }

    static void advancePhase (double& phase, double dp)
    {
        phase += dp;
        const auto twoPi = 2.0 * juce::MathConstants<double>::pi;
        if (phase >= twoPi)
            phase -= twoPi;
    }

    void updateParams()
    {
        auto get = [this] (const char* id) { return params.getRawParameterValue (id)->load(); };

        osc1Wave = static_cast<int> (get ("osc1Wave"));
        osc2Wave = static_cast<int> (get ("osc2Wave"));
        osc2DetuneCents = get ("osc2Detune");
        unisonCount = juce::jlimit (1, 8, static_cast<int> (get ("unisonCount")));
        unisonDetuneCents = get ("unisonDetune");

        smoothOsc1Level.setTargetValue (get ("osc1Level"));
        smoothOsc2Level.setTargetValue (get ("osc2Level"));

        juce::ADSR::Parameters a;
        a.attack = get ("attack");
        a.decay = get ("decay");
        a.sustain = get ("sustain");
        a.release = get ("release");
        adsr.setParameters (a);

        smoothCutoff.setTargetValue (get ("cutoff"));
        smoothResonance.setTargetValue (juce::jlimit (0.0f, 1.0f, get ("resonance")));
        smoothFilterEnvAmount.setTargetValue (get ("filterEnv"));

        lfoRate = get ("lfoRate");
        smoothLfoDepth.setTargetValue (get ("lfoDepth"));
        const int target = static_cast<int> (get ("lfoTarget"));
        lfoToPitch = target == 0;
        lfoToFilter = target == 1;
        lfoToAmp = target == 2;

        smoothMasterGain.setTargetValue (get ("gain"));
    }

    juce::AudioProcessorValueTreeState& params;
    juce::ADSR adsr;
    juce::dsp::StateVariableTPTFilter<float> filter;

    double currentSampleRate = 44100.0;
    double frequency = 440.0;
    double lfoPhase = 0.0;
    float level = 0.0f;
    int currentNote = -1;

    int osc1Wave = 0, osc2Wave = 2;
    float osc2DetuneCents = 5.0f;
    float lfoRate = 2.0f;
    int unisonCount = 2;
    float unisonDetuneCents = 7.0f;
    bool lfoToPitch = false, lfoToFilter = true, lfoToAmp = false;

    std::vector<double> unisonPhase1;
    std::vector<double> unisonPhase2;

    juce::SmoothedValue<float> smoothOsc1Level;
    juce::SmoothedValue<float> smoothOsc2Level;
    juce::SmoothedValue<float> smoothCutoff;
    juce::SmoothedValue<float> smoothResonance;
    juce::SmoothedValue<float> smoothFilterEnvAmount;
    juce::SmoothedValue<float> smoothLfoDepth;
    juce::SmoothedValue<float> smoothMasterGain;
};
