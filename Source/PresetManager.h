#pragma once

#include <JuceHeader.h>

/** Preset salvato su disco: numero progressivo, nome e file XML. */
struct PresetEntry
{
    int number = 0;
    juce::String name;
    juce::File file;

    /** Etichetta mostrata nella combo: "001 - Bass". */
    juce::String getLabel() const
    {
        return juce::String (number).paddedLeft ('0', 3) + "  -  " + name;
    }
};

/** Gestore dei preset utente: cartella su disco, numerazione progressiva,
 *  salvataggio con nome, rinomina e cancellazione. */
class PresetManager
{
public:
    PresetManager();

    const juce::File& getDirectory() const { return dir; }

    /** Rilegge i preset presenti nella cartella. */
    void scan();

    int getNumPresets() const { return static_cast<int> (presets.size()); }
    const PresetEntry* getPreset (int index) const;
    int getIndexForNumber (int number) const;

    /** Numero da assegnare al prossimo salvataggio (max + 1). */
    int getNextNumber() const;

    /** Nome file canonico di un preset. */
    juce::File fileForNumber (int number, const juce::String& name) const;
    /** Rimuove i caratteri non validi per un nome file. */
    static juce::String makeSafeName (const juce::String& name);

    /** Salva (o sovrascrive) il preset numero "number". */
    bool savePreset (int number, const juce::String& name, const juce::XmlElement& state);
    /** Salva un nuovo preset e restituisce il numero assegnato (0 se errore). */
    int saveNewPreset (const juce::String& name, const juce::XmlElement& state);
    /** Rinomina il preset, mantenendo il numero. */
    bool renamePreset (int number, const juce::String& newName);
    /** Elimina il preset. */
    bool deletePreset (int number);
    /** Carica lo stato (elemento "STATE") del preset. */
    std::unique_ptr<juce::XmlElement> loadPreset (int number) const;

private:
    juce::File dir;
    std::vector<PresetEntry> presets;
};