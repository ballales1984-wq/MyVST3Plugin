#include "SynthLookAndFeel.h"
#include <cmath>

SynthLookAndFeel::SynthLookAndFeel()
{
    // Palette colori - tema scuro moderno
    backgroundDark = juce::Colour (0xff0e0e16);
    panelDark      = juce::Colour (0xff161624);
    knobBody       = juce::Colour (0xff1e1e32);
    knobHighlight  = juce::Colour (0xff2a2a44);
    knobShadow     = juce::Colour (0xff060610);
    textPrimary    = juce::Colour (0xffe8e8f0);
    textSecondary  = juce::Colour (0xff7878a0);
    accentOrange   = juce::Colour (0xffff8c42);
    accentCyan     = juce::Colour (0xff00d4ff);
    accentGreen    = juce::Colour (0xff44ff88);
    accentPurple   = juce::Colour (0xffc44dff);
    accentWhite    = juce::Colour (0xffdceeff);

    // Font
    knobFont  = juce::Font ("Segoe UI", 10.0f, juce::Font::bold);
    comboFont = juce::Font ("Segoe UI", 12.0f, juce::Font::plain);
    labelFont = juce::Font ("Segoe UI", 11.0f, juce::Font::bold);

    // Colori per la tastiera MIDI
    setColour (juce::MidiKeyboardComponent::whiteNoteColourId,
               juce::Colour (0xffc8c8d8));
    setColour (juce::MidiKeyboardComponent::blackNoteColourId,
               juce::Colour (0xff1a1a28));
    setColour (juce::MidiKeyboardComponent::keyDownOverlayColourId,
               accentWhite.withAlpha (0.7f));
    setColour (juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId,
               juce::Colours::white.withAlpha (0.08f));
    setColour (juce::MidiKeyboardComponent::keySeparatorLineColourId,
               juce::Colour (0xff404060));
    setColour (juce::MidiKeyboardComponent::textLabelColourId,
               textSecondary);
    setColour (juce::MidiKeyboardComponent::shadowColourId,
               juce::Colours::transparentBlack);
}

