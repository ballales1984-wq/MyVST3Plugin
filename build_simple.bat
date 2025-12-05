@echo off
echo Starting build process...

cd /d "C:\Users\user\Downloads\JUCE-master\JUCE-master\extras\Projucer\Builds\VisualStudio2022\MyVST3Plugin\build"

echo Setting up Visual Studio environment...
call "C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to set up Visual Studio environment
    goto end
)

echo Configuring with CMake...
cmake -G "NMake Makefiles" .. 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed
    goto end
)

echo Building plugin...
nmake 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed
    goto end
)

echo SUCCESS! Plugin built successfully!
echo Check: C:\Program Files\Common Files\VST3\MyVST3Plugin.vst3

:end
echo Press any key to continue...
pause >nul
