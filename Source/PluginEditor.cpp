#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyVST3PluginAudioProcessorEditor::MyVST3PluginAudioProcessorEditor (MyVST3PluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      keyboardComponent (audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Set window size for 5 columns with waveform controls
    setSize (900, 650); // Optimized compact layout

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

    // Setup Square PWM slider (NEW)
    setupSlider(squarePWMSlider, squarePWMLabel, squarePWMValueLabel,
                "Square PWM", 0.1f, 0.9f, 0.5f, "%");

    // TEST: Both waveform selectors now
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

    // Setup advanced filter controls
    setupFilterTypeSelector(filterTypeSelector, filterTypeLabel, "Filter Type");
    setupSlider(filterDriveSlider, filterDriveLabel, filterDriveValueLabel,
                "Filter Drive", 0.0f, 1.0f, 0.0f, "");

    // Setup LFO controls
    setupSlider(lfoRateSlider, lfoRateLabel, lfoRateValueLabel,
                "LFO Rate", 0.1f, 20.0f, 1.0f, " Hz");

    setupWaveformSelector(lfoWaveformSelector, lfoWaveformLabel, "LFO Waveform");

    setupSlider(lfoAmountSlider, lfoAmountLabel, lfoAmountValueLabel,
                "LFO Amount", 0.0f, 1.0f, 0.0f, "");

    // Setup LFO routing buttons
    lfoRoutingLabel.setText("LFO Routing", juce::dontSendNotification);
    lfoRoutingLabel.setFont(juce::Font(12.0f));
    lfoRoutingLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lfoRoutingLabel);

    lfoToOsc1Button.setButtonText("OSC1");
    lfoToOsc1Button.setColour(juce::ToggleButton::tickColourId, juce::Colours::orange);
    addAndMakeVisible(lfoToOsc1Button);

    lfoToAmpButton.setButtonText("AMP");
    lfoToAmpButton.setColour(juce::ToggleButton::tickColourId, juce::Colours::orange);
    addAndMakeVisible(lfoToAmpButton);

    // Setup Third Oscillator enable button (FM modulation)
    osc3EnabledButton.setButtonText("FM ON");
    osc3EnabledButton.setColour(juce::ToggleButton::tickColourId, juce::Colours::purple);
    addAndMakeVisible(osc3EnabledButton);

    // Setup Third Oscillator frequency slider (FM modulation)
    setupSlider(osc3FrequencySlider, osc3FrequencyLabel, osc3FrequencyValueLabel, "FM Freq", 20.0f, 2000.0f, 330.0f, " Hz");

    // Setup Third Oscillator mix slider (FM modulation intensity)
    setupSlider(osc3MixSlider, osc3MixLabel, osc3MixValueLabel, "FM Mix", 0.0f, 1.0f, 0.0f, "");

    // Setup test mode button
    testModeButton.setButtonText("TEST MODE");
    testModeButton.setColour(juce::ToggleButton::textColourId, juce::Colours::green);
    addAndMakeVisible(testModeButton);

    // Create parameter attachments
    masterVolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramMasterVolume, masterVolumeSlider);

    osc1FrequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramOsc1Frequency, osc1FrequencySlider);

    osc2FrequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramOsc2Frequency, osc2FrequencySlider);

    osc2DetuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(  // NEW
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramOsc2Detune, osc2DetuneSlider);

    squarePWMAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(  // NEW
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramSquarePWM, squarePWMSlider);

    // TEST: Both waveform attachments now
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

    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramFilterType, filterTypeSelector);

    filterDriveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramFilterDrive, filterDriveSlider);

    // LFO attachments
    lfoRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramLfoRate, lfoRateSlider);

    lfoAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramLfoAmount, lfoAmountSlider);

    lfoToOsc1Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramLfoToOsc1, lfoToOsc1Button);

    lfoToAmpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramLfoToAmp, lfoToAmpButton);

    testModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramTestMode, testModeButton);

    // LFO Attachments
    lfoRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramLfoRate, lfoRateSlider);

    lfoWaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramLfoWaveform, lfoWaveformSelector);

    lfoAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramLfoAmount, lfoAmountSlider);

    lfoToOsc1Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramLfoToOsc1, lfoToOsc1Button);

    lfoToAmpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramLfoToAmp, lfoToAmpButton);

    osc3EnabledAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramOsc3Enabled, osc3EnabledButton);

    osc3FrequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramOsc3Frequency, osc3FrequencySlider);

    osc3MixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramOsc3Mix, osc3MixSlider);

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

    // Add all controls directly to main editor (simplified approach)
    addAndMakeVisible(osc1FrequencyLabel);
    addAndMakeVisible(osc1FrequencySlider);
    addAndMakeVisible(osc1FrequencyValueLabel);
    addAndMakeVisible(osc1WaveformLabel);
    addAndMakeVisible(osc1WaveformSelector);

    addAndMakeVisible(osc2FrequencyLabel);
    addAndMakeVisible(osc2FrequencySlider);
    addAndMakeVisible(osc2FrequencyValueLabel);
    addAndMakeVisible(osc2WaveformLabel);
    addAndMakeVisible(osc2WaveformSelector);

    addAndMakeVisible(osc2DetuneLabel);
    addAndMakeVisible(osc2DetuneSlider);
    addAndMakeVisible(osc2DetuneValueLabel);

    addAndMakeVisible(masterVolumeLabel);
    addAndMakeVisible(masterVolumeSlider);
    addAndMakeVisible(masterVolumeValueLabel);

    addAndMakeVisible(attackLabel);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(attackValueLabel);

    addAndMakeVisible(decayLabel);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(decayValueLabel);

    addAndMakeVisible(sustainLabel);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(sustainValueLabel);

    addAndMakeVisible(releaseLabel);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(releaseValueLabel);

    addAndMakeVisible(filterCutoffLabel);
    addAndMakeVisible(filterCutoffSlider);
    addAndMakeVisible(filterCutoffValueLabel);

    addAndMakeVisible(filterResonanceLabel);
    addAndMakeVisible(filterResonanceSlider);
    addAndMakeVisible(filterResonanceValueLabel);

    // Add LFO controls
    addAndMakeVisible(lfoRateLabel);
    addAndMakeVisible(lfoRateSlider);
    addAndMakeVisible(lfoRateValueLabel);

    addAndMakeVisible(lfoAmountLabel);
    addAndMakeVisible(lfoAmountSlider);
    addAndMakeVisible(lfoAmountValueLabel);

    addAndMakeVisible(lfoToOsc1Button);
    addAndMakeVisible(lfoToAmpButton);

    // Setup test mode button
    testModeButton.setButtonText("🎵 TEST MODE 🎵");
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
    auto bounds = getLocalBounds().reduced(10);

    // Title at the top
    titleLabel.setBounds(bounds.removeFromTop(25));

    // Test Mode button below title
    testModeButton.setBounds(bounds.removeFromTop(30));

    // Keyboard component at the bottom
    auto keyboardArea = bounds.removeFromBottom(80);
    keyboardComponent.setBounds(keyboardArea);

    // Now work with the remaining area for controls
    auto area = bounds;

    // =======================================================================
    // SIMPLIFIED SINGLE-VIEW LAYOUT
    // =======================================================================

    const int margin = 8;
    const int controlWidth = (area.getWidth() - margin * 5) / 4;  // 4 controls per row
    const int rowHeight = 45; // More compact layout

    int y = area.getY();

    // Row 1: OSC1 Frequency + OSC1 Waveform
    osc1FrequencyLabel.setBounds(margin, y, controlWidth, 15);
    osc1FrequencySlider.setBounds(margin, y + 15, controlWidth, 30);
    osc1FrequencyValueLabel.setBounds(margin, y + 45, controlWidth, 15);

    osc1WaveformLabel.setBounds(margin + controlWidth + margin, y, controlWidth, 15);
    osc1WaveformSelector.setBounds(margin + controlWidth + margin, y + 15, controlWidth, 45);

    // Row 2: OSC2 Frequency + OSC2 Waveform
    y += rowHeight;
    osc2FrequencyLabel.setBounds(margin, y, controlWidth, 15);
    osc2FrequencySlider.setBounds(margin, y + 15, controlWidth, 30);
    osc2FrequencyValueLabel.setBounds(margin, y + 45, controlWidth, 15);

    osc2WaveformLabel.setBounds(margin + controlWidth + margin, y, controlWidth, 15);
    osc2WaveformSelector.setBounds(margin + controlWidth + margin, y + 15, controlWidth, 45);

    // Row 3: Detune + Master Volume
    y += rowHeight;
    osc2DetuneLabel.setBounds(margin, y, controlWidth, 15);
    osc2DetuneSlider.setBounds(margin, y + 15, controlWidth, 30);
    osc2DetuneValueLabel.setBounds(margin, y + 45, controlWidth, 15);

    squarePWMLabel.setBounds(margin + controlWidth + margin, y, controlWidth, 15);
    squarePWMSlider.setBounds(margin + controlWidth + margin, y + 15, controlWidth, 30);
    squarePWMValueLabel.setBounds(margin + controlWidth + margin, y + 45, controlWidth, 15);

    // Row 3.5: Master Volume (spostato in nuova riga)
    y += rowHeight;
    masterVolumeLabel.setBounds(margin, y, controlWidth, 15);
    masterVolumeSlider.setBounds(margin, y + 15, controlWidth, 30);
    masterVolumeValueLabel.setBounds(margin, y + 45, controlWidth, 15);

    // Row 4: ADSR - Attack + Decay
    y += rowHeight;
    attackLabel.setBounds(margin, y, controlWidth, 15);
    attackSlider.setBounds(margin, y + 15, controlWidth, 30);
    attackValueLabel.setBounds(margin, y + 45, controlWidth, 15);

    decayLabel.setBounds(margin + controlWidth + margin, y, controlWidth, 15);
    decaySlider.setBounds(margin + controlWidth + margin, y + 15, controlWidth, 30);
    decayValueLabel.setBounds(margin + controlWidth + margin, y + 45, controlWidth, 15);

    // Row 5: ADSR - Sustain + Release
    y += rowHeight;
    sustainLabel.setBounds(margin, y, controlWidth, 15);
    sustainSlider.setBounds(margin, y + 15, controlWidth, 30);
    sustainValueLabel.setBounds(margin, y + 45, controlWidth, 15);

    releaseLabel.setBounds(margin + controlWidth + margin, y, controlWidth, 15);
    releaseSlider.setBounds(margin + controlWidth + margin, y + 15, controlWidth, 30);
    releaseValueLabel.setBounds(margin + controlWidth + margin, y + 45, controlWidth, 15);

    // Row 6: Filter Cutoff + Filter Type
    y += rowHeight;
    filterCutoffLabel.setBounds(margin, y, controlWidth, 15);
    filterCutoffSlider.setBounds(margin, y + 15, controlWidth, 30);
    filterCutoffValueLabel.setBounds(margin, y + 45, controlWidth, 15);

    filterTypeLabel.setBounds(margin + controlWidth + margin, y, controlWidth, 15);
    filterTypeSelector.setBounds(margin + controlWidth + margin, y + 15, controlWidth, 45);

    // Row 7: Filter Resonance + Filter Drive
    y += rowHeight;
    filterResonanceLabel.setBounds(margin, y, controlWidth, 15);
    filterResonanceSlider.setBounds(margin, y + 15, controlWidth, 30);
    filterResonanceValueLabel.setBounds(margin, y + 45, controlWidth, 15);

    filterDriveLabel.setBounds(margin + controlWidth + margin, y, controlWidth, 15);
    filterDriveSlider.setBounds(margin + controlWidth + margin, y + 15, controlWidth, 30);
    filterDriveValueLabel.setBounds(margin + controlWidth + margin, y + 45, controlWidth, 15);

    // Row 8: LFO Rate + LFO Amount
    y += rowHeight;
    lfoRateLabel.setBounds(margin, y, controlWidth, 15);
    lfoRateSlider.setBounds(margin, y + 15, controlWidth, 30);
    lfoRateValueLabel.setBounds(margin, y + 45, controlWidth, 15);

    lfoAmountLabel.setBounds(margin + controlWidth + margin, y, controlWidth, 15);
    lfoAmountSlider.setBounds(margin + controlWidth + margin, y + 15, controlWidth, 30);
    lfoAmountValueLabel.setBounds(margin + controlWidth + margin, y + 45, controlWidth, 15);

    // LFO routing buttons (compact)
    lfoToOsc1Button.setBounds(margin + (controlWidth + margin) * 2, y + 15, controlWidth / 2 - 2, 25);
    lfoToAmpButton.setBounds(margin + (controlWidth + margin) * 2 + controlWidth / 2 + 2, y + 15, controlWidth / 2 - 2, 25);

    // FM ON button next to LFO buttons (column 3)
    osc3EnabledButton.setBounds(margin + (controlWidth + margin) * 3, y + 15, controlWidth - 10, 25);

    // FM controls in next row (more compact)
    y += rowHeight;
    // FM Freq - left side
    osc3FrequencySlider.setBounds(margin, y + 25, controlWidth, 35);
    osc3FrequencyLabel.setBounds(margin, y + 5, controlWidth, 20);
    osc3FrequencyValueLabel.setBounds(margin, y + 60, controlWidth, 15);

    // FM Mix - right side
    osc3MixSlider.setBounds(margin + controlWidth + margin, y + 25, controlWidth, 35);
    osc3MixLabel.setBounds(margin + controlWidth + margin, y + 5, controlWidth, 20);
    osc3MixValueLabel.setBounds(margin + controlWidth + margin, y + 60, controlWidth, 15);
}

// Simplified single-view layout

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

void MyVST3PluginAudioProcessorEditor::setupFilterTypeSelector(juce::ComboBox& comboBox, juce::Label& label, const juce::String& paramName)
{
    // Setup label
    label.setText(paramName, juce::dontSendNotification);
    label.setFont(juce::Font(12.0f));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);

    // Setup ComboBox for filter types
    comboBox.addItem("LPF", 1);     // Low-Pass Filter
    comboBox.addItem("HPF", 2);     // High-Pass Filter
    comboBox.addItem("BPF", 3);     // Band-Pass Filter
    comboBox.addItem("Notch", 4);   // Notch Filter
    comboBox.setSelectedItemIndex(0); // Default to LPF
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

    // Update LFO value labels (simplified)
    lfoRateValueLabel.setText(juce::String(lfoRateSlider.getValue(), 1) + " Hz", juce::dontSendNotification);
    lfoAmountValueLabel.setText(juce::String((int)(lfoAmountSlider.getValue() * 100)) + "%", juce::dontSendNotification);

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
