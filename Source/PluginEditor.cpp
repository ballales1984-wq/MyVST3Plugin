#include "PluginProcessor.h"
#include "PluginEditor.h"

// Costruisce la GUI e collega ogni controllo al parametro corrispondente.
MyVST3PluginAudioProcessorEditor::MyVST3PluginAudioProcessorEditor (MyVST3PluginAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      keyboardComponent (audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize (760, 420);

    titleLabel.setText ("MyVST3Plugin - Dual Osc Synth", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (18.0f, juce::Font::bold));
    titleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel);

    // Voci dei menu a tendina.
    juce::StringArray waves { "Sine", "Square", "Saw", "Triangle" };
    juce::StringArray targets { "Pitch", "Filter", "Amp" };

    setupCombo (osc1Wave, waves, "osc1Wave");
    setupCombo (osc2Wave, waves, "osc2Wave");
    setupCombo (lfoTarget, targets, "lfoTarget");

    // Sezione oscillatori.
    setupSlider (osc1Level, "Osc1", "osc1Level");
    setupSlider (osc2Level, "Osc2", "osc2Level");
    setupSlider (osc2Detune, "Detune", "osc2Detune");

    // Sezione inviluppo ADSR.
    setupSlider (attack, "A", "attack");
    setupSlider (decay, "D", "decay");
    setupSlider (sustain, "S", "sustain");
    setupSlider (release, "R", "release");

    // Sezione filtro.
    setupSlider (cutoff, "Cutoff", "cutoff");
    setupSlider (resonance, "Res", "resonance");
    setupSlider (filterEnv, "F.Env", "filterEnv");

    // Sezione LFO e uscita.
    setupSlider (lfoRate, "Rate", "lfoRate");
    setupSlider (lfoDepth, "Depth", "lfoDepth");
    setupSlider (gain, "Gain", "gain");

    addAndMakeVisible (keyboardComponent);
}

MyVST3PluginAudioProcessorEditor::~MyVST3PluginAudioProcessorEditor() = default;

// Slider rotativo con casella di testo e attachment al parametro.
void MyVST3PluginAudioProcessorEditor::setupSlider (juce::Slider& s, const juce::String& name, const char* paramId)
{
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 56, 16);
    s.setName (name);
    addAndMakeVisible (s);
    sliderAtts.push_back (std::make_unique<SliderAtt> (audioProcessor.parameters, paramId, s));
}

// Combo box con elenco di voci e attachment al parametro.
void MyVST3PluginAudioProcessorEditor::setupCombo (juce::ComboBox& c, const juce::StringArray& items, const char* paramId)
{
    c.addItemList (items, 1);
    addAndMakeVisible (c);
    comboAtts.push_back (std::make_unique<ComboAtt> (audioProcessor.parameters, paramId, c));
}

// Disegna lo sfondo e i tre pannelli che raggruppano i controlli.
void MyVST3PluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkslategrey.darker (0.4f));
    g.setColour (juce::Colours::white.withAlpha (0.08f));
    auto area = getLocalBounds().reduced (8);
    auto top = area.removeFromTop (260);
    auto col1 = top.removeFromLeft (top.getWidth() / 3).reduced (4);
    auto col2 = top.removeFromLeft (top.getWidth() / 2).reduced (4);
    auto col3 = top.reduced (4);
    g.fillRoundedRectangle (col1.toFloat(), 6.0f);
    g.fillRoundedRectangle (col2.toFloat(), 6.0f);
    g.fillRoundedRectangle (col3.toFloat(), 6.0f);
}

// Layout: titolo, tre colonne di controlli e tastiera in basso.
void MyVST3PluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (8);
    titleLabel.setBounds (area.removeFromTop (28));
    auto main = area.removeFromTop (250);
    auto keyboard = area;

    // Distribuisce i controlli di una riga in parti uguali.
    auto placeRow = [] (juce::Rectangle<int> r, std::initializer_list<juce::Component*> comps) {
        const int w = r.getWidth() / static_cast<int> (comps.size());
        for (auto* c : comps)
        {
            c->setBounds (r.removeFromLeft (w).reduced (4));
        }
    };

    // Colonna 1: oscillatori.
    auto col1 = main.removeFromLeft (main.getWidth() / 3).reduced (4);
    auto c1r1 = col1.removeFromTop (28);
    osc1Wave.setBounds (c1r1.removeFromLeft (c1r1.getWidth() / 2).reduced (2));
    osc2Wave.setBounds (c1r1.reduced (2));
    placeRow (col1.removeFromTop (110), { &osc1Level, &osc2Level, &osc2Detune });

    // Colonna 2: inviluppo e filtro.
    auto col2 = main.removeFromLeft (main.getWidth() / 2).reduced (4);
    placeRow (col2.removeFromTop (110), { &attack, &decay, &sustain, &release });
    placeRow (col2, { &cutoff, &resonance, &filterEnv });

    // Colonna 3: LFO e guadagno.
    auto col3 = main.reduced (4);
    lfoTarget.setBounds (col3.removeFromTop (28).reduced (2));
    placeRow (col3, { &lfoRate, &lfoDepth, &gain });

    keyboardComponent.setBounds (keyboard.reduced (2));
}
