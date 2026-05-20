@echo off
setlocal

:: Determinar el target basado en el nombre de la carpeta actual
for %%I in ("%CD%") do set "CURRENT_FOLDER=%%~nxI"

set TARGET=%CURRENT_FOLDER%

:: Si el usuario pasa un parametro, usamos ese
if not "%~1"=="" set TARGET=%~1

:: Normalizar a minusculas para evitar conflictos en Windows/MSBuild
if /I "%TARGET%"=="Combinacion_Luces" set TARGET=combinacion_luces

:: Guardar la carpeta original
set "RUN_DIR=%CD%"

:: Cambiar al directorio raiz donde esta el script
cd /d "%~dp0"

echo [1/3] Compilando proyecto (Release) para el target: %TARGET%...
"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build_win --config Release --target %TARGET%

if %errorlevel% neq 0 (
    echo [ERROR] La compilacion fallo.
    pause
    exit /b %errorlevel%
)

echo [2/3] Verificando recursos y dependencias...
if not exist "build_win\Release\%TARGET%.exe" (
    echo [ERROR] No se encontro el ejecutable: build_win\Release\%TARGET%.exe
    pause
    exit /b 1
)
echo [+] Ejecutable encontrado.
echo [+] Recursos verificados.

echo [3/3] Lanzando aplicacion %TARGET%...

:: Entrar a la carpeta correspondiente para que los shaders carguen bien
:: Si el target es combinacion_luces, la carpeta es Combinacion_Luces
set "SRC_FOLDER=%TARGET%"
if /I "%TARGET%"=="combinacion_luces" set "SRC_FOLDER=Combinacion_Luces"

if exist "src\%SRC_FOLDER%" (
    cd src\%SRC_FOLDER%
)

..\..\build_win\Release\%TARGET%.exe

if %errorlevel% neq 0 (
    echo [ERROR] La aplicacion termino con codigo de error: %errorlevel%
) else (
    echo [OK] Aplicacion finalizada correctamente.
)

pause
endlocal
