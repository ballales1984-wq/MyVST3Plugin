@echo off
setlocal EnableExtensions
echo === MyVST3Plugin Test ===
echo.
echo Compiling standalone test (main.cpp)...

where cl >nul 2>nul
if errorlevel 1 (
  if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
)
where cl >nul 2>nul
if errorlevel 1 (
  echo ERRORE: compilatore MSVC (cl.exe) non trovato.
  echo Apri "x64 Native Tools Command Prompt for VS" e riprova.
  exit /b 1
)

cl.exe /nologo /EHsc /std:c++17 main.cpp winmm.lib /Fe:MyVST3Plugin_test.exe
if errorlevel 1 (
  echo ERRORE: compilazione fallita.
  exit /b 1
)

echo.
echo Avvio test...
MyVST3Plugin_test.exe
endlocal
