@echo off
setlocal enabledelayedexpansion
:: ============================================
:: VGC Emulator - Setup After Vanguard Install
:: ============================================
:: Este script configura el emulador DESPUÉS de instalar Vanguard oficialmente.
:: Ejecutar COMO ADMINISTRADOR.
echo.
echo ========================================================
echo   VGC Emulator - Setup Post-Instalacion de Vanguard
echo ========================================================
echo.
:: [1/6] Verificar permisos de administrador
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo [ERROR] Este script requiere privilegios de administrador.
    echo Click derecho en este archivo -^> "Ejecutar como administrador"
    pause
    exit /b 1
)
echo [1/6] Privilegios de administrador verificados. OK.
echo.
:: [2/6] Verificar que vgk.sys esté instalado
if not exist "C:\Windows\System32\drivers\vgk.sys" (
    echo [ERROR] vgk.sys no encontrado. Instala Vanguard oficialmente primero:
    echo https://valorant.playvalorant.com/download/
    echo Luego reinicia tu PC y vuelve a ejecutar este script.
    pause
    exit /b 1
)
echo [2/6] vgk.sys detectado en C:\Windows\System32\drivers\vgk.sys. OK.
echo.
:: [3/6] Iniciar servicio vgc si está detenido
sc query vgc | find "RUNNING" >nul
if %errorLevel% neq 0 (
    echo [INFO] Iniciando servicio vgc...
    net start vgc
    if %errorLevel% neq 0 (
        echo [WARNING] No se pudo iniciar vgc. El emulador lo hará automáticamente.
    ) else (
        echo [3/6] Servicio vgc iniciado correctamente. OK.
    )
) else (
    echo [3/6] Servicio vgc ya está corriendo. OK.
)
echo.
:: [4/6] Encontrar instalación de Valorant
set VALORANT_PATH=
for %%D in (C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    if exist "%%D:\Riot Games\VALORANT\live\Valorant.exe" (
        set VALORANT_PATH=%%D:\Riot Games\VALORANT\live
        goto :found
    )
)
:found
if "%VALORANT_PATH%"=="" (
    echo [ERROR] No se encontró la instalación de Valorant.
    echo Asegúrate de tener Valorant instalado.
    pause
    exit /b 1
)
echo [4/6] Valorant encontrado en: %VALORANT_PATH%. OK.
echo.
:: [5/6] Copiar vClient.exe a carpeta de Valorant
if not exist "vClient.exe" (
    echo [ERROR] vClient.exe no encontrado en el directorio actual.
    echo Asegúrate de ejecutar este script desde la carpeta del emulador.
    pause
    exit /b 1
)
copy /Y "vClient.exe" "%VALORANT_PATH%\vClient.exe" >nul
if %errorLevel% neq 0 (
    echo [ERROR] No se pudo copiar vClient.exe a %VALORANT_PATH%
    pause
    exit /b 1
)
echo [5/6] vClient.exe copiado a %VALORANT_PATH%. OK.
echo.
:: [6/6] Crear launcher automático start_emulator.bat
(
echo @echo off
echo echo.
echo echo ========================================================
echo echo   VGC Emulator Launcher
echo echo ========================================================
echo echo.
echo echo Iniciando tunnel server...
echo start "VGC Tunnel Server" cmd /k "python server/tunnel_server.py"
echo timeout /t 3 /nobreak ^>nul
echo echo.
echo echo Iniciando vClient como administrador...
echo cd /d "%VALORANT_PATH%"
echo start "VGC Client" /HIGH vClient.exe
echo echo.
echo echo Esperando conexion...
echo timeout /t 5 /nobreak
echo echo.
echo echo ¡VGC Emulator listo! Ahora abre Riot Client y deberías ver PLAY.
echo echo.
echo echo Para detener: cierra ambas ventanas manualmente.
echo pause
) > start_emulator.bat
echo [6/6] Launcher start_emulator.bat creado. OK.
echo.
echo ========================================================
echo   SETUP COMPLETADO EXITOSAMENTE
echo ========================================================
echo.
echo Pasos siguientes:
echo 1. Cierra Riot Client completamente (revisa system tray)
echo 2. Ejecuta start_emulator.bat COMO ADMINISTRADOR
echo 3. Espera el mensaje "VGC Emulator listo!"
echo 4. Abre Riot Client y deberías ver botón PLAY
echo.
echo Si aparece UPDATE en lugar de PLAY:
echo - Verifica que vClient.exe esté corriendo como admin
echo - Revisa los logs de tunnel_server.py
echo - Checkea logs en logs/sessions/
echo.
pause