// =============================================================================
// KNOB ROTATIVO - Disegno realistico con gradiente metallico, smussi, tick
// =============================================================================
void SynthLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                          int x, int y, int width, int height,
                                          float sliderPos,
                                          float rotaryStartAngle,
                                          float rotaryEndAngle,
                                          juce::Slider& slider)
{
    // Determina il colore di sezione basandosi sul nome del parametro
    juce::Colour accent = accentWhite;
    auto name = slider.getName();
    if (name.containsIgnoreCase ("Osc") || name.containsIgnoreCase ("Detune"))
        accent = accentOrange;
    else if (name.containsIgnoreCase ("cut") || name.containsIgnoreCase ("res") || name.containsIgnoreCase ("F.Env"))
        accent = accentCyan;
    else if (name.containsIgnoreCase ("A") || name.containsIgnoreCase ("D") || name.containsIgnoreCase ("S") || name.containsIgnoreCase ("R")
          || name.containsIgnoreCase ("Low") || name.containsIgnoreCase ("Mid") || name.containsIgnoreCase ("High"))
        accent = accentGreen;
    else if (name.containsIgnoreCase ("Rate") || name.containsIgnoreCase ("Depth")
          || name.containsIgnoreCase ("Target") || name.containsIgnoreCase ("Size") || name.containsIgnoreCase ("Damp") || name.containsIgnoreCase ("Mix"))
        accent = accentPurple;
    else if (name.containsIgnoreCase ("Time") || name.containsIgnoreCase ("Fdbk"))
        accent = accentOrange;
    else if (name.containsIgnoreCase ("Gain"))
        accent = accentWhite;

    // Area del knob (ridotta)
    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (6);
    const float cx = bounds.getCentreX();
    const float cy = bounds.getCentreY();
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;

    const float toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const float lineW = juce::jmin (3.5f, radius * 0.2f);
    const float arcRadius = radius - lineW * 0.5f - 1.0f;
    const float knobRadius = arcRadius - lineW * 0.5f - 2.0f;

    // --- Ombra esterna (drop shadow) ---
    g.setColour (knobShadow.withAlpha (0.5f));
    g.fillEllipse (cx - arcRadius + 2.5f, cy - arcRadius + 3.0f,
                   arcRadius * 2.0f, arcRadius * 2.0f);

    // --- Tick marks (linee di riferimento) ---
    const int numTicks = 11;
    for (int i = 0; i < numTicks; ++i)
    {
        const float t = static_cast<float>(i) / static_cast<float>(numTicks - 1);
        const float tickAngle = rotaryStartAngle + t * (rotaryEndAngle - rotaryStartAngle);
        const float cosA = std::cos (tickAngle - juce::MathConstants<float>::halfPi);
        const float sinA = std::sin (tickAngle - juce::MathConstants<float>::halfPi);

        const float innerR = arcRadius + lineW * 0.5f + 1.5f;
        const float outerR = innerR + (i % 5 == 0) ? 4.0f : 2.5f;

        g.setColour (knobShadow.withAlpha (0.4f));
        g.drawLine (cx + cosA * innerR, cy + sinA * innerR,
                    cx + cosA * outerR, cy + sinA * outerR,
                    (i % 5 == 0) ? 1.5f : 0.8f);
    }

    // --- Corpo del knob con gradiente metallico radiale ---
    {
        juce::ColourGradient gradient (
            knobHighlight.brighter (0.1f),
            cx - knobRadius * 0.3f, cy - knobRadius * 0.3f,
            knobBody.darker (0.2f),
            cx + knobRadius * 0.4f, cy + knobRadius * 0.4f,
            false);
        gradient.addColour (0.0, knobBody.brighter (0.05f));
        gradient.addColour (0.5, knobBody);
        gradient.addColour (1.0, knobBody.darker (0.15f));

        g.setGradientFill (gradient);
        g.fillEllipse (cx - knobRadius, cy - knobRadius,
                       knobRadius * 2.0f, knobRadius * 2.0f);
    }

    // --- Highlight superiore (riflesso luce) ---
    g.setColour (knobHighlight.withAlpha (0.15f));
    g.fillEllipse (cx - knobRadius + 2.0f, cy - knobRadius + 2.0f,
                   knobRadius * 2.0f - 4.0f, knobRadius * 1.2f);

    // --- Bordo esterno del corpo (smusso) ---
    g.setColour (knobShadow.withAlpha (0.8f));
    g.drawEllipse (cx - knobRadius, cy - knobRadius,
                   knobRadius * 2.0f, knobRadius * 2.0f, 1.2f);

    // --- Bordo interno highlight (smusso alto) ---
    g.setColour (knobHighlight.withAlpha (0.2f));
    g.drawEllipse (cx - knobRadius + 1.5f, cy - knobRadius + 1.5f,
                   knobRadius * 2.0f - 3.0f, knobRadius * 2.0f - 3.0f, 0.8f);

    // --- Traccia di sfondo dell'arco (grigio scuro) ---
    juce::Path trackArc;
    trackArc.addCentredArc (cx, cy, arcRadius, arcRadius,
                            0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (knobShadow.withAlpha (0.6f));
    g.strokePath (trackArc,
                  juce::PathStrokeType (lineW,
                                        juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));

    // --- Arco di valore (colorato con gradiente) ---
    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc (cx, cy, arcRadius, arcRadius,
                                0.0f, rotaryStartAngle, toAngle, true);
        g.setColour (accent);
        g.strokePath (valueArc,
                      juce::PathStrokeType (lineW,
                                            juce::PathStrokeType::curved,
                                            juce::PathStrokeType::rounded));

        // --- Glow attorno all'arco di valore ---
        juce::Path glowArc;
        glowArc.addCentredArc (cx, cy, arcRadius + 1.5f, arcRadius + 1.5f,
                               0.0f, rotaryStartAngle, toAngle, true);
        g.setColour (accent.withAlpha (0.12f));
        g.strokePath (glowArc,
                      juce::PathStrokeType (lineW + 4.0f,
                                            juce::PathStrokeType::curved,
                                            juce::PathStrokeType::rounded));
    }

    // --- Indicatore di posizione (linea dal centro) ---
    {
        const float cosA = std::cos (toAngle - juce::MathConstants<float>::halfPi);
        const float sinA = std::sin (toAngle - juce::MathConstants<float>::halfPi);
        const float lineStart = knobRadius * 0.25f;
        const float lineEnd = knobRadius * 0.65f;

        // Linea scura di base
        g.setColour (knobShadow.withAlpha (0.6f));
        g.drawLine (cx + cosA * lineStart, cy + sinA * lineStart,
                    cx + cosA * lineEnd, cy + sinA * lineEnd, 2.5f);
        // Linea colorata
        g.setColour (accent);
        g.drawLine (cx + cosA * lineStart, cy + sinA * lineStart,
                    cx + cosA * lineEnd, cy + sinA * lineEnd, 1.5f);
    }

    // --- Punto centrale ---
    g.setColour (accent.withAlpha (0.6f));
    g.fillEllipse (cx - 1.5f, cy - 1.5f, 3.0f, 3.0f);
    g.setColour (knobShadow.withAlpha (0.5f));
    g.drawEllipse (cx - 1.5f, cy - 1.5f, 3.0f, 3.0f, 0.5f);
}

