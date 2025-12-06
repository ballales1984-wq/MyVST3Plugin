# 🔄 MyVST3Plugin - Development Workflow & Paths Guide

Questo documento descrive il flusso di lavoro corretto per lo sviluppo del plugin MyVST3Plugin, inclusi percorsi, comandi e procedure standardizzate.

## 📁 Struttura del Progetto

```
MyVST3Plugin/
├── Source/                          # Codice sorgente C++
│   ├── PluginProcessor.h/cpp       # Logica audio principale
│   └── PluginEditor.h/cpp          # Interfaccia grafica
├── CMakeLists.txt                  # Configurazione build CMake
├── build/                          # Directory build (generata)
│   └── MyVST3Plugin_artefacts/     # Artefatti compilati
│       ├── Debug/                  # Build di debug
│       │   ├── Standalone/         # Eseguibile standalone
│       │   └── VST3/               # Plugin VST3
│       └── Release/                # Build di release
├── build_release/                  # Build di release separato
├── *.bat                           # Script di build automation
├── *.ps1                           # Script PowerShell per test/installazione
└── README.md                       # Documentazione utente
```

## 🚀 Flusso di Sviluppo Standard

### 1. **Setup Ambiente di Sviluppo**
```bash
# Directory principale del progetto
cd C:\Users\user\Downloads\JUCE-master\JUCE-master\extras\Projucer\Builds\VisualStudio2022\MyVST3Plugin
```

### 2. **Modifiche al Codice**
- Modifica solo i file in `Source/`
- **NON modificare** file in `build/` o `build_release/`
- Testa sempre le modifiche in Debug prima del Release

### 3. **Build Process**

#### **Build Rapido (Debug)**
```bash
# Dalla directory del progetto
.\quick_build.bat
```
- Compila in modalità Debug
- Output: `build/MyVST3Plugin_artefacts/Debug/`

#### **Build Release**
```bash
# Per build release completo
.\final_build.bat
```
- Compila in modalità Release ottimizzata
- Output: `build_release/MyVST3Plugin_artefacts/Release/`

### 4. **Testing**

#### **Test Standalone**
```bash
# Avvia applicazione standalone per testing
Start-Process "build\MyVST3Plugin_artefacts\Debug\Standalone\MyVST3Plugin.exe"
```

#### **Test in Reaper**
```bash
# 1. Installa localmente
.\install_vst3_local.ps1

# 2. Test Reaper setup
.\test_reaper_setup.ps1

# 3. Test funzionalità specifiche
.\test_detune.ps1
.\test_reaper_detune.ps1
```

### 5. **Version Control (Git)**

#### **Workflow Git Standard**
```bash
# 1. Controlla status
git status

# 2. Aggiungi modifiche
git add Source/PluginEditor.cpp Source/PluginProcessor.cpp

# 3. Commit con messaggio descrittivo
git commit -m "feat/fix: Descrizione delle modifiche

- Cosa è stato aggiunto/modificato
- Perché è necessario
- Eventuali breaking changes"

# 4. Push su GitHub
git push origin main
```

#### **Messaggi Commit Convenzionali**
- `feat:` - Nuove funzionalità
- `fix:` - Correzione bug
- `docs:` - Documentazione
- `style:` - Formattazione codice
- `refactor:` - Refactoring senza modifiche funzionali

## 📂 Percorsi di Lavoro Importanti

### **Directory Base del Progetto**
```
C:\Users\user\Downloads\JUCE-master\JUCE-master\extras\Projucer\Builds\VisualStudio2022\MyVST3Plugin
```

### **Directory Build**
- **Debug Build:** `build/MyVST3Plugin_artefacts/Debug/`
- **Release Build:** `build_release/MyVST3Plugin_artefacts/Release/`

### **File Eseguibili**
- **Standalone Debug:** `build/MyVST3Plugin_artefacts/Debug/Standalone/MyVST3Plugin.exe`
- **Standalone Release:** `build_release/MyVST3Plugin_artefacts/Release/Standalone/MyVST3Plugin.exe`

