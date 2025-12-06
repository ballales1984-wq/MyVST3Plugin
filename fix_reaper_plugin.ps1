# Script per risolvere problemi con il plugin VST3 in Reaper

Write-Host "🔧 RIPARAZIONE PLUGIN VST3 PER REAPER" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor White
Write-Host ""

# Verifica che il plugin sia compilato
$pluginSource = "build\MyVST3Plugin_artefacts\Debug\VST3\MyVST3Plugin.vst3"
$reaperPluginDir = "C:\Users\user\Documents\REAPER\UserPlugins"
$desktopTestDir = "C:\Users\user\Desktop\VST3_TEST"

Write-Host "📁 Verifica plugin compilato..." -ForegroundColor Yellow
if (Test-Path $pluginSource) {
    Write-Host "✅ Plugin compilato trovato" -ForegroundColor Green
} else {
    Write-Host "❌ Plugin NON compilato! Ricompila prima." -ForegroundColor Red
    exit
}

Write-Host ""

# Rimuovi vecchie installazioni
Write-Host "🗑️ Rimozione vecchie installazioni..." -ForegroundColor Yellow
Remove-Item "$reaperPluginDir\MyVST3Plugin.vst3" -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item "$desktopTestDir\MyVST3Plugin.vst3" -Recurse -Force -ErrorAction SilentlyContinue

# Installa in directory Reaper
Write-Host "📥 Installazione in directory Reaper..." -ForegroundColor Green
Copy-Item $pluginSource "$reaperPluginDir\" -Recurse -Force
Write-Host "✅ Installato in: $reaperPluginDir" -ForegroundColor Green

# Installa anche sul desktop per test
Write-Host "📥 Installazione di test sul desktop..." -ForegroundColor Green
Copy-Item $pluginSource "$desktopTestDir\" -Recurse -Force
Write-Host "✅ Installato in: $desktopTestDir" -ForegroundColor Green

Write-Host ""
Write-Host "🔄 ISTRUZIONI PER REAPER:" -ForegroundColor Cyan
Write-Host "1. Apri Reaper" -ForegroundColor White
Write-Host "2. Vai su: Options → Preferences → Plug-ins → VST" -ForegroundColor White
Write-Host "3. Clicca 'Clear cache and rescan'" -ForegroundColor Yellow
Write-Host "4. Aspetta che finisca (può richiedere tempo)" -ForegroundColor White
Write-Host "5. Riavvia Reaper completamente" -ForegroundColor Yellow
Write-Host ""

Write-Host "🎯 SE NON FUNZIONA:" -ForegroundColor Red
Write-Host "• Prova a cercare manualmente nella directory VST3" -ForegroundColor White
Write-Host "• Usa 'Add from directory' e seleziona: $desktopTestDir" -ForegroundColor Yellow
Write-Host "• Verifica che Reaper stia scansionando la directory UserPlugins" -ForegroundColor White
Write-Host ""

Write-Host "🧪 TEST IMMEDIATO:" -ForegroundColor Green
Write-Host "Prova l'applicazione standalone:" -ForegroundColor White
Write-Host ".\build\MyVST3Plugin_artefacts\Debug\Standalone\MyVST3Plugin.exe" -ForegroundColor Yellow
Write-Host ""

Write-Host "Premi un tasto per continuare..." -ForegroundColor Gray
$null = $host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
