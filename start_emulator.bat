@echo off
echo.
echo ========================================================
echo   VGC Emulator Launcher
echo ========================================================
echo.
echo Iniciando tunnel server...
start "VGC Tunnel Server" cmd /k "python server/tunnel_server.py"
timeout /t 3 /nobreak >nul
echo.
echo Iniciando vClient como administrador...
cd /d "C:\Riot Games\VALORANT\live"
start "VGC Client" /HIGH vClient.exe
echo.
echo Esperando conexion...
timeout /t 5 /nobreak
echo.
echo ¡VGC Emulator listo Ahora abre Riot Client y deberías ver PLAY.
echo.
echo Para detener: cierra ambas ventanas manualmente.
pause
