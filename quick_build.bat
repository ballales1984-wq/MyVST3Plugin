@echo off
cd /d "%~dp0"

echo Setting up Visual Studio environment...
call "C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1

echo Creating build directory...
if not exist build mkdir build

echo Going to build directory...
cd build

echo Configuring with CMake...
cmake -G "NMake Makefiles" ..

echo Building...
nmake

echo Done!
