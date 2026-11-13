#include "PluginProcessor.h"
#include "PluginEditor.h"

// Costruisce la GUI e collega ogni controllo al parametro corrispondente.
MyVST3PluginAudioProcessorEditor::MyVST3PluginAudioProcessorEditor (MyVST3PluginAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      keyboardComponent (audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setLookAndFeel (&synthLnf);
    setSize (860, 640);
    setWantsKeyboardFocus (true);

    // La tastiera a video non deve leggere i tasti del PC: lo fa l'editor,
    // cosi il focus puo restare su un qualsiasi controllo.
    keyboardComponent.clearKeyMappings();

    // Mappa QWERTY: un semitono per tasto, come nei tracker.
    const char* keys = "awsedftftgyhujkolp;";
    for (int i = 0; i < numComputerKeys && keys[i] != 0; ++i)
    {
        keyMap[i] = juce::KeyPress (static_cast<int> (static_cast<unsigned char> (keys[i])));
        keyOffsets[i] = i;
    }

    // Titolo principale
    titleLabel.setText ("MyVST3Plugin  -  Dual Oscillator Synthesizer", juce::dontSendNotification);
    titleLabel.setFont (juce::Font ("Segoe UI", 16.0f, juce::Font::bold));
    titleLabel.setColour (juce::Label::textColourId, synthLnf.textPrimary);
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel);

    // Etichette di sezione - riga synth
    setupSectionLabel (oscSectionLabel,   "OSCILLATORS",     synthLnf.accentOrange);
    setupSectionLabel (envSectionLabel,   "ENVELOPE",        synthLnf.accentGreen);
    setupSectionLabel (filterSectionLabel,"FILTER",          synthLnf.accentCyan);
    setupSectionLabel (lfoSectionLabel,   "LFO  &  OUTPUT",  synthLnf.accentPurple);

    // Etichette di sezione - riga effetti
    setupSectionLabel (eqSectionLabel,     "EQUALIZER",       synthLnf.accentGreen);
    setupSectionLabel (delaySectionLabel,  "DELAY",           synthLnf.accentOrange);
    setupSectionLabel (reverbSectionLabel, "REVERB",          synthLnf.accentPurple);

    // Label per le combo degli oscillatori
    osc1WaveLabel.setText ("Osc 1", juce::dontSendNotification);
    osc1WaveLabel.setFont (synthLnf.getLabelFont());
    osc1WaveLabel.setColour (juce::Label::textColourId, synthLnf.textSecondary);
    osc1WaveLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (osc1WaveLabel);

    osc2WaveLabel.setText ("Osc 2", juce::dontSendNotification);
    osc2WaveLabel.setFont (synthLnf.getLabelFont());
    osc2WaveLabel.setColour (juce::Label::textColourId, synthLnf.textSecondary);
    osc2WaveLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (osc2WaveLabel);

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

    // EQ 3 bande
    setupSlider (eqLow, "Low", "eqLow");
    setupSlider (eqMid, "Mid", "eqMid");
    setupSlider (eqHigh, "High", "eqHigh");

    // Delay
    setupSlider (delayTime, "Time", "delayTime");
    setupSlider (delayFeedback, "Fdbk", "delayFeedback");
    setupSlider (delayMix, "Mix", "delayMix");

    // Riverbero
    setupSlider (reverbSize, "Size", "reverbSize");
    setupSlider (reverbDamping, "Damp", "reverbDamping");
    setupSlider (reverbMix, "Mix", "reverbMix");

    addAndMakeVisible (keyboardComponent);

    startTimerHz (30);
}

MyVST3PluginAudioProcessorEditor::~MyVST3PluginAudioProcessorEditor()
{
    stopTimer();
    releaseComputerKeys();
    setLookAndFeel (nullptr);
}

// Quando la finestra appare, l'editor riprende il focus da tastiera.
void MyVST3PluginAudioProcessorEditor::visibilityChanged()
{
    if (isVisible())
        grabKeyboardFocus();
}

