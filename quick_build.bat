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

echo [2/4] Configuring (CMake + Visual Studio 2022)...
if not exist build_vs mkdir build_vs
cmake -S . -B build_vs -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
  echo ERROR: CMake configure failed.
  exit /b 1
)

echo [3/4] Building (Release)...
cmake --build build_vs --config Release --parallel
if errorlevel 1 (
  echo ERROR: Build failed.
  exit /b 1
)

echo [4/4] Done.
echo Built artifacts are under: build_vs\MyVST3Plugin_artefacts\Release\
endlocal
