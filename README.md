# MyVST3Plugin - Dual Oscillator Synthesizer (VST3)

JUCE-based VST3/Standalone synthesizer.

## Current Features
- Dual oscillators (Sine/Square/Saw/Triangle)
- ADSR envelope
- Low-pass ladder filter with resonance + envelope amount
- LFO with depth and target (Pitch/Filter/Amp)
- MIDI input + virtual keyboard
- Master gain
- State save/restore via APVTS

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