// Legge lo stato dei tasti del PC e invia note on/off al synth.
void MyVST3PluginAudioProcessorEditor::timerCallback()
{
    // Senza focus (finestra in background) non si suona.
    if (! hasKeyboardFocus (true))
    {
        releaseComputerKeys();
        return;
    }

    // Z = ottava sotto, X = ottava sopra: rilascia le note in corso.
    const bool octaveDown = juce::KeyPress (static_cast<int> ('z')).isCurrentlyDown();
    const bool octaveUp = juce::KeyPress (static_cast<int> ('x')).isCurrentlyDown();

    if (octaveDown != octaveDownWasDown || octaveUp != octaveUpWasDown)
    {
        releaseComputerKeys();

        if (octaveDown && ! octaveDownWasDown)
            octaveShift = juce::jmax (-2, octaveShift - 1);
        if (octaveUp && ! octaveUpWasDown)
            octaveShift = juce::jmin (2, octaveShift + 1);

        octaveDownWasDown = octaveDown;
        octaveUpWasDown = octaveUp;
    }

    const int baseNote = 12 * (4 + octaveShift); // C4 = 60

    for (int i = 0; i < numComputerKeys; ++i)
    {
        const bool down = keyMap[i].isCurrentlyDown();

        if (down == keyDown[i])
            continue;

        keyDown[i] = down;
        const int note = baseNote + keyOffsets[i];

        if (down)
            audioProcessor.keyboardState.noteOn (1, note, 0.8f);
        else
            audioProcessor.keyboardState.noteOff (1, note, 0.0f);
    }
}

// Rilascia le note eventualmente ancora tenute premute.
void MyVST3PluginAudioProcessorEditor::releaseComputerKeys()
{
    const int baseNote = 12 * (4 + octaveShift);

    for (int i = 0; i < numComputerKeys; ++i)
    {
        if (keyDown[i])
            audioProcessor.keyboardState.noteOff (1, baseNote + keyOffsets[i], 0.0f);

        keyDown[i] = false;
    }
}

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

// Crea una label di sezione con stile personalizzato.
void MyVST3PluginAudioProcessorEditor::setupSectionLabel (juce::Label& label, const juce::String& text, const juce::Colour& colour)
{
    label.setText (text, juce::dontSendNotification);
    label.setFont (juce::Font ("Segoe UI", 10.0f, juce::Font::bold));
    label.setColour (juce::Label::textColourId, colour);
    label.setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    label.setColour (juce::Label::outlineColourId, juce::Colours::transparentBlack);
    label.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (label);
}

// =============================================================================
// PAINT - Sfondo con gradiente e pannelli di sezione
// =============================================================================
void MyVST3PluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Sfondo con gradiente verticale scuro
    g.setGradientFill (juce::ColourGradient::vertical(
        synthLnf.backgroundDark.brighter (0.05f),
        synthLnf.backgroundDark.darker (0.1f),
        bounds));
    g.fillRect (bounds);

    const float keyboardHeight = 80.0f;
    auto mainArea = bounds.reduced (10.0f);
    mainArea.removeFromBottom (keyboardHeight + 10.0f);

    // Titolo
    auto titleArea = mainArea.removeFromTop (32.0f);

    // Linea sottile sotto il titolo
    g.setColour (synthLnf.accentWhite.withAlpha (0.1f));
    g.fillRect (
        static_cast<int>(mainArea.getX()),
        static_cast<int>(titleArea.getBottom()) + 2,
        static_cast<int>(mainArea.getWidth()), 1);

    const float gap = 8.0f;
    const float colWidth = (mainArea.getWidth() - gap * 2.0f) / 3.0f;
    const float rowHeight = (mainArea.getHeight() - gap) * 0.5f;

    // --- Riga 1: Synth ---
    auto synthRow = mainArea.removeFromTop (rowHeight);

    auto panel1 = synthRow.removeFromLeft (colWidth);
    auto panel2 = synthRow.removeFromLeft (colWidth);
    auto panel3 = synthRow;

    // --- Riga 2: Effetti ---
    auto fxRow = mainArea;

    auto fx1 = fxRow.removeFromLeft (colWidth);
    auto fx2 = fxRow.removeFromLeft (colWidth);
    auto fx3 = fxRow;

    auto drawPanel = [&g, this] (juce::Rectangle<float> rect, juce::Colour accent) {
        // Ombra
        g.setColour (juce::Colours::black.withAlpha (0.3f));
        g.fillRoundedRectangle (rect.translated (1.5f, 1.5f), 6.0f);
        // Corpo del pannello
        g.setColour (synthLnf.panelDark);
        g.fillRoundedRectangle (rect, 6.0f);
        // Bordo sottile con accento della sezione
        g.setColour (accent.withAlpha (0.15f));
        g.drawRoundedRectangle (rect.reduced (0.5f), 6.0f, 1.0f);
        // Linea accent in alto
        g.setColour (accent.withAlpha (0.6f));
        g.fillRoundedRectangle (rect.removeFromTop (2.0f).reduced (4.0f, 0.0f), 1.0f);
    };

    drawPanel (panel1, synthLnf.accentOrange);
    drawPanel (panel2, synthLnf.accentGreen);
    drawPanel (panel3, synthLnf.accentPurple);

    drawPanel (fx1, synthLnf.accentGreen);
    drawPanel (fx2, synthLnf.accentOrange);
    drawPanel (fx3, synthLnf.accentPurple);

    // Bordo della tastiera
    auto keyboardArea = bounds.reduced (10.0f);
    keyboardArea.removeFromTop (keyboardArea.getHeight() - keyboardHeight);
    g.setColour (synthLnf.panelDark.brighter (0.1f));
    g.fillRoundedRectangle (keyboardArea, 4.0f);
    g.setColour (synthLnf.accentWhite.withAlpha (0.05f));
    g.drawRoundedRectangle (keyboardArea.reduced (0.5f), 4.0f, 1.0f);
}

