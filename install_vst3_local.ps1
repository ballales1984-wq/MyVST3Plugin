# Script per installare VST3 in directory locale (non richiede admin)

Write-Host "Installing MyVST3Plugin to local VST3 directory..." -ForegroundColor Green

# Directory locali per Reaper
$localVst3Dir = "$env:USERPROFILE\Documents\REAPER\UserPlugins"

$source = "build\MyVST3Plugin_artefacts\Debug\VST3\MyVST3Plugin.vst3"

if (Test-Path $source) {
    if (!(Test-Path $localVst3Dir)) {
        New-Item -ItemType Directory -Path $localVst3Dir -Force | Out-Null
    }

    Copy-Item -Path $source -Destination "$localVst3Dir\MyVST3Plugin.vst3" -Recurse -Force
    Write-Host "SUCCESS: VST3 plugin installed!" -ForegroundColor Green
    Write-Host "Location: $localVst3Dir\MyVST3Plugin.vst3" -ForegroundColor Yellow
} else {
    Write-Host "ERROR: VST3 bundle not found at $source" -ForegroundColor Red
    Write-Host "Make sure to build the plugin first with .\quick_build.bat" -ForegroundColor Yellow
}

Write-Host "Press any key to continue..."
$null = $host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
