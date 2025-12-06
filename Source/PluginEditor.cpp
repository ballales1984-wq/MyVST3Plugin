#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyVST3PluginAudioProcessorEditor::MyVST3PluginAudioProcessorEditor (MyVST3PluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      keyboardComponent (audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Set window size for 5 columns with waveform controls
    setSize (1400, 850); // Wider window for waveform controls

    // Setup title
    titleLabel.setText("MyVST3Plugin - Debug Test", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Add MidiKeyboardComponent
    addAndMakeVisible (keyboardComponent);

    // Setup master volume slider
    setupSlider(masterVolumeSlider, masterVolumeLabel, masterVolumeValueLabel,
                "Master Volume", 0.0f, 1.0f, 0.5f, "");

    // Setup oscillator frequency sliders
    setupSlider(osc1FrequencySlider, osc1FrequencyLabel, osc1FrequencyValueLabel,
                "Osc1 Frequency", 20.0f, 20000.0f, 440.0f, " Hz");

    setupSlider(osc2FrequencySlider, osc2FrequencyLabel, osc2FrequencyValueLabel,
                "Osc2 Frequency", 20.0f, 20000.0f, 220.0f, " Hz");

    // Setup Osc2 detune slider (NEW)
    setupSlider(osc2DetuneSlider, osc2DetuneLabel, osc2DetuneValueLabel,
                "Osc2 Detune", -50.0f, 50.0f, 0.0f, " cents");

    // Setup waveform selectors (NEW)
    setupWaveformSelector(osc1WaveformSelector, osc1WaveformLabel, "Osc1 Waveform");
    setupWaveformSelector(osc2WaveformSelector, osc2WaveformLabel, "Osc2 Waveform");

    // Setup ADSR sliders
    setupSlider(attackSlider, attackLabel, attackValueLabel,
                "Attack", 0.001f, 5.0f, 0.1f, " s");

    setupSlider(decaySlider, decayLabel, decayValueLabel,
                "Decay", 0.001f, 5.0f, 0.2f, " s");

    setupSlider(sustainSlider, sustainLabel, sustainValueLabel,
                "Sustain", 0.0f, 1.0f, 0.8f, "");

    setupSlider(releaseSlider, releaseLabel, releaseValueLabel,
                "Release", 0.001f, 5.0f, 0.3f, " s");

    // Setup filter sliders
    setupSlider(filterCutoffSlider, filterCutoffLabel, filterCutoffValueLabel,
                "Filter Cutoff", 20.0f, 20000.0f, 10000.0f, " Hz");

    setupSlider(filterResonanceSlider, filterResonanceLabel, filterResonanceValueLabel,
                "Filter Resonance", 0.1f, 10.0f, 0.707f, "");


    // Create parameter attachments
    masterVolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramMasterVolume, masterVolumeSlider);

    osc1FrequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramOsc1Frequency, osc1FrequencySlider);

    osc2FrequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramOsc2Frequency, osc2FrequencySlider);

    osc2DetuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(  // NEW
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramOsc2Detune, osc2DetuneSlider);

    osc1WaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(  // NEW
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramOsc1Waveform, osc1WaveformSelector);

    osc2WaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(  // NEW
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramOsc2Waveform, osc2WaveformSelector);

    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramAttack, attackSlider);

    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramDecay, decaySlider);

    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramSustain, sustainSlider);

    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramRelease, releaseSlider);

    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramFilterCutoff, filterCutoffSlider);

    filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramFilterResonance, filterResonanceSlider);

    // Setup MIDI status labels
    midiStatusLabel.setText("MIDI Status: No note", juce::dontSendNotification);
    midiStatusLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    midiStatusLabel.setJustificationType(juce::Justification::centred);
    midiStatusLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    addAndMakeVisible(midiStatusLabel);

    currentNoteLabel.setText("Note: --", juce::dontSendNotification);
    currentNoteLabel.setFont(juce::Font(12.0f));
    currentNoteLabel.setJustificationType(juce::Justification::centred);
    currentNoteLabel.setColour(juce::Label::textColourId, juce::Colours::blue);
    addAndMakeVisible(currentNoteLabel);

    // Setup envelope display
    envelopeLabel.setText("Envelope: 0.00", juce::dontSendNotification);
    envelopeLabel.setFont(juce::Font(12.0f));
    envelopeLabel.setJustificationType(juce::Justification::centred);
    envelopeLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    addAndMakeVisible(envelopeLabel);

    // Setup envelope status label
    envelopeStatusLabel.setText("Envelope: Idle", juce::dontSendNotification);
    envelopeStatusLabel.setFont(juce::Font(12.0f));
    envelopeStatusLabel.setJustificationType(juce::Justification::centred);
    envelopeStatusLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    addAndMakeVisible(envelopeStatusLabel);

    // Setup test mode button
    testModeButton.setButtonText("🎵 TEST MODE - CLICK TO PLAY SOUND 🎵");
    testModeButton.setTooltip("Enable test mode to play sound continuously without MIDI input");
    testModeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    testModeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(testModeButton);

    testModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramTestMode, testModeButton);

    // Add MidiMessageCollector as a listener to the keyboard state
    audioProcessor.keyboardState.addListener (&audioProcessor.midiCollector);

    // Start timer for updating value labels
    startTimer(50); // Update every 50ms for better MIDI responsiveness
}

