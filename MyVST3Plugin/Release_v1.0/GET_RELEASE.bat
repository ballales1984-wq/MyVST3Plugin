@echo off
echo ===========================================
echo   MyVST3Plugin v1.0 - Build Release
echo ===========================================
echo.
echo Questo script compila automaticamente
echo la versione finale del plugin.
echo.
echo Verranno creati i file:
echo - MyVST3Plugin.vst3 (per DAW)
echo - MyVST3Plugin.exe (standalone)
echo.

pause

echo.
echo Compilazione in corso...
echo.

call ..\quick_build.bat

if %errorlevel% neq 0 (
    echo.
    echo ❌ ERRORE durante la compilazione!
    echo Controlla i messaggi di errore sopra.
    echo.
    pause
    exit /b 1
)

echo.
echo ✅ Compilazione completata con successo!
echo.
echo File creati in:
echo build\MyVST3Plugin_artefacts\Debug\
echo.
echo Per installare:
echo 1. Esegui: install_vst3_local.ps1
echo 2. Riavvia Reaper
echo 3. Cerca "MyVST3Plugin" nei plugin
echo.
echo Per testare standalone:
echo Avvia: build\MyVST3Plugin_artefacts\Debug\Standalone\MyVST3Plugin.exe
echo.

pause
