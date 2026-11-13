# MyVST3Plugin - Dual Oscillator Synthesizer (VST3)

JUCE-based VST3/Standalone synthesizer for Windows (and macOS via CI).

## Current Features
- Dual oscillators (Sine/Square/Saw/Triangle) with PolyBLEP anti-aliasing on saw/square
- Oscillator 2 detune (±50 cents)
- ADSR envelope (attack/decay/sustain/release)
- Low-pass StateVariableTPT filter with resonance + envelope amount
- LFO with depth and target (Pitch/Filter/Amp)
- 8-voice polyphonic synthesis via `juce::Synthesiser`
- MIDI input + virtual keyboard (`juce::MidiKeyboardState`)
- Master gain
- Parameter smoothing (`juce::SmoothedValue`, 50 ms) to avoid zipper noise
- Velocity note-on
- State save/restore via `juce::AudioProcessorValueTreeState` (APVTS)
- Stereo output bus, `isBusesLayoutSupported` mono/stereo

## DSP notes
- Oscillators saw/square use PolyBLEP (band-limited step) integration of
  discontinuities to suppress aliasing at high frequencies.
- Parameters that change during audio (`cutoff`, `resonance`, levels,
  `gain`, LFO depth, filter env amount) are ramped per-sample with
  `juce::SmoothedValue` (50 ms time constant).
- The filter is a `juce::dsp::StateVariableTPTFilter` low-pass, smoothed
  cutoff/resonance each sample.
- No allocations or locks in `processBlock`.

## Build (Windows, MSVC)
```bat
.\quick_build.bat
```

## Install (Reaper, local user)
```powershell
.\install_vst3_local.ps1
```

## Standalone quick test
```bat
.\test_plugin.bat
```

## CI
GitHub Actions builds Release on Windows (`windows-latest`, MSVC/NMake) and
macOS (`macOS-latest`, Unix Makefiles) on every push/PR to `master`, and
uploads the VST3 artifact.