MyVST3PluginAudioProcessorEditor::~MyVST3PluginAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void MyVST3PluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill background
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // Draw border
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);
}

void MyVST3PluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);

    // Title at the top
    titleLabel.setBounds(area.removeFromTop(40));

    // Keyboard component - moved to bottom
    auto keyboardArea = area.removeFromBottom(80);
    keyboardComponent.setBounds(keyboardArea);

    // Waveform selection grid layout - 5 columns
    const int numCols = 5;
    const int controlHeight = 70;
    const int controlWidth = area.getWidth() / numCols;

    // Row 1: Osc1, Osc2, Detune, Master, [Empty]
    for (int col = 0; col < numCols; ++col)
    {
        auto cellArea = area.removeFromTop(controlHeight).withWidth(controlWidth).withX(col * controlWidth);

        switch (col)
        {
            case 0: // Osc1
                osc1FrequencyLabel.setBounds(cellArea.removeFromTop(15));
                osc1FrequencySlider.setBounds(cellArea.removeFromTop(35));
                osc1FrequencyValueLabel.setBounds(cellArea);
                break;
            case 1: // Osc2
                osc2FrequencyLabel.setBounds(cellArea.removeFromTop(15));
                osc2FrequencySlider.setBounds(cellArea.removeFromTop(35));
                osc2FrequencyValueLabel.setBounds(cellArea);
                break;
            case 2: // Osc2 Detune (NEW)
                osc2DetuneLabel.setBounds(cellArea.removeFromTop(15));
                osc2DetuneSlider.setBounds(cellArea.removeFromTop(35));
                osc2DetuneValueLabel.setBounds(cellArea);
                break;
            case 3: // Master
                masterVolumeLabel.setBounds(cellArea.removeFromTop(15));
                masterVolumeSlider.setBounds(cellArea.removeFromTop(35));
                masterVolumeValueLabel.setBounds(cellArea);
                break;
            case 4: // Empty space
                break;
        }
    }

    // Row 2: ADSR Controls
    for (int col = 0; col < numCols; ++col)
    {
        auto cellArea = area.removeFromTop(controlHeight).withWidth(controlWidth).withX(col * controlWidth);

        switch (col)
        {
            case 0: // Attack
                attackLabel.setBounds(cellArea.removeFromTop(15));
                attackSlider.setBounds(cellArea.removeFromTop(35));
                attackValueLabel.setBounds(cellArea);
                break;
            case 1: // Decay
                decayLabel.setBounds(cellArea.removeFromTop(15));
                decaySlider.setBounds(cellArea.removeFromTop(35));
                decayValueLabel.setBounds(cellArea);
                break;
            case 2: // Sustain
                sustainLabel.setBounds(cellArea.removeFromTop(15));
                sustainSlider.setBounds(cellArea.removeFromTop(35));
                sustainValueLabel.setBounds(cellArea);
                break;
            case 3: // Release
                releaseLabel.setBounds(cellArea.removeFromTop(15));
                releaseSlider.setBounds(cellArea.removeFromTop(35));
                releaseValueLabel.setBounds(cellArea);
                break;
        }
    }

    // Row 2: ADSR Controls
    for (int col = 0; col < numCols; ++col)
    {
        auto cellArea = area.removeFromTop(controlHeight).withWidth(controlWidth).withX(col * controlWidth);

        switch (col)
        {
            case 0: // Attack
                attackLabel.setBounds(cellArea.removeFromTop(15));
                attackSlider.setBounds(cellArea.removeFromTop(35));
                attackValueLabel.setBounds(cellArea);
                break;
            case 1: // Decay
                decayLabel.setBounds(cellArea.removeFromTop(15));
                decaySlider.setBounds(cellArea.removeFromTop(35));
                decayValueLabel.setBounds(cellArea);
                break;
            case 2: // Sustain
                sustainLabel.setBounds(cellArea.removeFromTop(15));
                sustainSlider.setBounds(cellArea.removeFromTop(35));
                sustainValueLabel.setBounds(cellArea);
                break;
            case 3: // Release
                releaseLabel.setBounds(cellArea.removeFromTop(15));
                releaseSlider.setBounds(cellArea.removeFromTop(35));
                releaseValueLabel.setBounds(cellArea);
                break;
        }
    }

    // Row 3: Filter Controls
    auto filterArea = area.removeFromTop(controlHeight);
    filterCutoffLabel.setBounds(filterArea.removeFromLeft(200));
    filterCutoffSlider.setBounds(filterArea.removeFromLeft(150));
    filterCutoffValueLabel.setBounds(filterArea.removeFromLeft(100));
    filterResonanceLabel.setBounds(filterArea.removeFromLeft(200));
    filterResonanceSlider.setBounds(filterArea.removeFromLeft(150));
    filterResonanceValueLabel.setBounds(filterArea.removeFromLeft(100));
    testModeButton.setBounds(filterArea);
}

