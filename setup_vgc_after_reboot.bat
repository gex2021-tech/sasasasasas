@echo off
echo ============================================
echo   SETUP VGC SERVICE (post-reboot)
echo   Run as Administrator
echo ============================================
echo.

echo [1] Checking if old VGC is gone...
sc query vgc 2>&1 | findstr /I "1060" >nul
if %errorlevel%==0 (
    echo    [+] VGC service is gone. Creating fresh one...
) else (
    echo    [!] VGC still exists, deleting first...
    sc stop vgc 2>nul
    sc delete vgc 2>nul
    timeout /t 3 /nobreak >nul
)

echo [2] Creating VGC service (auto start)...
sc create vgc binPath= "C:\Windows\System32\svchost.exe -k netsvcs" start= auto type= own DisplayName= "Vanguard Service"
echo    Result: %errorlevel%

echo [3] Setting description...
sc description vgc "Riot Vanguard anti-cheat service"

echo [4] Verifying...
sc qc vgc
echo.
sc query vgc
echo.

echo ============================================
echo   If TIPO_INICIO = 2 AUTO_START and
echo   ESTADO = 1 STOPPED, that is CORRECT.
echo   
echo   Now run vClient.exe FIRST, then open
echo   Valorant from Riot Client.
echo ============================================
pause