// =============================================================================
// RESIZED - Layout con sezioni e intestazioni
// =============================================================================
void MyVST3PluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().toFloat().reduced (10.0f);
    const float keyboardHeight = 80.0f;

    // Titolo
    titleLabel.setBounds (area.removeFromTop (28.0f).toNearestInt());

    // Area principale
    auto main = area.removeFromTop (area.getHeight() - keyboardHeight - 6.0f);

    const float gap = 8.0f;
    const float colWidth = (main.getWidth() - gap * 2.0f) / 3.0f;
    const float rowHeight = (main.getHeight() - gap) * 0.5f;

    // --- Riga 1: Synth ---
    auto synthRow = main.removeFromTop (rowHeight);

    // Colonna 1: Oscillatori
    auto col1 = synthRow.removeFromLeft (colWidth);
    auto c1Header = col1.removeFromTop (22.0f);
    oscSectionLabel.setBounds (c1Header.reduced (4.0f, 0.0f).toNearestInt());

    auto c1ComboRow = col1.removeFromTop (24.0f);
    const float comboW = (c1ComboRow.getWidth() - 12.0f) * 0.5f;
    osc1WaveLabel.setBounds (c1ComboRow.removeFromLeft (comboW).reduced (2.0f).toNearestInt());
    osc2WaveLabel.setBounds (c1ComboRow.removeFromLeft (comboW).reduced (2.0f).toNearestInt());
    osc1Wave.setBounds (c1ComboRow.removeFromLeft (comboW).reduced (2.0f).toNearestInt());
    osc2Wave.setBounds (c1ComboRow.reduced (2.0f).toNearestInt());

    auto c1SliderRow = col1;
    const float sliderW = c1SliderRow.getWidth() / 3.0f;
    osc1Level.setBounds (c1SliderRow.removeFromLeft (sliderW).reduced (4.0f).toNearestInt());
    osc2Level.setBounds (c1SliderRow.removeFromLeft (sliderW).reduced (4.0f).toNearestInt());
    osc2Detune.setBounds (c1SliderRow.reduced (4.0f).toNearestInt());

    // Colonna 2: Envelope + Filtro
    auto col2 = synthRow.removeFromLeft (colWidth);

    auto envHeader = col2.removeFromTop (22.0f);
    envSectionLabel.setBounds (envHeader.reduced (4.0f, 0.0f).toNearestInt());

    auto envRow = col2.removeFromTop (col2.getHeight() * 0.5f);
    const float envSliderW = envRow.getWidth() / 4.0f;
    attack.setBounds   (envRow.removeFromLeft (envSliderW).reduced (4.0f).toNearestInt());
    decay.setBounds    (envRow.removeFromLeft (envSliderW).reduced (4.0f).toNearestInt());
    sustain.setBounds  (envRow.removeFromLeft (envSliderW).reduced (4.0f).toNearestInt());
    release.setBounds  (envRow.reduced (4.0f).toNearestInt());

    auto filterHeader = col2.removeFromTop (22.0f);
    filterSectionLabel.setBounds (filterHeader.reduced (4.0f, 0.0f).toNearestInt());

    auto filterRow = col2;
    const float filterSliderW = filterRow.getWidth() / 3.0f;
    cutoff.setBounds     (filterRow.removeFromLeft (filterSliderW).reduced (4.0f).toNearestInt());
    resonance.setBounds  (filterRow.removeFromLeft (filterSliderW).reduced (4.0f).toNearestInt());
    filterEnv.setBounds  (filterRow.reduced (4.0f).toNearestInt());

    // Colonna 3: LFO + Output
    auto col3 = synthRow;

    auto lfoHeader = col3.removeFromTop (22.0f);
    lfoSectionLabel.setBounds (lfoHeader.reduced (4.0f, 0.0f).toNearestInt());

    auto c3ComboRow = col3.removeFromTop (24.0f);
    lfoTarget.setBounds (c3ComboRow.reduced (4.0f).toNearestInt());

    auto c3SliderRow = col3.removeFromTop (col3.getHeight() * 0.5f);
    const float lfoSliderW = c3SliderRow.getWidth() / 2.0f;
    lfoRate.setBounds  (c3SliderRow.removeFromLeft (lfoSliderW).reduced (4.0f).toNearestInt());
    lfoDepth.setBounds (c3SliderRow.reduced (4.0f).toNearestInt());

    auto c3OutputRow = col3;
    gain.setBounds (c3OutputRow.reduced (4.0f).toNearestInt());

    // --- Riga 2: Effetti ---
    auto fxRow = main;

    // EQ
    auto eqCol = fxRow.removeFromLeft (colWidth);
    auto eqHeader = eqCol.removeFromTop (22.0f);
    eqSectionLabel.setBounds (eqHeader.reduced (4.0f, 0.0f).toNearestInt());
    auto eqRow = eqCol;
    const float eqSliderW = eqRow.getWidth() / 3.0f;
    eqLow.setBounds  (eqRow.removeFromLeft (eqSliderW).reduced (4.0f).toNearestInt());
    eqMid.setBounds  (eqRow.removeFromLeft (eqSliderW).reduced (4.0f).toNearestInt());
    eqHigh.setBounds (eqRow.reduced (4.0f).toNearestInt());

    // Delay
    auto delayCol = fxRow.removeFromLeft (colWidth);
    auto delayHeader = delayCol.removeFromTop (22.0f);
    delaySectionLabel.setBounds (delayHeader.reduced (4.0f, 0.0f).toNearestInt());
    auto delayRow = delayCol;
    const float delaySliderW = delayRow.getWidth() / 3.0f;
    delayTime.setBounds      (delayRow.removeFromLeft (delaySliderW).reduced (4.0f).toNearestInt());
    delayFeedback.setBounds  (delayRow.removeFromLeft (delaySliderW).reduced (4.0f).toNearestInt());
    delayMix.setBounds       (delayRow.reduced (4.0f).toNearestInt());

    // Riverbero
    auto reverbCol = fxRow;
    auto reverbHeader = reverbCol.removeFromTop (22.0f);
    reverbSectionLabel.setBounds (reverbHeader.reduced (4.0f, 0.0f).toNearestInt());
    auto reverbRow = reverbCol;
    const float reverbSliderW = reverbRow.getWidth() / 3.0f;
    reverbSize.setBounds    (reverbRow.removeFromLeft (reverbSliderW).reduced (4.0f).toNearestInt());
    reverbDamping.setBounds (reverbRow.removeFromLeft (reverbSliderW).reduced (4.0f).toNearestInt());
    reverbMix.setBounds     (reverbRow.reduced (4.0f).toNearestInt());

    // --- Tastiera ---
    auto keyboardArea = area;
    keyboardComponent.setBounds (keyboardArea.toNearestInt());

    // DEBUG temporaneo: dimensioni reali del layout.
    juce::File ("C:/Users/user/AppData/Local/Temp/kilo/editor_debug.txt")
        .replaceWithText ("editor=" + juce::String (getWidth()) + "x" + juce::String (getHeight())
                          + " keyboard=" + keyboardComponent.getBounds().toString() + "\n");
}