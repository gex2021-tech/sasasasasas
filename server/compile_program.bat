@echo off
REM ================================================================
REM Script de compilación para program.exe (PC Servidor)
REM ================================================================
REM Uso: compile_program.bat
REM Requiere: Visual Studio con MSVC (cl.exe en PATH)
REM ================================================================

echo [BUILD] Compilando program.exe para PC Servidor...
echo.

REM Verificar que cl.exe esté disponible
where cl >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [ERROR] cl.exe no encontrado. Abra este script desde "Developer Command Prompt for VS"
    echo o asegúrese de que MSVC esté instalado y en PATH.
    pause
    exit /b 1
)

REM Compilar
echo [COMPILER] Usando: 
cl | findstr /C:"Microsoft"
echo.

echo [FLAGS] /EHsc /std:c++17 /O2 /W3
echo [LIBS] ws2_32.lib bcrypt.lib
echo.

cl program.cpp /EHsc /std:c++17 /O2 /W3 ^
   /link ws2_32.lib bcrypt.lib ^
   /OUT:program.exe

if %ERRORLEVEL% equ 0 (
    echo.
    echo [SUCCESS] program.exe compilado correctamente
    echo [SIZE] 
    dir program.exe | findstr "program.exe"
    echo.
    echo [NEXT STEPS]
    echo 1. Mueva program.exe a la carpeta server/ del PC Servidor
    echo 2. Edite config.yaml: wine.enabled: true
    echo 3. Ejecute: python -m server.main
    echo.
) else (
    echo.
    echo [ERROR] La compilación falló. Revise los errores arriba.
    echo.
)

pause
