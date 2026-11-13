# Installs VST3 into a local directory (no admin)

$ErrorActionPreference = "Stop"
Write-Host "Installing MyVST3Plugin to local VST3 directory..." -ForegroundColor Green

$localVst3Dir = "$env:USERPROFILE\Documents\REAPER\UserPlugins"
$candidates = @(
    "build_vs\MyVST3Plugin_artefacts\Release\VST3\MyVST3Plugin.vst3",
    "build\MyVST3Plugin_artefacts\Release\VST3\MyVST3Plugin.vst3",
    "build_vs\MyVST3Plugin_artefacts\Debug\VST3\MyVST3Plugin.vst3",
    "build\MyVST3Plugin_artefacts\Debug\VST3\MyVST3Plugin.vst3",
    "build_vs\MyVST3Plugin_artefacts\VST3\MyVST3Plugin.vst3",
    "build\MyVST3Plugin_artefacts\VST3\MyVST3Plugin.vst3"
)

$source = $candidates | Where-Object { Test-Path $_ } | Select-Object -First 1

if ($source) {
    if (!(Test-Path $localVst3Dir)) {
        New-Item -ItemType Directory -Path $localVst3Dir -Force | Out-Null
    }

    Copy-Item -Path $source -Destination (Join-Path $localVst3Dir "MyVST3Plugin.vst3") -Recurse -Force
    Write-Host "SUCCESS: VST3 plugin installed!" -ForegroundColor Green
    Write-Host "Location: $(Join-Path $localVst3Dir 'MyVST3Plugin.vst3')" -ForegroundColor Yellow
} else {
    Write-Host "ERROR: VST3 bundle not found." -ForegroundColor Red
    Write-Host "Checked:" -ForegroundColor Yellow
    $candidates | ForEach-Object { Write-Host " - $_" -ForegroundColor Yellow }
    Write-Host "Build first with .\quick_build.bat" -ForegroundColor Yellow
    exit 1
}
