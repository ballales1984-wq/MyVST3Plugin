# Script per installare VST3 in directory locale (non richiede admin)

Write-Host "Installing MyVST3Plugin to local VST3 directory..." -ForegroundColor Green

# Directory locali alternative dove Reaper potrebbe cercare
$localVst3Dirs = @(
    "$env:USERPROFILE\Documents\REAPER\UserPlugins",
    "$env:APPDATA\REAPER\UserPlugins",
    "$env:USERPROFILE\Desktop\VST3",
    "$env:USERPROFILE\Documents\VST3"
)

$source = "build\MyVST3Plugin_artefacts\Debug\VST3\MyVST3Plugin.vst3"
$installed = $false

foreach ($destDir in $localVst3Dirs) {
    if (Test-Path $source) {
        # Crea la directory se non esiste
        if (!(Test-Path $destDir)) {
            New-Item -ItemType Directory -Path $destDir -Force | Out-Null
            Write-Host "Created directory: $destDir" -ForegroundColor Yellow
        }

        $destPath = Join-Path $destDir "MyVST3Plugin.vst3"

        try {
            Copy-Item -Path $source -Destination $destPath -Recurse -Force
            Write-Host "SUCCESS: VST3 plugin installed!" -ForegroundColor Green
            Write-Host "Location: $destPath" -ForegroundColor Yellow
            Write-Host "Try loading in Reaper from this location" -ForegroundColor Cyan
            $installed = $true
            break
        }
        catch {
            Write-Host "Failed to install in: $destDir" -ForegroundColor Red
            continue
        }
    } else {
        Write-Host "ERROR: Source VST3 bundle not found!" -ForegroundColor Red
        Write-Host "Expected at: $source" -ForegroundColor Yellow
        break
    }
}

if (-not $installed) {
    Write-Host "Could not install to any local directory. Try:" -ForegroundColor Red
    Write-Host "1. Run install_vst3.ps1 as Administrator" -ForegroundColor Yellow
    Write-Host "2. Or manually copy the .vst3 file to Program Files\Common Files\VST3\" -ForegroundColor Yellow
}

Write-Host "Press any key to continue..."
$null = $host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