### **Plugin VST3**
- **Debug VST3:** `build/MyVST3Plugin_artefacts/Debug/VST3/MyVST3Plugin.vst3`
- **Release VST3:** `build_release/MyVST3Plugin_artefacts/Release/VST3/MyVST3Plugin.vst3`

### **Directory di Installazione VST3**

#### **Installazione Globale (Admin)**
```
C:\Program Files\Common Files\VST3\
```
❌ **NON RACCOMANDATO** - Richiede privilegi amministratore

#### **Installazione Locale (Raccomandato)**
```
%USERPROFILE%\Documents\REAPER\UserPlugins\
```
✅ **RACCOMANDATO** - Non richiede admin, specifico per utente

### **Directory JUCE**
```
C:\Users\user\Downloads\JUCE-master\JUCE-master\
```

## ⚙️ Comandi di Build Disponibili

### **Script Batch (.bat)**
- `quick_build.bat` - Build veloce Debug
- `final_build.bat` - Build completo Release
- `build_simple.bat` - Build semplice senza ottimizzazioni
- `compile_now.bat` - Solo compilazione (no linking)
- `compile_final.bat` - Compilazione ottimizzata

### **Script PowerShell (.ps1)**
- `install_vst3_local.ps1` - Installa VST3 localmente
- `install_vst3.ps1` - Installa VST3 globalmente (admin)
- `test_reaper_setup.ps1` - Verifica setup Reaper
- `test_detune.ps1` - Test funzionalità detune
- `test_reaper_detune.ps1` - Test detune in Reaper
- `fix_reaper_plugin.ps1` - Fix problemi Reaper

## 🧪 Procedure di Testing

### **1. Test Funzionalità Base**
```bash
# Build Debug
.\quick_build.bat

# Avvia standalone
Start-Process "build\MyVST3Plugin_artefacts\Debug\Standalone\MyVST3Plugin.exe"

# Verifica:
# - Interfaccia si apre
# - Oscillatori producono suono
# - ADSR funziona
# - Filtro funziona
# - MIDI keyboard funziona
```

### **2. Test Detune Feature**
```bash
# Test standalone
.\test_detune.ps1

# Test in Reaper
.\test_reaper_detune.ps1
```

### **3. Test VST3 in DAW**
```bash
# Installa
.\install_vst3_local.ps1

# Apri Reaper
# - Scan VST plugins
# - Inserisci MyVST3Plugin
# - Test MIDI input
# - Test parametri in real-time
```

## 🔧 Troubleshooting

### **Plugin non si apre**
1. Chiudi eventuali processi attivi: `Stop-Process -Name "MyVST3Plugin" -Force`
2. Ricompila: `.\quick_build.bat`
3. Riavvia standalone

### **Reaper non trova il plugin**
1. Cancella cache Reaper: Options → Preferences → Plug-ins → VST → Clear cache
2. Rescan plugins
3. Verifica percorso installazione

### **Errori di Build**
1. Pulisci build: Rimuovi directory `build/` e `build_release/`
2. Ricompila: `.\quick_build.bat`
3. Verifica Visual Studio Build Tools installati

## 📋 Checklist Pre-Commit

Prima di ogni commit, verifica:

- [ ] **Build riuscito** in Debug mode
- [ ] **Test standalone** funziona
- [ ] **Nessun crash** o errori evidenti
- [ ] **Codice pulito** e ben commentato
- [ ] **File modificati** solo in `Source/`
- [ ] **Messaggio commit** descrittivo e formattato correttamente

## 🚀 Deployment Finale

### **Per Release Pubblico**
1. Build Release: `.\final_build.bat`
2. Test Release build completamente
3. Aggiorna versione in `CMakeLists.txt`
4. Aggiorna `README.md` con nuove funzionalità
5. Commit e tag versione: `git tag v1.x.x`
6. Push tag: `git push origin v1.x.x`
7. Crea release su GitHub con file VST3

---

**Ricorda:** Lavora sempre nella directory del progetto e usa i percorsi relativi per massima portabilità! 🔧✨
