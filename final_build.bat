@echo off
cd /d "%~dp0"
if exist build rmdir /s /q build
mkdir build
cd build
call C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat x64
cmake -G NMake Makefiles ..
nmake
echo Build completed!
