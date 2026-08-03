@echo off
title VGC Emulator Loader
cd /d "%~dp0"

echo Starting VGC Emulator Loader...
python emulator_loader.py

if errorlevel 1 (
    echo.
    echo Error: Failed to start loader
    echo Make sure Python is installed and in PATH
    pause
)
