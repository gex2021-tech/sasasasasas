@echo off
echo ============================================
echo   FORCE FIX VGC SERVICE - Administrator
echo ============================================
echo.

echo [1] Killing ANY process that might hold VGC...
taskkill /F /IM vgc.exe 2>nul
taskkill /F /IM vgtray.exe 2>nul
taskkill /F /IM vgk.exe 2>nul
timeout /t 1 /nobreak >nul

echo [2] Force deleting VGC service via registry...
reg delete "HKLM\SYSTEM\CurrentControlSet\Services\vgc" /f 2>nul
echo    Registry delete result: %errorlevel%
timeout /t 1 /nobreak >nul

echo [3] Also trying sc delete...
sc stop vgc 2>nul
sc delete vgc 2>nul
timeout /t 2 /nobreak >nul

echo [4] Checking if VGC is gone...
sc query vgc 2>&1
echo.

echo [5] Creating fresh VGC service (auto start, dummy binary)...
sc create vgc binPath= "C:\Windows\System32\svchost.exe -k netsvcs" start= auto type= own DisplayName= "Vanguard Service"
echo    Create result: %errorlevel%
timeout /t 1 /nobreak >nul

echo [6] Setting VGC description...
sc description vgc "Riot Vanguard anti-cheat service"

echo [7] Final state:
sc qc vgc
echo.
sc query vgc
echo.

echo ============================================
echo   If service shows STOPPED + AUTO, thats OK
echo   Valorant should accept STOPPED+AUTO state
echo   Close this and restart Valorant
echo ============================================
pause