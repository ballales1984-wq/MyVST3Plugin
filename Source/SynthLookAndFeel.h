#pragma once

#include <JuceHeader.h>

/** LookAndFeel personalizzato per il sintetizzatore:
 *  - Knobs rotativi moderni con arco colorato e glow
 *  - Combo box con freccia e bordo sottile
 *  - Slider text box con stile minimalista
 *  - Tastiera MIDI con tema scuro
 *  - Colori per sezione (oscillatori, inviluppo, filtro, LFO) */
class SynthLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SynthLookAndFeel();
    ~SynthLookAndFeel() override = default;

    /** Disegna un knob rotativo circolare con arco colorato e glow. */
    void drawRotarySlider (juce::Graphics& g,
                           int x, int y, int width, int height,
                           float sliderPos,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider& slider) override;

    /** Crea il text box sotto il knob con stile minimalista. */
    juce::Label* createSliderTextBox (juce::Slider& slider) override;

    /** Disegna una combo box con bordo sottile e freccia. */
    void drawComboBox (juce::Graphics& g,
                       int width, int height,
                       bool isButtonDown,
                       int buttonX, int buttonY,
                       int buttonW, int buttonH,
                       juce::ComboBox& box) override;

    /** Font per il testo della combo box. */
    juce::Font getComboBoxFont (juce::ComboBox& box) override;

    // Palette colori accessibili per il paint dell'editor
    juce::Colour backgroundDark;
    juce::Colour panelDark;
    juce::Colour textPrimary;
    juce::Colour textSecondary;
    juce::Colour accentOrange;   // Oscillatori
    juce::Colour accentCyan;     // Filtro
    juce::Colour accentGreen;    // Inviluppo
    juce::Colour accentPurple;   // LFO
    juce::Colour accentWhite;    // Generico

    juce::Font getLabelFont() const { return labelFont; }

private:
    juce::Colour knobBody;
    juce::Colour knobHighlight;
    juce::Colour knobShadow;
    juce::Font knobFont;
    juce::Font comboFont;
    juce::Font labelFont;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthLookAndFeel)
};