#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyVST3PluginAudioProcessorEditor::MyVST3PluginAudioProcessorEditor (MyVST3PluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      keyboardComponent (audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize (600, 300);

    // Title
    titleLabel.setText("MyVST3Plugin v1.0 - Clean Version", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Test button
    testModeButton.setButtonText("TEST MODE");
    testModeButton.setClickingTogglesState(true);
    testModeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    testModeButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    testModeButton.onClick = [this]() {
        auto* param = audioProcessor.parameters.getRawParameterValue("testMode");
        if (param)
            param->store(testModeButton.getToggleState() ? 1.0f : 0.0f);
    };
    addAndMakeVisible(testModeButton);

    // Keyboard
    addAndMakeVisible(keyboardComponent);
}

MyVST3PluginAudioProcessorEditor::~MyVST3PluginAudioProcessorEditor()
{
}

//==============================================================================
void MyVST3PluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
}

void MyVST3PluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    titleLabel.setBounds(bounds.removeFromTop(30));
    testModeButton.setBounds(bounds.removeFromTop(40).reduced(100, 5));
    keyboardComponent.setBounds(bounds.reduced(5));
}
