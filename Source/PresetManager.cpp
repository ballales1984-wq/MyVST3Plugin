#include "PresetManager.h"

//==============================================================================
PresetManager::PresetManager()
{
    // I preset stanno nella cartella utente: %APPDATA%\MyVST3Plugin\Presets
    dir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
              .getChildFile ("MyVST3Plugin")
              .getChildFile ("Presets");

    dir.createDirectory();
    scan();
}

//==============================================================================
juce::String PresetManager::makeSafeName (const juce::String& name)
{
    auto cleaned = name.trim();

    // Caratteri non ammessi nei nomi file di Windows.
    for (auto c : cleaned)
        if (juce::String (c).containsAnyOf ("<>:\"/\\|?*"))
            cleaned = cleaned.replace (juce::String (c), "_");

    cleaned = cleaned.trim();

    if (cleaned.isEmpty())
        cleaned = "Preset";

    if (cleaned.length() > 40)
        cleaned = cleaned.substring (0, 40).trim();

    return cleaned;
}

juce::File PresetManager::fileForNumber (int number, const juce::String& name) const
{
    return dir.getChildFile (juce::String (number).paddedLeft ('0', 3)
                             + "_" + makeSafeName (name) + ".xml");
}

//==============================================================================
void PresetManager::scan()
{
    presets.clear();

    auto files = dir.findChildFiles (juce::File::findFiles, true, "*.xml");

    for (auto& file : files)
    {
        if (auto xml = juce::XmlDocument::parse (file))
        {
            PresetEntry entry;
            entry.file = file;
            entry.number = xml->getIntAttribute ("number", 0);
            entry.name = xml->getStringAttribute ("name", file.getFileNameWithoutExtension());

            // Se manca il numero lo si ricava dal nome del file (001_Nome).
            if (entry.number <= 0)
                entry.number = file.getFileNameWithoutExtension().getIntValue();

            if (entry.number <= 0)
                continue;

            if (entry.name.isEmpty())
                entry.name = juce::String (entry.number);

            presets.push_back (entry);
        }
    }

    std::sort (presets.begin(), presets.end(),
               [] (const PresetEntry& a, const PresetEntry& b) { return a.number < b.number; });
}

const PresetEntry* PresetManager::getPreset (int index) const
{
    if (juce::isPositiveAndBelow (index, static_cast<int> (presets.size())))
        return &presets[static_cast<size_t> (index)];

    return nullptr;
}

int PresetManager::getIndexForNumber (int number) const
{
    for (int i = 0; i < static_cast<int> (presets.size()); ++i)
        if (presets[static_cast<size_t> (i)].number == number)
            return i;

    return -1;
}

int PresetManager::getNextNumber() const
{
    int maxNumber = 0;

    for (const auto& p : presets)
        maxNumber = juce::jmax (maxNumber, p.number);

    return maxNumber + 1;
}

//==============================================================================
bool PresetManager::savePreset (int number, const juce::String& name, const juce::XmlElement& state)
{
    if (number <= 0)
        return false;

    const auto newFile = fileForNumber (number, name);

    // Rimuove eventuali vecchi file con lo stesso numero ma nome diverso.
    for (const auto& p : presets)
        if (p.number == number && p.file != newFile && p.file.exists())
            p.file.deleteFile();

    juce::XmlElement root ("PRESET");
    root.setAttribute ("number", number);
    root.setAttribute ("name", name);

    auto* stateCopy = new juce::XmlElement (state);
    auto* stateElement = new juce::XmlElement ("STATE");
    stateElement->addChildElement (stateCopy);
    root.addChildElement (stateElement);

    if (! root.writeTo (newFile))
        return false;

    scan();
    return true;
}

int PresetManager::saveNewPreset (const juce::String& name, const juce::XmlElement& state)
{
    const int number = getNextNumber();

    return savePreset (number, name, state) ? number : 0;
}

bool PresetManager::renamePreset (int number, const juce::String& newName)
{
    const int index = getIndexForNumber (number);

    if (index < 0)
        return false;

    auto entry = presets[static_cast<size_t> (index)];

    // Lo stato viene copiato dal file esistente.
    std::unique_ptr<juce::XmlElement> state;

    if (auto xml = juce::XmlDocument::parse (entry.file))
        if (auto* st = xml->getChildByName ("STATE"))
            state.reset (new juce::XmlElement (*st));

    if (state == nullptr)
        return false;

    if (! savePreset (number, newName, *state))
        return false;

    if (entry.file != fileForNumber (number, newName) && entry.file.exists())
        entry.file.deleteFile();

    scan();
    return true;
}

bool PresetManager::deletePreset (int number)
{
    const int index = getIndexForNumber (number);

    if (index < 0)
        return false;

    const bool ok = presets[static_cast<size_t> (index)].file.deleteFile();
    scan();
    return ok;
}

std::unique_ptr<juce::XmlElement> PresetManager::loadPreset (int number) const
{
    const int index = getIndexForNumber (number);

    if (index < 0)
        return {};

    if (auto xml = juce::XmlDocument::parse (presets[static_cast<size_t> (index)].file))
        if (auto* st = xml->getChildByName ("STATE"))
            return std::unique_ptr<juce::XmlElement> (new juce::XmlElement (*st));

    return {};
}