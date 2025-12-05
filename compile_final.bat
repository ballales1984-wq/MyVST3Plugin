@echo off
cd /d "%~dp0"
if exist build rmdir /s /q build >nul 2>&1
mkdir build
cd build

echo Setting up Visual Studio environment and running CMake...
call "C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1 && (
    echo Environment configured successfully
    echo Running CMake...
    cmake -G "NMake Makefiles" .. && (
        echo CMake completed successfully
        echo Building with nmake...
        nmake && (
            echo.
            echo ========================================
            echo SUCCESS! Plugin compiled successfully!
            echo ========================================
            echo Your plugin is ready at:
            echo MyVST3Plugin_artefacts\Release\Standalone\MyVST3Plugin.exe
            echo.
            echo Copy it to your VST3 folder to use in DAWs!
            echo ========================================
        ) || (
            echo ERROR: Build failed
            goto :error
        )
    ) || (
        echo ERROR: CMake failed
        goto :error
    )
) || (
    echo ERROR: Failed to set up Visual Studio environment
    goto :error
)

goto :end

:error
echo.
echo Compilation failed. Check the errors above.
pause
exit /b 1

:end
echo.
pause
