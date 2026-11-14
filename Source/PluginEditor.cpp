#include "PluginProcessor.h"
#include "PluginEditor.h"

// Costruisce la GUI e collega ogni controllo al parametro corrispondente.
MyVST3PluginAudioProcessorEditor::MyVST3PluginAudioProcessorEditor (MyVST3PluginAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p)
{
    setLookAndFeel (&synthLnf);
    setSize (900, 700);
    setWantsKeyboardFocus (true);

    // Mappa QWERTY: un semitono per tasto, come nei tracker.
    const char* keys = "awsedftgyhujkolp;";
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

    // --- Barra dei preset ---
    presetLabel.setText ("PRESET", juce::dontSendNotification);
    presetLabel.setFont (synthLnf.getLabelFont());
    presetLabel.setColour (juce::Label::textColourId, synthLnf.textSecondary);
    presetLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (presetLabel);

    presetBox.setTextWhenNothingSelected ("nessun preset salvato");
    addAndMakeVisible (presetBox);
    presetBox.onChange = [this] { loadPreset (presetBox.getSelectedId()); };

    auto styleButton = [this] (juce::TextButton& b, const juce::String& text)
    {
        b.setButtonText (text);
        addAndMakeVisible (b);
    };

    styleButton (presetSaveButton,    "Save");
    styleButton (presetSaveAsButton,  "Save As");
    styleButton (presetRenameButton,  "Rename");
    styleButton (presetDeleteButton,  "Delete");

    presetSaveButton.onClick    = [this] { savePreset (false); };
    presetSaveAsButton.onClick  = [this] { savePreset (true); };
    presetRenameButton.onClick  = [this] { renamePreset(); };
    presetDeleteButton.onClick  = [this] { deletePreset(); };

    refreshPresetBox();

    // Voci dei menu a tendina.
    juce::StringArray waves { "Sine", "Square", "Saw", "Triangle", "Pulse", "FM", "Organ" };
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

    // Aggiorna la tastiera disegnata (30 fps)
    if (keyboardRect.getWidth() > 0.0f)
        repaint (keyboardRect.toNearestInt());
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
// PAINT - Sfondo con gradiente, pannelli di sezione e tastiera disegnata
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

    const float margin = 10.0f;
    const float titleH = 30.0f;
    const float kbH = 90.0f;

    auto area = bounds.reduced (margin);
    const float titleBottom = area.getY() + titleH;
    const float kbTop = area.getBottom() - kbH;

    // Linea sotto il titolo
    g.setColour (synthLnf.accentWhite.withAlpha (0.1f));
    g.fillRect (static_cast<int>(area.getX()), static_cast<int>(titleBottom) + 2,
                static_cast<int>(area.getWidth()), 1);

    // Due righe di pannelli: synth (sopra) e fx (sotto)
    const float gap = 8.0f;
    const float panelAreaTop = titleBottom + 8.0f;
    const float panelAreaH = kbTop - 8.0f - panelAreaTop;
    const float rowH = (panelAreaH - gap) * 0.5f;
    const float colW = (area.getWidth() - gap * 2.0f) / 3.0f;

    auto drawPanel = [&g, this] (juce::Rectangle<float> r, juce::Colour accent) {
        g.setColour (juce::Colours::black.withAlpha (0.3f));
        g.fillRoundedRectangle (r.translated (1.5f, 1.5f), 6.0f);
        g.setColour (synthLnf.panelDark);
        g.fillRoundedRectangle (r, 6.0f);
        g.setColour (accent.withAlpha (0.15f));
        g.drawRoundedRectangle (r.reduced (0.5f), 6.0f, 1.0f);
        g.setColour (accent.withAlpha (0.6f));
        auto top = r.removeFromTop (2.0f);
        g.fillRoundedRectangle (top.reduced (4.0f, 0.0f), 1.0f);
    };

    // Riga synth
    float ry = panelAreaTop;
    drawPanel ({ area.getX(), ry, colW, rowH }, synthLnf.accentOrange);
    drawPanel ({ area.getX() + colW + gap, ry, colW, rowH }, synthLnf.accentGreen);
    drawPanel ({ area.getX() + (colW + gap) * 2.0f, ry, colW, rowH }, synthLnf.accentPurple);

    // Riga fx
    ry += rowH + gap;
    drawPanel ({ area.getX(), ry, colW, rowH }, synthLnf.accentGreen);
    drawPanel ({ area.getX() + colW + gap, ry, colW, rowH }, synthLnf.accentOrange);
    drawPanel ({ area.getX() + (colW + gap) * 2.0f, ry, colW, rowH }, synthLnf.accentPurple);

    // Pannello tastiera
    keyboardRect = { area.getX(), kbTop + 4.0f, area.getWidth(), kbH - 8.0f };
    g.setColour (synthLnf.panelDark.brighter (0.1f));
    g.fillRoundedRectangle (keyboardRect, 4.0f);
    g.setColour (synthLnf.accentWhite.withAlpha (0.06f));
    g.drawRoundedRectangle (keyboardRect.reduced (0.5f), 4.0f, 1.0f);

    // Tastiera disegnata
    drawKeyboard (g, keyboardRect);
}

