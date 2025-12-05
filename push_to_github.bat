@echo off
echo ========================================
echo PUSHING VST3 PLUGIN TO GITHUB
echo ========================================

cd /d "%~dp0"

echo Initializing Git repository...
git init

echo Adding .gitignore...
echo build/ > .gitignore
echo build_*/ >> .gitignore
echo *.lib >> .gitignore
echo *.exp >> .gitignore
echo *.pdb >> .gitignore
echo *.obj >> .gitignore
echo CMakeCache.txt >> .gitignore
echo CMakeFiles/ >> .gitignore
echo .vs/ >> .gitignore

echo Adding all files...
git add .

echo Creating initial commit...
git commit -m "Initial commit: VST3 Plugin with Virtual MIDI Keyboard

- Dual oscillator synthesizer (sine + square waves)
- ADSR envelope controls
- Virtual MIDI keyboard component
- Real-time parameter visualization
- Support for external MIDI devices
- JUCE framework integration
- CMake build system
- VST3 format"

echo Setting up remote repository...
echo Please enter your GitHub repository URL:
echo Example: https://github.com/yourusername/MyVST3Plugin.git
echo.
echo If you don't have a repository yet, create one at: https://github.com/new
echo.
set /p repo_url="GitHub Repository URL: "

if "%repo_url%"=="" (
    echo No URL provided. Skipping remote setup.
    echo You can manually add remote later with:
    echo git remote add origin YOUR_REPO_URL
    echo git push -u origin main
) else (
    git remote add origin %repo_url%
    git branch -M main
    git push -u origin main
    echo ========================================
    echo SUCCESS! Repository pushed to GitHub!
    echo ========================================
)

pause
