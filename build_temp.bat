@echo off
cd /d "C:\Users\user\Downloads\JUCE-master\JUCE-master\extras\Projucer\Builds\VisualStudio2022\MyVST3Plugin\build"
call "C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to set up Visual Studio environment
    exit /b 1
)
del CMakeCache.txt 2>nul
rmdir /s /q CMakeFiles 2>nul
cmake -G "NMake Makefiles" ..
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed
    exit /b 1
)
nmake
if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed
    exit /b 1
)
echo SUCCESS: Plugin compiled successfully!
