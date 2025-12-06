# MyVST3Plugin - Professional Dual Oscillator Synthesizer

A powerful and versatile VST3 synthesizer plugin built with JUCE framework, featuring dual oscillators, ADSR envelope, and advanced filtering capabilities.

![MyVST3Plugin](https://img.shields.io/badge/Version-1.0.0-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![License](https://img.shields.io/badge/License-Commercial-red)

## 🎵 Features

### Core Synthesis
- **Dual Oscillators**: Independent waveform selection (Sine/Square/Saw/Triangle)
- **Oscillator Detune**: -50 to +50 cents for chorus effects
- **ADSR Envelope**: Full Attack, Decay, Sustain, Release envelope control
- **Master Volume**: Precise output level control

### Advanced Filtering
- **Low-Pass Filter**: 20Hz - 20kHz cutoff frequency
- **Resonance Control**: 0.1 - 10.0 Q-factor adjustment
- **Stereo Processing**: Independent filter processing per channel

### FM Modulation System
- **Third Oscillator**: Dedicated FM modulator for advanced sound design
- **FM Frequency**: 20Hz - 2000Hz modulation rate control
- **FM Mix**: 0-100% modulation intensity
- **FM Enable**: On/off switch for frequency modulation

### User Interface
- **Real-time Controls**: All parameters adjustable in real-time
- **Visual Feedback**: Live parameter value display
- **MIDI Keyboard**: Built-in virtual MIDI keyboard for testing
- **Test Mode**: Constant audio generation for parameter testing

### Audio Quality
- **Professional DSP**: Built with JUCE's high-quality audio processing
- **Stereo Output**: Full stereo signal path
- **Sample Accurate**: Precise parameter timing

## 🚀 Getting Started

### Prerequisites
- Windows 10/11
- DAW supporting VST3 plugins (Reaper, Ableton Live, FL Studio, etc.)
- Visual Studio Build Tools (for compilation)

### Installation
1. Download the latest release
2. Copy `MyVST3Plugin.vst3` to your VST3 directory:
   ```
   C:\Program Files\Common Files\VST3\
   ```
   Or for local installation (recommended):
   ```
   %USERPROFILE%\Documents\REAPER\UserPlugins\
   ```
3. **For Reaper**: Go to Options → Preferences → Plug-ins → VST → Clear cache and rescan
4. The plugin will appear under "MyVST3Plugin" in your effects list

### Standalone Version
The plugin also includes a standalone executable for testing without a DAW:
```
MyVST3Plugin_artefacts\Debug\Standalone\MyVST3Plugin.exe
```

### Reaper-Specific Setup
For optimal performance in Reaper:

1. **Plugin Installation**: Place the .vst3 file in your Reaper UserPlugins directory
2. **VST Scanning**: Use Reaper's VST scanner (Options → Preferences → Plug-ins → VST)
3. **Buffer Settings**: Works best with Reaper's default buffer settings (64-512 samples)
4. **MIDI Input**: Enable "Send all input to VSTi" in Reaper's MIDI settings for keyboard control

**Reaper Compatibility**: ✅ Fully tested and optimized for Reaper

## 🎛️ Usage

### Basic Operation
1. Load the plugin in your DAW
2. Choose waveforms for Osc1 and Osc2 (Sine/Square/Saw/Triangle)
3. Set oscillator frequencies (20Hz - 20kHz range)
4. Apply detune to Osc2 (-50/+50 cents) for chorus effects
5. Adjust ADSR envelope for desired sound shaping
6. Use the filter to sculpt the tone
7. Control master volume for final output level

### FM Modulation
8. Enable FM modulation with the "FM ON" button
9. Set FM frequency (20Hz-2000Hz) for modulation speed
10. Adjust FM mix (0-100%) for modulation intensity
11. Experiment with different FM frequencies for evolving timbres

### Oscillator Controls
- **Waveform Selection**: Choose from 4 waveforms per oscillator
  - **Sine**: Pure, smooth tones
  - **Square**: Bright, hollow sound
  - **Saw**: Rich, cutting harmonics
  - **Triangle**: Soft, flute-like tones
- **Frequency**: 20Hz - 20kHz range
- **Detune**: -50 to +50 cents for chorus effects

### Filter Controls
- **Cutoff**: Sets the frequency where filtering begins (lower = darker sound)
- **Resonance**: Emphasizes frequencies around the cutoff (higher = more pronounced peak)

### MIDI Control
- Receives MIDI note input for pitch control
- Velocity sensitive (future enhancement)
- Supports all standard MIDI channels

## 🔧 Building from Source

### Requirements
- CMake 3.15+
- Visual Studio 2019/2022 Build Tools
- JUCE Framework

### Build Steps
```bash
# Configure
cmake -B build -S .

# Build
cmake --build build --config Debug

# Install
cmake --install build
```

## 📊 Technical Specifications

- **Format**: VST3
- **Architecture**: 64-bit
- **Sample Rates**: All standard rates supported
- **Latency**: Minimal (< 1ms)
- **CPU Usage**: Optimized for real-time performance

## 🎼 Sound Design Tips

### Bass Sounds
- Osc1: Saw wave (rich harmonics)
- Osc2: Square wave + 7 cents detune
- Fast attack, slow release
- Filter cutoff around 200-500Hz

### Leads
- Osc1: Square wave (bright)
- Osc2: Triangle wave + 12 cents detune
- Medium ADSR settings
- Filter resonance for bite

### Pads
- Osc1: Sine wave (smooth)
- Osc2: Saw wave + 5 cents detune
- Slow attack and release
- Wide filter settings
- Low resonance for smoothness

### Sound Design Examples
- **Classic Synth**: Square + Square waves
- **Warm Pad**: Sine + Triangle waves
- **Cutting Lead**: Saw + Square waves
- **Detuned Bass**: Saw + Saw with 10 cents detune

## 🔒 Commercial Product

This is a commercial software product. No external contributions or modifications are accepted.

For feature requests or support inquiries, please contact the developer directly.

## 💰 Licensing & Purchase

This is a commercial software product that requires a paid license to use.

### License Options
- **Personal License**: For individual musicians and producers
- **Commercial License**: For commercial studios and production companies
- **Educational License**: Discounted rates for students and educational institutions

### Purchase Information
- Visit [Your Website] for pricing and purchase options
- Demo/trial versions may be available with limited functionality
- All sales are final - no refunds

### License Terms
- One license per user/machine
- License keys are required for activation
- See [LICENSE](LICENSE) file for complete terms and conditions

---

**Built with ❤️ using JUCE Framework**