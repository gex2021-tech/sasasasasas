@echo off
echo ============================================
echo   FIX VGC SERVICE - Run as Administrator
echo ============================================
echo.

echo [1] Querying current VGC state...
sc qc vgc
sc query vgc
echo.

echo [2] Stopping VGC service...
net stop vgc /y 2>nul
sc stop vgc 2>nul
timeout /t 2 /nobreak >nul

echo [3] Configuring VGC as AUTO start...
sc config vgc start= auto
echo.

echo [4] Trying to start VGC...
sc start vgc 2>nul
timeout /t 2 /nobreak >nul

echo [5] Final state:
sc qc vgc
sc query vgc
echo.

echo ============================================
echo   DONE - Close this and restart Valorant
echo ============================================
pause