// =============================================================================
// Tastiera disegnata dall'editor
// =============================================================================
static bool isBlackKey (int midiNote)
{
    const int pc = ((midiNote % 12) + 12) % 12;
    return (pc == 1 || pc == 3 || pc == 6 || pc == 8 || pc == 10);
}

void MyVST3PluginAudioProcessorEditor::drawKeyboard (juce::Graphics& g, juce::Rectangle<float> area)
{
    const int startNote = 21;  // A0
    const int endNote   = 108; // C8 (88 tasti)
    const int numWhite  = 52;  // tasti bianchi in 88 tasti

    const float whiteW = area.getWidth() / static_cast<float>(numWhite);
    const float blackW = whiteW * 0.62f;
    const float blackH = area.getHeight() * 0.62f;

    // Tasti bianchi
    int whiteIndex = 0;
    for (int note = startNote; note <= endNote; ++note)
    {
        if (isBlackKey (note))
            continue;

        float x = area.getX() + static_cast<float>(whiteIndex) * whiteW;
        juce::Rectangle<float> keyRect (x, area.getY(), whiteW - 1.0f, area.getHeight());

        const bool down = audioProcessor.keyboardState.isNoteOn (1, note);
        if (down)
        {
            g.setColour (synthLnf.accentWhite.withAlpha (0.9f));
            g.fillRoundedRectangle (keyRect, 2.0f);
        }
        else
        {
            g.setColour (juce::Colour (0xffd8d8e4));
            g.fillRoundedRectangle (keyRect, 2.0f);
        }

        g.setColour (juce::Colour (0xff060610).withAlpha (0.5f));
        g.drawRoundedRectangle (keyRect.reduced (0.5f), 2.0f, 0.8f);

        ++whiteIndex;
    }

    // Tasti neri
    whiteIndex = 0;
    for (int note = startNote; note <= endNote; ++note)
    {
        if (isBlackKey (note))
        {
            // posizione: sopra il bordo destro del tasto bianco precedente
            float x = area.getX() + static_cast<float>(whiteIndex) * whiteW - blackW * 0.5f;
            juce::Rectangle<float> keyRect (x, area.getY(), blackW, blackH);

            const bool down = audioProcessor.keyboardState.isNoteOn (1, note);
            g.setColour (down ? synthLnf.accentWhite : juce::Colour (0xff14141f));
            g.fillRoundedRectangle (keyRect, 2.0f);
            g.setColour (juce::Colour (0xff060610).withAlpha (0.8f));
            g.drawRoundedRectangle (keyRect.reduced (0.5f), 2.0f, 0.8f);
        }
        else
        {
            ++whiteIndex;
        }
    }
}

// Determina la nota MIDI in base alla posizione del mouse.
int MyVST3PluginAudioProcessorEditor::noteAtPosition (float x, float y) const
{
    const int startNote = 21;
    const int endNote   = 108;
    const int numWhite  = 52;

    const float whiteW = keyboardRect.getWidth() / static_cast<float>(numWhite);
    const float blackW = whiteW * 0.62f;
    const float blackH = keyboardRect.getHeight() * 0.62f;

    if (! keyboardRect.contains (static_cast<int>(x), static_cast<int>(y)))
        return -1;

    const float localX = x - keyboardRect.getX();
    const float localY = y - keyboardRect.getY();

    // Prima controlla i tasti neri (sono sopra)
    int whiteIndex = 0;
    for (int note = startNote; note <= endNote; ++note)
    {
        if (isBlackKey (note))
        {
            float bx = static_cast<float>(whiteIndex) * whiteW - blackW * 0.5f;
            juce::Rectangle<float> kr (bx, 0.0f, blackW, blackH);
            if (kr.contains (localX, localY))
                return note;
        }
        else
        {
            ++whiteIndex;
        }
    }

    // Poi i tasti bianchi
    whiteIndex = 0;
    for (int note = startNote; note <= endNote; ++note)
    {
        if (isBlackKey (note))
            continue;
        float bx = static_cast<float>(whiteIndex) * whiteW;
        juce::Rectangle<float> kr (bx, 0.0f, whiteW, keyboardRect.getHeight());
        if (kr.contains (localX, localY))
            return note;
        ++whiteIndex;
    }

    return -1;
}

