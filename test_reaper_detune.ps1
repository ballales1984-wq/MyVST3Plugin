# Guida per testare Detune in Reaper

Write-Host "🎵 TEST DETUNE IN REAPER - Guida passo passo" -ForegroundColor Cyan
Write-Host "==================================================" -ForegroundColor White
Write-Host ""

Write-Host "📋 PASSO 1: Apri Reaper e crea un nuovo progetto" -ForegroundColor Green
Write-Host "1. Apri Reaper" -ForegroundColor White
Write-Host "2. File → New Project → Create blank project" -ForegroundColor White
Write-Host ""

Write-Host "📋 PASSO 2: Aggiungi il plugin VST3" -ForegroundColor Green
Write-Host "1. Nella track vuota, clicca sul pulsante FX (icona effetto)" -ForegroundColor White
Write-Host "2. Seleziona 'VST3' dal menu a sinistra" -ForegroundColor White
Write-Host "3. Cerca 'MyVST3Plugin' nella lista" -ForegroundColor White
Write-Host "4. Selezionalo e clicca 'OK'" -ForegroundColor White
Write-Host ""

Write-Host "📋 PASSO 3: Configura per test detune" -ForegroundColor Green
Write-Host "Nel plugin appena aperto:" -ForegroundColor White
Write-Host "• Osc1 Frequency: 440 Hz" -ForegroundColor Yellow
Write-Host "• Osc2 Frequency: 220 Hz" -ForegroundColor Yellow
Write-Host "• Osc2 Detune: 0 cents (inizialmente)" -ForegroundColor Yellow
Write-Host ""

Write-Host "📋 PASSO 4: Test del detune" -ForegroundColor Green
Write-Host "1. Premi play in Reaper (barra spaziatrice)" -ForegroundColor White
Write-Host "2. Nel plugin, attiva 'TEST MODE'" -ForegroundColor Magenta
Write-Host "3. Ascolta il suono base (detune = 0)" -ForegroundColor White
Write-Host "4. Ora imposta Osc2 Detune a +8 cents" -ForegroundColor Yellow
Write-Host "5. Nota come il suono diventa più 'corposo' e ricco!" -ForegroundColor Green
Write-Host ""

Write-Host "🎛️ VALORI DETUNE DA PROVARE:" -ForegroundColor Cyan
Write-Host "• +5 cents  → Chorus sottile, molto naturale" -ForegroundColor White
Write-Host "• +12 cents → Chorus caldo (perfetto per bass/pad)" -ForegroundColor White
Write-Host "• +25 cents → Chorus evidente" -ForegroundColor White
Write-Host "• -10 cents → Detune discendente" -ForegroundColor White
Write-Host ""

Write-Host "💡 CONSIGLI:" -ForegroundColor Yellow
Write-Host "• Usa cuffie per sentire meglio le differenze" -ForegroundColor White
Write-Host "• Prova con ADSR lento per suoni pad" -ForegroundColor White
Write-Host "• Confronta sempre detune=0 vs detune attivo" -ForegroundColor White
Write-Host ""

Write-Host "🎯 COSA SENTIRAI:" -ForegroundColor Green
Write-Host "• Senza detune: Suono 'stretto', preciso" -ForegroundColor White
Write-Host "• Con detune: Suono più 'largo', professionale, chorus naturale" -ForegroundColor White
Write-Host ""

Write-Host "Se il plugin non appare nella lista VST3:" -ForegroundColor Red
Write-Host "• Options → Preferences → Plug-ins → VST → Clear cache and rescan" -ForegroundColor Yellow
Write-Host ""

Write-Host "🎉 Buon test! Dimmi come suona il detune in Reaper!" -ForegroundColor Green
Write-Host ""
Write-Host "Premi un tasto per aprire Reaper..." -ForegroundColor Gray
$null = $host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

# Prova ad aprire Reaper automaticamente
$reaperPath = "C:\Program Files\REAPER (x64)\reaper.exe"
if (Test-Path $reaperPath) {
    Write-Host "Apertura Reaper..." -ForegroundColor Green
    Start-Process $reaperPath
} else {
    Write-Host "Reaper non trovato nel percorso standard" -ForegroundColor Yellow
    Write-Host "Aprilo manualmente" -ForegroundColor Yellow
}
