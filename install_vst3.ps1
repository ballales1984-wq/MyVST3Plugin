#Requires -RunAsAdministrator

Write-Host "Installing MyVST3Plugin to VST3 directory..." -ForegroundColor Green

# Remove old files
Remove-Item "C:\Program Files\Common Files\VST3\MyVST3Plugin.exe" -Force -ErrorAction SilentlyContinue
Remove-Item "C:\Program Files\Common Files\VST3\MyVST3Plugin.ilk" -Force -ErrorAction SilentlyContinue
Remove-Item "C:\Program Files\Common Files\VST3\MyVST3Plugin.pdb" -Force -ErrorAction SilentlyContinue

# Copy VST3 bundle
$source = "C:\Users\user\Downloads\JUCE-master\JUCE-master\extras\Projucer\Builds\VisualStudio2022\MyVST3Plugin\build\MyVST3Plugin_artefacts\Debug\VST3\MyVST3Plugin.vst3"
$destination = "C:\Program Files\Common Files\VST3\MyVST3Plugin.vst3"

if (Test-Path $source) {
    Copy-Item -Path $source -Destination $destination -Recurse -Force
    Write-Host "SUCCESS: VST3 plugin installed!" -ForegroundColor Green
    Write-Host "Location: $destination" -ForegroundColor Yellow
} else {
    Write-Host "ERROR: Source VST3 bundle not found!" -ForegroundColor Red
    Write-Host "Expected at: $source" -ForegroundColor Yellow
}

Write-Host "Press any key to continue..."
$null = $host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
