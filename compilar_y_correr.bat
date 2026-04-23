@echo off
setlocal
cd /d "%~dp0"
echo [1/2] Compilando proyecto (Release)...
"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build_win --config Release

if %errorlevel% equ 0 (
    echo [2/2] Lanzando aplicacion...
    :: Se ejecuta desde la raiz para que las rutas "Resource Files/..." sean validas
    .\build_win\Release\app.exe
    if %errorlevel% neq 0 (
        echo [ERROR] La aplicacion termino con error: %errorlevel%
        pause
    )
) else (
    echo [ERROR] La compilacion fallo.
    pause
)
endlocal
