@echo off
setlocal EnableExtensions

cd /d "%~dp0"

echo [1/4] Locating Visual Studio environment...
set "VCVARS="
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
if not defined VCVARS if exist "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" set "VCVARS=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat"
if not defined VCVARS if exist "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvarsall.bat" set "VCVARS=C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvarsall.bat"
if not defined VCVARS (
  echo ERROR: vcvarsall.bat not found. Install Visual Studio Build Tools/Community with C++ workload.
  exit /b 1
)

call "%VCVARS%" x64 >nul
if errorlevel 1 (
  echo ERROR: Failed to initialize MSVC environment.
  exit /b 1
)

echo [2/4] Configuring (CMake)...
if not exist build mkdir build
cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
  echo ERROR: CMake configure failed.
  exit /b 1
)

echo [3/4] Building...
cmake --build build --config Release --parallel
if errorlevel 1 (
  echo ERROR: Build failed.
  exit /b 1
)

echo [4/4] Done.
echo Built artifacts are under: build\MyVST3Plugin_artefacts\
endlocal
