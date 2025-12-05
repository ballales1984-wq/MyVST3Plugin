@echo off
echo ========================================
echo BUILDING MY VST3 PLUGIN WITH KEYBOARD
echo ========================================

cd /d "%~dp0"

echo Setting up Visual Studio Build Tools environment...
call "C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to set up Visual Studio environment
    pause
    exit /b 1
)

echo Creating build directory if it doesn't exist...
if not exist build mkdir build

echo Going to build directory...
cd build

echo Cleaning old build files...
del CMakeCache.txt 2>nul
rmdir /s /q CMakeFiles 2>nul
rmdir /s /q juce_build 2>nul

echo Configuring with CMake...
cmake -G "NMake Makefiles" ..
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed
    pause
    exit /b 1
)

echo Building plugin...
nmake
if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo ========================================
echo SUCCESS! Plugin built successfully!
echo ========================================
echo The plugin should be in:
echo C:\Program Files\Common Files\VST3\MyVST3Plugin.vst3
echo ========================================
echo You can now load it in your DAW!
echo ========================================
pause
