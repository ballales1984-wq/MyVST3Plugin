@echo off
cd /d "%~dp0"

echo Setting up Visual Studio environment...
call "C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to set up Visual Studio environment
    pause
    exit /b 1
)

echo Going to build directory...
cd build

echo Building plugin...
nmake

echo ========================================
echo COMPILATION FINISHED!
echo ========================================
pause
