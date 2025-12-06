# Script per verificare il setup di MyVST3Plugin in Reaper

Write-Host "=== MyVST3Plugin Reaper Setup Test ===" -ForegroundColor Cyan

# Verifica che il plugin sia installato
$pluginPath = "$env:USERPROFILE\Documents\REAPER\UserPlugins\MyVST3Plugin.vst3"
$pluginExists = Test-Path $pluginPath

if ($pluginExists) {
    Write-Host "✅ Plugin trovato: $pluginPath" -ForegroundColor Green

    # Verifica la struttura del plugin
    $manifestPath = "$pluginPath\Contents\Resources\moduleinfo.json"
    if (Test-Path $manifestPath) {
        Write-Host "✅ Manifest VST3 presente" -ForegroundColor Green
    } else {
        Write-Host "❌ Manifest VST3 mancante" -ForegroundColor Red
    }

    $binaryPath = "$pluginPath\Contents\x86_64-win\MyVST3Plugin.vst3"
    if (Test-Path $binaryPath) {
        Write-Host "✅ Binary VST3 presente" -ForegroundColor Green
    } else {
        Write-Host "❌ Binary VST3 mancante" -ForegroundColor Red
    }

} else {
    Write-Host "❌ Plugin NON trovato in: $pluginPath" -ForegroundColor Red
    Write-Host "Esegui: .\install_vst3_local.ps1" -ForegroundColor Yellow
}

# Verifica Reaper
$reaperPath = "$env:ProgramFiles\REAPER (x64)\reaper.exe"
$reaperExists = Test-Path $reaperPath

if ($reaperExists) {
    Write-Host "✅ Reaper trovato: $reaperPath" -ForegroundColor Green
} else {
    Write-Host "⚠️  Reaper non trovato nel percorso standard" -ForegroundColor Yellow
    Write-Host "Verifica che Reaper sia installato" -ForegroundColor Yellow
}

Write-Host "`n=== Istruzioni per testare in Reaper ===" -ForegroundColor Cyan
Write-Host "1. Apri Reaper" -ForegroundColor White
Write-Host "2. Vai su: Insert → Virtual Instrument" -ForegroundColor White
Write-Host "3. Cerca 'MyVST3Plugin' nella lista" -ForegroundColor White
Write-Host "4. Selezionalo e clicca 'OK'" -ForegroundColor White
Write-Host "5. Premi alcuni tasti sulla tastiera virtuale per testare il suono" -ForegroundColor White

Write-Host "`n=== Troubleshooting ===" -ForegroundColor Yellow
Write-Host "- Se il plugin non appare: Options → Preferences → Plug-ins → VST → Clear cache and rescan" -ForegroundColor White
Write-Host "- Verifica che Reaper stia cercando nella directory UserPlugins" -ForegroundColor White
Write-Host "- Controlla la console di Reaper per eventuali errori" -ForegroundColor White

Write-Host "`nPremi un tasto per continuare..." -ForegroundColor Gray
$null = $host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