void MyVST3PluginAudioProcessorEditor::timerCallback()
{
    updateValueLabels();
}

//==============================================================================
// Helper methods
void MyVST3PluginAudioProcessorEditor::setupWaveformSelector(juce::ComboBox& comboBox, juce::Label& label, const juce::String& paramName)
{
    // Setup label
    label.setText(paramName, juce::dontSendNotification);
    label.setFont(juce::Font(12.0f));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);

    // Setup ComboBox
    comboBox.addItem("Sine", 1);
    comboBox.addItem("Square", 2);
    comboBox.addItem("Saw", 3);
    comboBox.addItem("Triangle", 4);
    comboBox.setSelectedItemIndex(0); // Default to Sine
    comboBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::darkgrey);
    comboBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    addAndMakeVisible(comboBox);
}

void MyVST3PluginAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label,
                                                  juce::Label& valueLabel, const juce::String& paramName,
                                                  float minValue, float maxValue, float defaultValue,
                                                  const juce::String& unit)
{
    // Setup label
    label.setText(paramName, juce::dontSendNotification);
    label.setFont(juce::Font(12.0f));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);

    // Setup rotary knob
    slider.setRange(minValue, maxValue, 0.001);
    slider.setValue(defaultValue);
    slider.setSliderStyle(juce::Slider::Rotary);
    slider.setRotaryParameters(juce::MathConstants<float>::pi * 1.2f, juce::MathConstants<float>::pi * 2.8f, true);
    slider.setTextValueSuffix(unit);
    slider.setNumDecimalPlacesToDisplay(2);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
    addAndMakeVisible(slider);

    // Setup value label
    valueLabel.setText(juce::String(defaultValue) + unit, juce::dontSendNotification);
    valueLabel.setFont(juce::Font(10.0f));
    valueLabel.setJustificationType(juce::Justification::centred);
    valueLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(valueLabel);
}

void MyVST3PluginAudioProcessorEditor::updateValueLabels()
{
    // Update parameter value labels
    masterVolumeValueLabel.setText(juce::String(masterVolumeSlider.getValue(), 2), juce::dontSendNotification);
    osc1FrequencyValueLabel.setText(juce::String(osc1FrequencySlider.getValue(), 0) + " Hz", juce::dontSendNotification);
    osc2FrequencyValueLabel.setText(juce::String(osc2FrequencySlider.getValue(), 0) + " Hz", juce::dontSendNotification);
    osc2DetuneValueLabel.setText(juce::String(osc2DetuneSlider.getValue(), 1) + " cents", juce::dontSendNotification);  // NEW
    attackValueLabel.setText(juce::String(attackSlider.getValue(), 2) + " s", juce::dontSendNotification);
    decayValueLabel.setText(juce::String(decaySlider.getValue(), 2) + " s", juce::dontSendNotification);
    sustainValueLabel.setText(juce::String(sustainSlider.getValue(), 2), juce::dontSendNotification);
    releaseValueLabel.setText(juce::String(releaseSlider.getValue(), 2) + " s", juce::dontSendNotification);
    filterCutoffValueLabel.setText(juce::String(filterCutoffSlider.getValue(), 0) + " Hz", juce::dontSendNotification);
    filterResonanceValueLabel.setText(juce::String(filterResonanceSlider.getValue(), 2), juce::dontSendNotification);


    // Update MIDI status with debug info
    if (audioProcessor.noteOn && audioProcessor.currentMidiNote >= 0)
    {
        midiStatusLabel.setText("MIDI Status: Note Active", juce::dontSendNotification);
        midiStatusLabel.setColour(juce::Label::textColourId, juce::Colours::green);

        // Convert MIDI note to note name
        static const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        int octave = (audioProcessor.currentMidiNote / 12) - 1;
        int note = audioProcessor.currentMidiNote % 12;
        currentNoteLabel.setText(juce::String(noteNames[note]) + juce::String(octave) + " (" + juce::String(audioProcessor.currentMidiNote) + ")", juce::dontSendNotification);

        // Show envelope level
        envelopeLabel.setText("Envelope: " + juce::String(audioProcessor.currentEnvelopeLevel, 2), juce::dontSendNotification);
        envelopeLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
    }
    else
    {
        midiStatusLabel.setText("MIDI Status: No note", juce::dontSendNotification);
        midiStatusLabel.setColour(juce::Label::textColourId, juce::Colours::red);
        currentNoteLabel.setText("Note: --", juce::dontSendNotification);

        // Show envelope level (release phase)
        envelopeLabel.setText("Envelope: " + juce::String(audioProcessor.currentEnvelopeLevel, 2), juce::dontSendNotification);
        envelopeLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    }
}
