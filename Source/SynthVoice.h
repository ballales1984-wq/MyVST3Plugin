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

/** Singola voce polifonica: due oscillatori, inviluppo ADSR, filtro lowpass
 *  e LFO instradabile su pitch, filtro o ampiezza. */
class SynthVoice : public juce::SynthesiserVoice
{
public:
    explicit SynthVoice (juce::AudioProcessorValueTreeState& apvts) : params (apvts) {}

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*> (sound) != nullptr;
    }

    // Nota premuta: reset delle fasi, lettura dei parametri e avvio ADSR.
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

    // Nota rilasciata: coda di rilascio oppure arresto immediato della voce.
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

    // Pitch bend e controller non gestiti.
    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    /** Prepara ADSR, filtro e smoothed-value con il sample rate corrente. */
    void prepareToPlay (double sampleRate, int, int)
    {
        currentSampleRate = sampleRate;
        adsr.setSampleRate (sampleRate);

        const double tail = 0.05; // 50 ms di smoothing sui parametri variabili
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

    // Rendering voce per campione: oscillatori -> filtro -> inviluppo -> uscita.
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (! isVoiceActive())
            return;

        updateParams();

        auto* outL = outputBuffer.getWritePointer (0, startSample);
        auto* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;

        const double dp1 = (2.0 * juce::MathConstants<double>::pi * frequency) / currentSampleRate;
        const double dpNorm1 = dp1 / (2.0 * juce::MathConstants<double>::pi); // passo normalizzato 0..1

        for (int i = 0; i < numSamples; ++i)
        {
            if (currentSampleRate <= 0.0)
                break;

            // LFO sinusoidale normalizzato in [-1, 1].
            const float lfo = static_cast<float> (std::sin (lfoPhase)) * smoothLfoDepth.getNextValue();
            lfoPhase += (2.0 * juce::MathConstants<double>::pi * lfoRate) / currentSampleRate;
            if (lfoPhase >= 2.0 * juce::MathConstants<double>::pi)
                lfoPhase -= 2.0 * juce::MathConstants<double>::pi;

            // Frequenza osc2: detune + eventuale LFO su pitch.
            const double freqMod = frequency * std::pow (2.0, (static_cast<double> (osc2DetuneCents) / 1200.0) + (lfoToPitch ? static_cast<double> (lfo) * 0.05 : 0.0));
            const double dp2 = (2.0 * juce::MathConstants<double>::pi * freqMod) / currentSampleRate;
            const double dpNorm2 = dp2 / (2.0 * juce::MathConstants<double>::pi);

            const float s1 = oscSampleBandLimited (phase1, osc1Wave, dpNorm1) * smoothOsc1Level.getNextValue();
            const float s2 = oscSampleBandLimited (phase2, osc2Wave, dpNorm2) * smoothOsc2Level.getNextValue();
            float sample = (s1 + s2) * 0.5f;

            advancePhase (phase1, dp1);
            advancePhase (phase2, dp2);

            // Inviluppo: usato sia per l'ampiezza sia per il filtro.
            float env = adsr.getNextSample();
            if (lfoToAmp)
                sample *= juce::jlimit (0.0f, 2.0f, 1.0f + lfo);

            // Modulazione del filtro: LFO e inviluppo sommati al cutoff.
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

            // Fine inviluppo: libera la voce.
            if (! adsr.isActive())
            {
                clearCurrentNote();
                break;
            }
        }
    }

private:
    /** Campione oscillatore anti-aliased dato fase (0..2pi), tipo d'onda e passo normalizzato. */
    static float oscSampleBandLimited (double phase, int wave, double dpNorm)
    {
        const auto twoPi = 2.0 * juce::MathConstants<double>::pi;
        const double t = phase / twoPi; // posizione nel ciclo 0..1

        switch (wave)
        {
            case 0: // Sine
                return static_cast<float> (std::sin (phase));

            case 1: // Square anti-aliased (PolyBLEP)
            {
                float v = t < 0.5 ? 1.0f : -1.0f;
                // Salto discendente a t = 0.5
                if (t < 0.5 && t + dpNorm >= 0.5)
                    v -= static_cast<float> (2.0 * blep (t + dpNorm - 0.5));
                // Salto ascendente al wrap t = 1 -> 0
                if (t + dpNorm >= 1.0)
                    v += static_cast<float> (2.0 * blep (t + dpNorm - 1.0));
                return v;
            }

            case 2: // Saw anti-aliased (PolyBLEP)
            {
                float v = static_cast<float> (2.0 * t - 1.0);
                // Salto al wrap t = 1 -> 0
                if (t + dpNorm >= 1.0)
                    v += static_cast<float> (2.0 * blep (t + dpNorm - 1.0));
                return v;
            }

            case 3: // Triangle (già senza salti discontinui)
            {
                return static_cast<float> (2.0 * std::abs (2.0 * t - 1.0) - 1.0);
            }

            default:
                return static_cast<float> (std::sin (phase));
        }
    }

    /** Incrementa la fase di dp e la mantiene in [0, 2pi). */
    static void advancePhase (double& phase, double dp)
    {
        phase += dp;
        const auto twoPi = 2.0 * juce::MathConstants<double>::pi;
        if (phase >= twoPi)
            phase -= twoPi;
    }

    /** Copia i valori correnti dei parametri nelle variabili della voce (target per lo smoothing). */
    void updateParams()
    {
        auto get = [this] (const char* id) { return params.getRawParameterValue (id)->load(); };

        osc1Wave = static_cast<int> (get ("osc1Wave"));
        osc2Wave = static_cast<int> (get ("osc2Wave"));
        osc2DetuneCents = get ("osc2Detune");

        // Target per lo smoothing (il valore effettivo viene letto nel loop via getNextValue()).
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

    // Stato della voce.
    double currentSampleRate = 44100.0;
    double frequency = 440.0;
    double phase1 = 0.0;
    double phase2 = 0.0;
    double lfoPhase = 0.0;
    float level = 0.0f;
    int currentNote = -1;

    // Parametri non-smoothed (cambiati solo a inizio nota o raramente).
    int osc1Wave = 0, osc2Wave = 2;
    float osc2DetuneCents = 5.0f;
    float lfoRate = 2.0f;
    bool lfoToPitch = false, lfoToFilter = true, lfoToAmp = false;

    // Valori parametri con smoothing applicato per campione.
    juce::SmoothedValue<float> smoothOsc1Level;
    juce::SmoothedValue<float> smoothOsc2Level;
    juce::SmoothedValue<float> smoothCutoff;
    juce::SmoothedValue<float> smoothResonance;
    juce::SmoothedValue<float> smoothFilterEnvAmount;
    juce::SmoothedValue<float> smoothLfoDepth;
    juce::SmoothedValue<float> smoothMasterGain;
};
