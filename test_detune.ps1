# Test script per la funzionalità Detune

Write-Host "=== MyVST3Plugin Detune Test ===" -ForegroundColor Cyan
Write-Host "Questa guida ti aiuta a testare la nuova funzionalità detune degli oscillatori" -ForegroundColor White
Write-Host ""

Write-Host "🎵 Come testare il detune:" -ForegroundColor Green
Write-Host "1. Apri l'applicazione standalone O carica il plugin in Reaper" -ForegroundColor White
Write-Host "2. Imposta Osc1 Frequency a 440 Hz (La centrale)" -ForegroundColor White
Write-Host "3. Imposta Osc2 Frequency a 220 Hz (La sotto)" -ForegroundColor White
Write-Host "4. Suona una nota (usa la tastiera virtuale o MIDI)" -ForegroundColor White
Write-Host ""

Write-Host "🎛️ Impostazioni detune consigliate:" -ForegroundColor Yellow
Write-Host ""
Write-Host "Chorus sottile (naturale):" -ForegroundColor Magenta
Write-Host "  - Detune: +5 a +15 cents" -ForegroundColor White
Write-Host "  - Risultato: Suono più 'corposo' e ricco" -ForegroundColor Gray
Write-Host ""
Write-Host "Chorus pronunciato:" -ForegroundColor Magenta
Write-Host "  - Detune: +20 a +30 cents" -ForegroundColor White
Write-Host "  - Risultato: Effetto chorus evidente" -ForegroundColor Gray
Write-Host ""
Write-Host "Effetto speciale:" -ForegroundColor Magenta
Write-Host "  - Detune: -10 a -50 cents" -ForegroundColor White
Write-Host "  - Risultato: Detune discendente (suono più 'scuro')" -ForegroundColor Gray
Write-Host ""

Write-Host "🧪 Test rapidi:" -ForegroundColor Green
Write-Host "- Prova valori positivi e negativi del detune" -ForegroundColor White
Write-Host "- Nota come cambia il suono da 'stretto' a 'largo'" -ForegroundColor White
Write-Host "- Confronta con detune = 0 (suono originale)" -ForegroundColor White
Write-Host ""

Write-Host "💡 Suggerimento:" -ForegroundColor Cyan
Write-Host "Per sentire meglio l'effetto, usa valori bassi di detune (5-10 cents)" -ForegroundColor White
Write-Host "e nota come il suono diventa più 'spesso' e professionale." -ForegroundColor White
Write-Host ""

Write-Host "🎼 Combinazioni interessanti:" -ForegroundColor Yellow
Write-Host "- Pad sound: Detune +8 cents, ADSR lento" -ForegroundColor White
Write-Host "- Bass: Detune +12 cents per più presenza" -ForegroundColor White
Write-Host "- Lead: Detune +5 cents per calore" -ForegroundColor White
Write-Host ""

Write-Host "Premi un tasto per continuare..." -ForegroundColor Gray
$null = $host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

# Avvia l'applicazione standalone automaticamente
Write-Host "Avvio applicazione standalone per test..." -ForegroundColor Green
Write-Host "Chiudi questa finestra e usa l'app per testare!" -ForegroundColor Yellow

Start-Process ".\build\MyVST3Plugin_artefacts\Debug\Standalone\MyVST3Plugin.exe" -NoNewWindow