void MyVST3PluginAudioProcessorEditor::noteOnFromMouse (int note)
{
    if (note > 0 && ! audioProcessor.keyboardState.isNoteOn (1, note))
        audioProcessor.keyboardState.noteOn (1, note, 0.8f);
}

void MyVST3PluginAudioProcessorEditor::noteOffFromMouse (int note)
{
    if (note > 0 && audioProcessor.keyboardState.isNoteOn (1, note))
        audioProcessor.keyboardState.noteOff (1, note, 0.0f);
}

void MyVST3PluginAudioProcessorEditor::mouseDown (const juce::MouseEvent& e)
{
    mouseNote = noteAtPosition (e.position.x, e.position.y);
    noteOnFromMouse (mouseNote);
    repaint();
}

void MyVST3PluginAudioProcessorEditor::mouseDrag (const juce::MouseEvent& e)
{
    const int note = noteAtPosition (e.position.x, e.position.y);
    if (note != mouseNote)
    {
        noteOffFromMouse (mouseNote);
        mouseNote = note;
        noteOnFromMouse (mouseNote);
        repaint();
    }
}

void MyVST3PluginAudioProcessorEditor::mouseUp (const juce::MouseEvent&)
{
    noteOffFromMouse (mouseNote);
    mouseNote = -1;
    repaint();
}

