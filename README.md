# MyVST3Plugin - Virtual MIDI Keyboard Synthesizer

A professional VST3 synthesizer plugin with a virtual MIDI keyboard, built using the JUCE framework.

## 🎹 Features

### Core Functionality
- **Dual Oscillator Synthesizer** with independent frequency controls
- **Virtual MIDI Keyboard** with clickable piano keys (white and black keys)
- **Complete ADSR Envelope** (Attack, Decay, Sustain, Release)
- **Master Volume Control** with real-time visualization
- **External MIDI Device Support** for physical keyboards and controllers

### Audio Engine
- **Oscillator 1**: Sine wave generator
- **Oscillator 2**: Square wave generator
- **ADSR Envelope**: Full ADSR with visual feedback
- **Real-time Processing**: Optimized for low latency

### User Interface
- **Virtual Keyboard Component**: Interactive piano keyboard
- **Parameter Sliders**: Smooth controls for all parameters
- **Real-time Displays**: Live visualization of envelope levels and MIDI notes
- **MIDI Status Indicators**: Shows active notes and input status
- **Test Mode**: Generate sound without external MIDI input

## 🚀 Getting Started

### Prerequisites
- **Windows 10/11**
- **Visual Studio Build Tools** (or full Visual Studio)
- **CMake 3.15+**
- **JUCE Framework** (included as submodule)

### Building the Plugin

1. **Clone the repository:**
   ```bash
   git clone YOUR_REPOSITORY_URL
   cd MyVST3Plugin
   ```

2. **Run the automated build script:**
   ```bash
   build_final.bat
   ```

3. **Install the plugin:**
   - Copy `MyVST3Plugin.vst3` to `C:\Program Files\Common Files\VST3\`
   - Or use the automated script

### Manual Build Steps
```bash
# Configure build environment
call "C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake -G "NMake Makefiles" ..

# Build
nmake
```

## 🎛️ Using the Plugin

### In Your DAW
1. **Create an Instrument Track**
2. **Load MyVST3Plugin** from your VST3 plugins
3. **Open the Plugin Interface**

### Interface Overview
- **Top Section**: Virtual MIDI keyboard (click to play notes)
- **Middle Section**: Oscillator controls and ADSR envelope
- **Bottom Section**: Master volume and status displays

### Controls
- **Osc1 Frequency**: Controls the first oscillator (sine wave)
- **Osc2 Frequency**: Controls the second oscillator (square wave)
- **Attack/Decay/Sustain/Release**: ADSR envelope parameters
- **Master Volume**: Overall output level
- **Test Mode**: Enable continuous sound generation

## 🎵 MIDI Support

### Virtual Keyboard
- **Click piano keys** to trigger notes
- **Visual feedback** shows pressed keys
- **MIDI note display** shows current note and octave

### External MIDI Devices
- **Automatic detection** of connected MIDI devices
- **Real-time input** from keyboards and controllers
- **MIDI channel support** (configurable)

## 🛠️ Technical Details

### Architecture
- **Plugin Format**: VST3
- **Framework**: JUCE 8.x
- **Build System**: CMake
- **Compiler**: MSVC (Visual Studio)
- **Platform**: Windows x64

### Code Structure
```
MyVST3Plugin/
├── CMakeLists.txt          # Build configuration
├── Source/
│   ├── PluginProcessor.h   # Audio processing logic
│   ├── PluginProcessor.cpp
│   ├── PluginEditor.h      # GUI implementation
│   └── PluginEditor.cpp
├── build/                  # Build artifacts (ignored)
└── README.md               # This file
```

### Dependencies
- **JUCE Modules**:
  - `juce_audio_basics`
  - `juce_audio_devices`
  - `juce_audio_processors`
  - `juce_audio_utils`
  - `juce_gui_basics`
  - `juce_gui_extra`
  - `juce_core`
  - `juce_dsp`

## 📋 Development Notes

### Building for Different Configurations
```bash
# Debug build
cmake -G "NMake Makefiles" ..
nmake

# Release build (default)
cmake -G "NMake Makefiles" ..
nmake
```

### Troubleshooting
- **Plugin not found in DAW**: Force plugin rescan in your DAW
- **Build errors**: Ensure Visual Studio Build Tools are installed
- **MIDI not working**: Check MIDI device connections in Windows settings

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

- **JUCE Framework**: For providing the audio plugin development tools
- **Raw Material Software**: For creating and maintaining JUCE
- **VST3 SDK**: For the plugin format specification

---

**Built with ❤️ using JUCE**
