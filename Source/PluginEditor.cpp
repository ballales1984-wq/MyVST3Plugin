#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyVST3PluginAudioProcessorEditor::MyVST3PluginAudioProcessorEditor (MyVST3PluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      keyboardComponent (audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Set window size for 5 columns with waveform controls
    setSize (1000, 700); // Optimized for tabbed interface

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

    // Create tabbed interface
    addAndMakeVisible(mainTabs);
    mainTabs.setTabBarDepth(30);
    mainTabs.setOutline(1);

    // Create tab components
    createOscillatorsTab();
    createEnvelopeTab();
    createModulationTab();

    // Setup test mode button (now in modulation tab)
    testModeButton.setButtonText("🎵 TEST MODE 🎵");
    testModeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    testModeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    // Test mode button will be added to modulation tab

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
    auto area = getLocalBounds().reduced(5);

    // Title at the top
    titleLabel.setBounds(area.removeFromTop(25));

    // Keyboard component at the bottom
    auto keyboardArea = area.removeFromBottom(80);
    keyboardComponent.setBounds(keyboardArea);

    // Main tabs take up the remaining space
    mainTabs.setBounds(area);

    // =======================================================================
    // LAYOUT CONTROLLI NEI TAB
    // =======================================================================

    const int tabWidth = area.getWidth();
    const int controlWidth = tabWidth / 4;
    const int controlHeight = 70;

    // Layout for Oscillators Tab
    if (oscillatorsTab)
    {
        auto oscArea = oscillatorsTab->getLocalBounds().reduced(10);

        // OSC1 controls - top row
        osc1FrequencyLabel.setBounds(oscArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        osc1FrequencySlider.setBounds(oscArea.removeFromTop(35).removeFromLeft(controlWidth).reduced(5));
        osc1FrequencyValueLabel.setBounds(oscArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        osc1WaveformLabel.setBounds(oscArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        osc1WaveformSelector.setBounds(oscArea.removeFromTop(35).removeFromLeft(controlWidth).reduced(5));

        // OSC2 controls - second row
        osc2FrequencyLabel.setBounds(oscArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        osc2FrequencySlider.setBounds(oscArea.removeFromTop(35).removeFromLeft(controlWidth).reduced(5));
        osc2FrequencyValueLabel.setBounds(oscArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        osc2WaveformLabel.setBounds(oscArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        osc2WaveformSelector.setBounds(oscArea.removeFromTop(35).removeFromLeft(controlWidth).reduced(5));

        // Detune and Master - bottom row
        osc2DetuneLabel.setBounds(oscArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        osc2DetuneSlider.setBounds(oscArea.removeFromTop(35).removeFromLeft(controlWidth).reduced(5));
        osc2DetuneValueLabel.setBounds(oscArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        masterVolumeLabel.setBounds(oscArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        masterVolumeSlider.setBounds(oscArea.removeFromTop(35).removeFromLeft(controlWidth).reduced(5));
        masterVolumeValueLabel.setBounds(oscArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
    }

    // Layout for Envelope Tab
    if (envelopeTab)
    {
        auto envArea = envelopeTab->getLocalBounds().reduced(10);

        // ADSR controls
        attackLabel.setBounds(envArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        attackSlider.setBounds(envArea.removeFromTop(35).removeFromLeft(controlWidth).reduced(5));
        attackValueLabel.setBounds(envArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));

        decayLabel.setBounds(envArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        decaySlider.setBounds(envArea.removeFromTop(35).removeFromLeft(controlWidth).reduced(5));
        decayValueLabel.setBounds(envArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));

        sustainLabel.setBounds(envArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        sustainSlider.setBounds(envArea.removeFromTop(35).removeFromLeft(controlWidth).reduced(5));
        sustainValueLabel.setBounds(envArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));

        releaseLabel.setBounds(envArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        releaseSlider.setBounds(envArea.removeFromTop(35).removeFromLeft(controlWidth).reduced(5));
        releaseValueLabel.setBounds(envArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));

        // Filter controls
        filterCutoffLabel.setBounds(envArea.removeFromTop(15).removeFromLeft(controlWidth * 2).reduced(5));
        filterCutoffSlider.setBounds(envArea.removeFromTop(35).removeFromLeft(controlWidth * 2).reduced(5));
        filterCutoffValueLabel.setBounds(envArea.removeFromTop(15).removeFromLeft(controlWidth * 2).reduced(5));

        filterResonanceLabel.setBounds(envArea.removeFromTop(15).removeFromLeft(controlWidth * 2).reduced(5));
        filterResonanceSlider.setBounds(envArea.removeFromTop(35).removeFromLeft(controlWidth * 2).reduced(5));
        filterResonanceValueLabel.setBounds(envArea.removeFromTop(15).removeFromLeft(controlWidth * 2).reduced(5));
    }

    // Layout for Modulation Tab
    if (modulationTab)
    {
        auto modArea = modulationTab->getLocalBounds().reduced(10);

        // LFO controls
        lfoRateLabel.setBounds(modArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        lfoRateSlider.setBounds(modArea.removeFromTop(35).removeFromLeft(controlWidth).reduced(5));
        lfoRateValueLabel.setBounds(modArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));

        lfoWaveformLabel.setBounds(modArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        lfoWaveformSelector.setBounds(modArea.removeFromTop(35).removeFromLeft(controlWidth).reduced(5));

        lfoAmountLabel.setBounds(modArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        lfoAmountSlider.setBounds(modArea.removeFromTop(35).removeFromLeft(controlWidth).reduced(5));
        lfoAmountValueLabel.setBounds(modArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));

        // LFO routing
        lfoRoutingLabel.setBounds(modArea.removeFromTop(15).removeFromLeft(controlWidth).reduced(5));
        lfoToOsc1Button.setBounds(modArea.removeFromTop(20).removeFromLeft(controlWidth).reduced(5));
        lfoToAmpButton.setBounds(modArea.removeFromTop(20).removeFromLeft(controlWidth).reduced(5));

        // Test mode
        testModeButton.setBounds(modArea.removeFromTop(25).reduced(5));
    }
}

//==============================================================================
// Tab creation methods
//==============================================================================

void MyVST3PluginAudioProcessorEditor::createOscillatorsTab()
{
    oscillatorsTab = std::make_unique<juce::Component>();
    oscillatorsTab->setName("Oscillators");

    // Add controls to oscillators tab
    oscillatorsTab->addAndMakeVisible(osc1FrequencyLabel);
    oscillatorsTab->addAndMakeVisible(osc1FrequencySlider);
    oscillatorsTab->addAndMakeVisible(osc1FrequencyValueLabel);
    oscillatorsTab->addAndMakeVisible(osc1WaveformLabel);
    oscillatorsTab->addAndMakeVisible(osc1WaveformSelector);

    oscillatorsTab->addAndMakeVisible(osc2FrequencyLabel);
    oscillatorsTab->addAndMakeVisible(osc2FrequencySlider);
    oscillatorsTab->addAndMakeVisible(osc2FrequencyValueLabel);
    oscillatorsTab->addAndMakeVisible(osc2WaveformLabel);
    oscillatorsTab->addAndMakeVisible(osc2WaveformSelector);

    oscillatorsTab->addAndMakeVisible(osc2DetuneLabel);
    oscillatorsTab->addAndMakeVisible(osc2DetuneSlider);
    oscillatorsTab->addAndMakeVisible(osc2DetuneValueLabel);

    oscillatorsTab->addAndMakeVisible(masterVolumeLabel);
    oscillatorsTab->addAndMakeVisible(masterVolumeSlider);
    oscillatorsTab->addAndMakeVisible(masterVolumeValueLabel);

    mainTabs.addTab("🎛️ Oscillators", juce::Colours::darkgrey, oscillatorsTab.get(), false);
}

void MyVST3PluginAudioProcessorEditor::createEnvelopeTab()
{
    envelopeTab = std::make_unique<juce::Component>();
    envelopeTab->setName("Envelope");

    // Add controls to envelope tab
    envelopeTab->addAndMakeVisible(attackLabel);
    envelopeTab->addAndMakeVisible(attackSlider);
    envelopeTab->addAndMakeVisible(attackValueLabel);

    envelopeTab->addAndMakeVisible(decayLabel);
    envelopeTab->addAndMakeVisible(decaySlider);
    envelopeTab->addAndMakeVisible(decayValueLabel);

    envelopeTab->addAndMakeVisible(sustainLabel);
    envelopeTab->addAndMakeVisible(sustainSlider);
    envelopeTab->addAndMakeVisible(sustainValueLabel);

    envelopeTab->addAndMakeVisible(releaseLabel);
    envelopeTab->addAndMakeVisible(releaseSlider);
    envelopeTab->addAndMakeVisible(releaseValueLabel);

    envelopeTab->addAndMakeVisible(filterCutoffLabel);
    envelopeTab->addAndMakeVisible(filterCutoffSlider);
    envelopeTab->addAndMakeVisible(filterCutoffValueLabel);

    envelopeTab->addAndMakeVisible(filterResonanceLabel);
    envelopeTab->addAndMakeVisible(filterResonanceSlider);
    envelopeTab->addAndMakeVisible(filterResonanceValueLabel);

    mainTabs.addTab("🎚️ Envelope & Filter", juce::Colours::darkgrey, envelopeTab.get(), false);
}

void MyVST3PluginAudioProcessorEditor::createModulationTab()
{
    modulationTab = std::make_unique<juce::Component>();
    modulationTab->setName("Modulation");

    // Add controls to modulation tab
    modulationTab->addAndMakeVisible(lfoRateLabel);
    modulationTab->addAndMakeVisible(lfoRateSlider);
    modulationTab->addAndMakeVisible(lfoRateValueLabel);

    modulationTab->addAndMakeVisible(lfoWaveformLabel);
    modulationTab->addAndMakeVisible(lfoWaveformSelector);

    modulationTab->addAndMakeVisible(lfoAmountLabel);
    modulationTab->addAndMakeVisible(lfoAmountSlider);
    modulationTab->addAndMakeVisible(lfoAmountValueLabel);

    modulationTab->addAndMakeVisible(lfoRoutingLabel);
    modulationTab->addAndMakeVisible(lfoToOsc1Button);
    modulationTab->addAndMakeVisible(lfoToAmpButton);

    modulationTab->addAndMakeVisible(testModeButton);

    mainTabs.addTab("⚡ Modulation", juce::Colours::darkgrey, modulationTab.get(), false);
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