// =============================================================================
// RESIZED - Layout con sezioni e intestazioni
// =============================================================================
void MyVST3PluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().toFloat().reduced (10.0f);
    const float titleH = 30.0f;
    const float presetH = 30.0f;
    const float kbH = 90.0f;
    const float gap = 8.0f;

    const float titleBottom = area.getY() + titleH;
    const float presetTop = titleBottom + 4.0f;
    const float kbTop = area.getBottom() - kbH;
    const float panelAreaTop = presetTop + presetH + 8.0f;
    const float panelAreaH = kbTop - 8.0f - panelAreaTop;
    const float rowH = (panelAreaH - gap) * 0.5f;
    const float colW = (area.getWidth() - gap * 2.0f) / 3.0f;

    // Titolo
    titleLabel.setBounds (area.getX(), area.getY(), area.getWidth(), titleH);

    // Barra dei preset: etichetta, elenco e pulsanti
    {
        auto bar = juce::Rectangle<float> (area.getX(), presetTop, area.getWidth(), presetH);
        const float btnW = 76.0f;

        presetDeleteButton.setBounds (bar.removeFromRight (btnW).reduced (2.0f).toNearestInt());
        presetRenameButton.setBounds (bar.removeFromRight (btnW).reduced (2.0f).toNearestInt());
        presetSaveAsButton.setBounds (bar.removeFromRight (btnW).reduced (2.0f).toNearestInt());
        presetSaveButton.setBounds   (bar.removeFromRight (btnW).reduced (2.0f).toNearestInt());
        presetLabel.setBounds        (bar.removeFromLeft (64.0f).reduced (2.0f).toNearestInt());
        presetBox.setBounds          (bar.reduced (2.0f).toNearestInt());
    }

    auto placeRow = [] (juce::Rectangle<float> r, std::initializer_list<juce::Component*> comps) {
        const int n = static_cast<int>(comps.size());
        const float w = r.getWidth() / static_cast<float>(n);
        for (auto* c : comps)
        {
            c->setBounds (r.removeFromLeft (w).reduced (4.0f).toNearestInt());
        }
    };

    // --- Riga 1: Synth ---
    float ry = panelAreaTop;

    // Colonna 1: Oscillatori
    {
        auto col = juce::Rectangle<float> (area.getX(), ry, colW, rowH);
        auto header = col.removeFromTop (22.0f);
        oscSectionLabel.setBounds (header.reduced (4.0f, 0.0f).toNearestInt());

        auto comboRow = col.removeFromTop (24.0f);
        // Metà sinistra: label "Osc 1" + combo forma d'onda
        auto osc1 = comboRow.removeFromLeft (comboRow.getWidth() * 0.5f);
        osc1WaveLabel.setBounds (osc1.removeFromLeft (34.0f).reduced (2.0f).toNearestInt());
        osc1Wave.setBounds (osc1.reduced (2.0f).toNearestInt());
        // Metà destra: label "Osc 2" + combo forma d'onda
        osc2WaveLabel.setBounds (comboRow.removeFromLeft (34.0f).reduced (2.0f).toNearestInt());
        osc2Wave.setBounds (comboRow.reduced (2.0f).toNearestInt());

        placeRow (col, { &osc1Level, &osc2Level, &osc2Detune });
    }

    // Colonna 2: Envelope + Filtro
    {
        auto col = juce::Rectangle<float> (area.getX() + colW + gap, ry, colW, rowH);

        auto envHeader = col.removeFromTop (22.0f);
        envSectionLabel.setBounds (envHeader.reduced (4.0f, 0.0f).toNearestInt());

        auto envRow = col.removeFromTop (col.getHeight() * 0.5f);
        placeRow (envRow, { &attack, &decay, &sustain, &release });

        auto filterHeader = col.removeFromTop (22.0f);
        filterSectionLabel.setBounds (filterHeader.reduced (4.0f, 0.0f).toNearestInt());

        placeRow (col, { &cutoff, &resonance, &filterEnv });
    }

    // Colonna 3: LFO + Output
    {
        auto col = juce::Rectangle<float> (area.getX() + (colW + gap) * 2.0f, ry, colW, rowH);

        auto lfoHeader = col.removeFromTop (22.0f);
        lfoSectionLabel.setBounds (lfoHeader.reduced (4.0f, 0.0f).toNearestInt());

        auto comboRow = col.removeFromTop (24.0f);
        lfoTarget.setBounds (comboRow.reduced (4.0f).toNearestInt());

        auto lfoRow = col.removeFromTop (col.getHeight() * 0.5f);
        placeRow (lfoRow, { &lfoRate, &lfoDepth });

        placeRow (col, { &gain });
    }

    // --- Riga 2: Effetti ---
    ry += rowH + gap;

    // EQ
    {
        auto col = juce::Rectangle<float> (area.getX(), ry, colW, rowH);
        auto header = col.removeFromTop (22.0f);
        eqSectionLabel.setBounds (header.reduced (4.0f, 0.0f).toNearestInt());
        placeRow (col, { &eqLow, &eqMid, &eqHigh });
    }

    // Delay
    {
        auto col = juce::Rectangle<float> (area.getX() + colW + gap, ry, colW, rowH);
        auto header = col.removeFromTop (22.0f);
        delaySectionLabel.setBounds (header.reduced (4.0f, 0.0f).toNearestInt());
        placeRow (col, { &delayTime, &delayFeedback, &delayMix });
    }

    // Riverbero
    {
        auto col = juce::Rectangle<float> (area.getX() + (colW + gap) * 2.0f, ry, colW, rowH);
        auto header = col.removeFromTop (22.0f);
        reverbSectionLabel.setBounds (header.reduced (4.0f, 0.0f).toNearestInt());
        placeRow (col, { &reverbSize, &reverbDamping, &reverbMix });
    }

    // La tastiera viene disegnata in paint(); aggiorna il rettangolo.
    keyboardRect = { area.getX(), kbTop + 4.0f, area.getWidth(), kbH - 8.0f };
}

// =============================================================================
// PRESET - salvataggio, caricamento, rinomina ed eliminazione
// =============================================================================
void MyVST3PluginAudioProcessorEditor::showNameDialog (const juce::String& title,
                                                       const juce::String& suggested,
                                                       std::function<void (juce::String)> onDone)
{
    // Dialogo asincrono: i modal loop sono disabilitati nei plugin.
    auto* dialog = new juce::AlertWindow (title,
                                          "Inserisci il nome del preset:",
                                          juce::AlertWindow::QuestionIcon,
                                          this);

    dialog->addTextEditor ("name", suggested, "Nome:");
    dialog->addButton ("OK", 1, juce::KeyPress (juce::KeyPress::returnKey));
    dialog->addButton ("Annulla", 0, juce::KeyPress (juce::KeyPress::escapeKey));

    struct NameCallback : juce::ModalComponentManager::Callback
    {
        NameCallback (juce::AlertWindow* w, std::function<void (juce::String)> f)
            : window (w), fn (std::move (f)) {}

        void modalStateFinished (int result) override
        {
            juce::String name;

            if (result == 1 && window != nullptr)
                name = window->getTextEditorContents ("name").trim();

            if (fn)
                fn (name);
        }

        juce::AlertWindow* window;
        std::function<void (juce::String)> fn;
    };

    dialog->enterModalState (true, new NameCallback (dialog, onDone), true);
}

