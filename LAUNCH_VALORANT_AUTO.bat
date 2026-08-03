@echo off
title VGC Emulator - Auto Launcher
cd /d "%~dp0"

:: Request Admin Privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo [*] Solicitando permisos de Administrador...
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b
)

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0launch_auto.ps1"
pause
