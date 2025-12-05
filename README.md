# MyVST3Plugin - Professional Dual Oscillator Synthesizer

A powerful and versatile VST3 synthesizer plugin built with JUCE framework, featuring dual oscillators, ADSR envelope, and advanced filtering capabilities.

![MyVST3Plugin](https://img.shields.io/badge/Version-1.0.0-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![License](https://img.shields.io/badge/License-Proprietary-red)

## 🎵 Features

### Core Synthesis
- **Dual Oscillators**: Independent sine and square wave oscillators
- **ADSR Envelope**: Full Attack, Decay, Sustain, Release envelope control
- **Master Volume**: Precise output level control

### Advanced Filtering
- **Low-Pass Filter**: 20Hz - 20kHz cutoff frequency
- **Resonance Control**: 0.1 - 10.0 Q-factor adjustment
- **Stereo Processing**: Independent filter processing per channel

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
3. Rescan plugins in your DAW

### Standalone Version
The plugin also includes a standalone executable for testing without a DAW:
```
MyVST3Plugin_artefacts\Debug\Standalone\MyVST3Plugin.exe
```

## 🎛️ Usage

### Basic Operation
1. Load the plugin in your DAW
2. Set oscillator frequencies (20Hz - 20kHz range)
3. Adjust ADSR envelope for desired sound shaping
4. Use the filter to sculpt the tone
5. Control master volume for final output level

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
- Osc1: Low frequency sine wave
- Osc2: Slightly detuned for thickness
- Fast attack, slow release
- Filter cutoff around 200-500Hz

### Leads
- Mix of sine and square waves
- Medium ADSR settings
- Filter resonance for bite
- Higher oscillator frequencies

### Pads
- Slow attack and release
- Both oscillators active
- Wide filter settings
- Low resonance for smoothness

## 🤝 Contributing

This is a proprietary commercial product. No external contributions accepted.

## 📞 Support

For support or licensing inquiries, please contact the developer.

## 📄 License

Copyright (c) 2025 [Your Name]. All rights reserved.

This software is proprietary and confidential. No part may be reproduced, distributed, or transmitted in any form without written permission.

---

**Built with ❤️ using JUCE Framework**