#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyVST3PluginAudioProcessorEditor::MyVST3PluginAudioProcessorEditor (MyVST3PluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      keyboardComponent (audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Set window size (increased for Test Mode button)
    setSize (500, 600); // Increased height to accommodate the keyboard

    // Setup title
    titleLabel.setText("Dual Oscillator Synth V2.0", juce::dontSendNotification);
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

    // Setup ADSR sliders
    setupSlider(attackSlider, attackLabel, attackValueLabel,
               "Attack", 0.001f, 5.0f, 0.1f, " s");

    setupSlider(decaySlider, decayLabel, decayValueLabel,
               "Decay", 0.001f, 5.0f, 0.2f, " s");

    setupSlider(sustainSlider, sustainLabel, sustainValueLabel,
               "Sustain", 0.0f, 1.0f, 0.8f, "");

    setupSlider(releaseSlider, releaseLabel, releaseValueLabel,
               "Release", 0.001f, 5.0f, 0.3f, " s");


    // Create parameter attachments
    masterVolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramMasterVolume, masterVolumeSlider);

    osc1FrequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramOsc1Frequency, osc1FrequencySlider);

    osc2FrequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramOsc2Frequency, osc2FrequencySlider);

    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramAttack, attackSlider);

    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramDecay, decaySlider);

    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramSustain, sustainSlider);

    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), MyVST3PluginAudioProcessor::paramRelease, releaseSlider);

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

    // Keyboard component
    keyboardComponent.setBounds(area.removeFromTop(80)); // Allocate 80 pixels for the keyboard

    // MIDI and envelope status area
    auto statusArea = area.removeFromTop(70);
    auto midiArea = statusArea.removeFromTop(35);
    midiStatusLabel.setBounds(midiArea.removeFromTop(17));
    currentNoteLabel.setBounds(midiArea);
    envelopeLabel.setBounds(statusArea);

    area.removeFromTop(10); // Spacing

    // Layout in a 3x3 grid for all controls
    const int numRows = 3;
    const int numCols = 3;
    const int controlHeight = 80;
    const int controlWidth = area.getWidth() / numCols;

    // Row 1: Oscillators and Master Volume
    for (int col = 0; col < numCols; ++col)
    {
        auto cellArea = area.removeFromTop(controlHeight).withWidth(controlWidth).withX(col * controlWidth);

        switch (col)
        {
            case 0: // Osc1 Frequency
                osc1FrequencyLabel.setBounds(cellArea.removeFromTop(20));
                osc1FrequencySlider.setBounds(cellArea.removeFromTop(30));
                osc1FrequencyValueLabel.setBounds(cellArea);
                break;
            case 1: // Osc2 Frequency
                osc2FrequencyLabel.setBounds(cellArea.removeFromTop(20));
                osc2FrequencySlider.setBounds(cellArea.removeFromTop(30));
                osc2FrequencyValueLabel.setBounds(cellArea);
                break;
            case 2: // Master Volume
                masterVolumeLabel.setBounds(cellArea.removeFromTop(20));
                masterVolumeSlider.setBounds(cellArea.removeFromTop(30));
                masterVolumeValueLabel.setBounds(cellArea);
                break;
        }
    }

    area.removeFromTop(10); // Spacing between rows

    // Row 2: ADSR controls
    for (int col = 0; col < numCols; ++col)
    {
        auto cellArea = area.removeFromTop(controlHeight).withWidth(controlWidth).withX(col * controlWidth);

        switch (col)
        {
            case 0: // Attack
                attackLabel.setBounds(cellArea.removeFromTop(20));
                attackSlider.setBounds(cellArea.removeFromTop(30));
                attackValueLabel.setBounds(cellArea);
                break;
            case 1: // Decay
                decayLabel.setBounds(cellArea.removeFromTop(20));
                decaySlider.setBounds(cellArea.removeFromTop(30));
                decayValueLabel.setBounds(cellArea);
                break;
            case 2: // Sustain
                sustainLabel.setBounds(cellArea.removeFromTop(20));
                sustainSlider.setBounds(cellArea.removeFromTop(30));
                sustainValueLabel.setBounds(cellArea);
                break;
        }
    }

    area.removeFromTop(10); // Spacing between rows

    // Row 3: Release control
    auto cellArea = area.removeFromTop(controlHeight);
    releaseLabel.setBounds(cellArea.removeFromTop(20));
    releaseSlider.setBounds(cellArea.removeFromTop(30));
    releaseValueLabel.setBounds(cellArea);

    // Test mode button at the bottom (larger)
    area.removeFromTop(10); // Spacing
    testModeButton.setBounds(area.removeFromTop(40));
}

void MyVST3PluginAudioProcessorEditor::timerCallback()
{
    updateValueLabels();
}

//==============================================================================
// Helper methods
void MyVST3PluginAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label,
                                                  juce::Label& valueLabel, const juce::String& paramName,
                                                  float minValue, float maxValue, float defaultValue,
                                                  const juce::String& unit)
{
    // Setup label
    label.setText(paramName, juce::dontSendNotification);
    label.setFont(juce::Font(14.0f));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);

    // Setup slider
    slider.setRange(minValue, maxValue, 0.001);
    slider.setValue(defaultValue);
    slider.setTextValueSuffix(unit);
    slider.setNumDecimalPlacesToDisplay(2);
    addAndMakeVisible(slider);

    // Setup value label
    valueLabel.setText(juce::String(defaultValue) + unit, juce::dontSendNotification);
    valueLabel.setFont(juce::Font(12.0f));
    valueLabel.setJustificationType(juce::Justification::centred);
    valueLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    addAndMakeVisible(valueLabel);
}

void MyVST3PluginAudioProcessorEditor::updateValueLabels()
{
    // Update parameter value labels
    masterVolumeValueLabel.setText(juce::String(masterVolumeSlider.getValue(), 2), juce::dontSendNotification);
    osc1FrequencyValueLabel.setText(juce::String(osc1FrequencySlider.getValue(), 0) + " Hz", juce::dontSendNotification);
    osc2FrequencyValueLabel.setText(juce::String(osc2FrequencySlider.getValue(), 0) + " Hz", juce::dontSendNotification);
    attackValueLabel.setText(juce::String(attackSlider.getValue(), 2) + " s", juce::dontSendNotification);
    decayValueLabel.setText(juce::String(decaySlider.getValue(), 2) + " s", juce::dontSendNotification);
    sustainValueLabel.setText(juce::String(sustainSlider.getValue(), 2), juce::dontSendNotification);
    releaseValueLabel.setText(juce::String(releaseSlider.getValue(), 2) + " s", juce::dontSendNotification);


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