void MyVST3PluginAudioProcessorEditor::refreshPresetBox()
{
    presetBox.clear (juce::dontSendNotification);

    for (int i = 0; i < presetManager.getNumPresets(); ++i)
    {
        const auto* preset = presetManager.getPreset (i);
        presetBox.addItem (preset->getLabel(), preset->number);
    }

    if (presetManager.getIndexForNumber (currentPresetNumber) >= 0)
    {
        presetBox.setSelectedId (currentPresetNumber, juce::dontSendNotification);
    }
    else
    {
        currentPresetNumber = 0;
        presetBox.setTextWhenNothingSelected ("nessun preset salvato");
    }
}

void MyVST3PluginAudioProcessorEditor::loadPreset (int number)
{
    if (number <= 0)
        return;

    auto state = presetManager.loadPreset (number);

    if (state == nullptr)
        return;

    const auto tree = juce::ValueTree::fromXml (*state);

    if (! tree.isValid())
        return;

    audioProcessor.parameters.replaceState (tree);
    currentPresetNumber = number;
    presetBox.setSelectedId (number, juce::dontSendNotification);
}

void MyVST3PluginAudioProcessorEditor::savePreset (bool asNew)
{
    auto stateXml = audioProcessor.parameters.copyState().createXml();

    if (stateXml == nullptr)
        return;

    const int currentIndex = presetManager.getIndexForNumber (currentPresetNumber);

    // Sovrascrive il preset selezionato mantenendo numero e nome.
    if (! asNew && currentIndex >= 0)
    {
        const auto* preset = presetManager.getPreset (currentIndex);
        const int number = preset->number;
        const juce::String name = preset->name;

        if (presetManager.savePreset (number, name, *stateXml))
        {
            currentPresetNumber = number;
            refreshPresetBox();
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                                    "Preset",
                                                    "Salvataggio non riuscito.",
                                                    "OK",
                                                    this);
        }

        return;
    }

    // Nuovo preset: il numero viene assegnato automaticamente.
    const int nextNumber = presetManager.getNextNumber();

    // shared_ptr: la callback deve restare copiabile (std::function).
    auto stateToSave = std::make_shared<std::unique_ptr<juce::XmlElement>> (std::move (stateXml));

    showNameDialog ("Salva preset",
                    "Preset " + juce::String (nextNumber),
                    [this, stateToSave, nextNumber] (juce::String name)
                    {
                        if (name.isEmpty())
                            return;

                        if (presetManager.savePreset (nextNumber, name, **stateToSave))
                        {
                            currentPresetNumber = nextNumber;
                            refreshPresetBox();
                        }
                        else
                        {
                            juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                                                    "Preset",
                                                                    "Salvataggio non riuscito.",
                                                                    "OK",
                                                                    this);
                        }
                    });
}

void MyVST3PluginAudioProcessorEditor::renamePreset()
{
    const int currentIndex = presetManager.getIndexForNumber (currentPresetNumber);

    if (currentIndex < 0)
        return;

    const auto* preset = presetManager.getPreset (currentIndex);
    const int number = preset->number;
    const juce::String currentName = preset->name;

    showNameDialog ("Rinomina preset",
                    currentName,
                    [this, number, currentName] (juce::String newName)
                    {
                        if (newName.isEmpty() || newName == currentName)
                            return;

                        if (presetManager.renamePreset (number, newName))
                            refreshPresetBox();
                    });
}

void MyVST3PluginAudioProcessorEditor::deletePreset()
{
    const int currentIndex = presetManager.getIndexForNumber (currentPresetNumber);

    if (currentIndex < 0)
        return;

    const auto* preset = presetManager.getPreset (currentIndex);
    const int number = preset->number;
    const juce::String name = preset->name;

    struct DeleteCallback : juce::ModalComponentManager::Callback
    {
        DeleteCallback (MyVST3PluginAudioProcessorEditor& e, int n)
            : editor (e), presetNumber (n) {}

        void modalStateFinished (int result) override
        {
            if (result != 1)
                return;

            editor.presetManager.deletePreset (presetNumber);
            editor.currentPresetNumber = 0;
            editor.refreshPresetBox();
        }

        MyVST3PluginAudioProcessorEditor& editor;
        int presetNumber;
    };

    juce::AlertWindow::showOkCancelBox (juce::AlertWindow::QuestionIcon,
                                       "Elimina preset",
                                       "Eliminare il preset \"" + name + "\"?",
                                       "Elimina",
                                       "Annulla",
                                       this,
                                       new DeleteCallback (*this, number));
}