// =============================================================================
// TEXT BOX per slider - stile minimalista
// =============================================================================
juce::Label* SynthLookAndFeel::createSliderTextBox (juce::Slider& /*slider*/)
{
    auto* label = new juce::Label();
    label->setJustificationType (juce::Justification::centred);
    label->setFont (knobFont);
    label->setColour (juce::Label::textColourId, textPrimary);
    label->setColour (juce::Label::backgroundColourId, juce::Colour());
    label->setColour (juce::Label::outlineColourId, juce::Colour());
    label->setEditable (false);
    label->setInterceptsMouseClicks (false, false);
    return label;
}

// =============================================================================
// COMBO BOX - bordo sottile e freccia personalizzata
// =============================================================================
void SynthLookAndFeel::drawComboBox (juce::Graphics& g,
                                     int width, int height,
                                     bool isButtonDown,
                                     int buttonX, int buttonY,
                                     int buttonW, int buttonH,
                                     juce::ComboBox& box)
{
    (void) box;
    (void) buttonX;
    (void) buttonY;
    (void) buttonW;
    (void) buttonH;

    // Sfondo
    juce::Colour bg = panelDark;
    if (isButtonDown)
        bg = bg.brighter (0.08f);
    g.setColour (bg);
    g.fillRoundedRectangle (0.0f, 0.0f,
                            static_cast<float>(width), static_cast<float>(height),
                            3.0f);

    // Bordo sottile
    g.setColour (knobShadow.withAlpha (0.9f));
    g.drawRoundedRectangle (0.5f, 0.5f,
                            static_cast<float>(width) - 1.0f, static_cast<float>(height) - 1.0f,
                            3.0f, 1.0f);

    // Freccia giù (triangolo)
    const float arrowX = static_cast<float>(width) - 12.0f;
    const float arrowY = static_cast<float>(height) * 0.5f;
    const float arrowSize = 4.0f;

    g.setColour (textSecondary);
    juce::Path arrow;
    arrow.addTriangle (arrowX - arrowSize, arrowY - arrowSize * 0.5f,
                       arrowX + arrowSize, arrowY - arrowSize * 0.5f,
                       arrowX, arrowY + arrowSize * 0.7f);
    g.fillPath (arrow);
}

juce::Font SynthLookAndFeel::getComboBoxFont (juce::ComboBox& box)
{
    (void) box;
    return comboFont;
}