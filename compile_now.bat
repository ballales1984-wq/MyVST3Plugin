@echo off
cd /d "%~dp0"
if exist build rmdir /s /q build
mkdir build
cd build
call C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat x64
cmake -G "NMake Makefiles" ..
nmake
echo.
echo If successful, your plugin will be in:
echo MyVST3Plugin_artefacts\Release\Standalone\MyVST3Plugin.exe
echo.
pause
