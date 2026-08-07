@echo off
REM fix_val5.bat - Complete VAL 5 diagnostic and remediation

echo.
echo ============================================================
echo VAL 5 DIAGNOSTIC AND AUTO-FIX TOOL
echo ============================================================
echo.

REM Step 1: Run diagnostics
echo [STEP 1] Running diagnostics...
python -m server.diagnostics.val5_debugger
if errorlevel 1 (
    echo [ERROR] Diagnostics failed
    pause
    exit /b 1
)

REM Step 2: Check if report was created
if not exist logs\val5_diagnostic.json (
    echo [ERROR] Diagnostic report not created
    pause
    exit /b 1
)

REM Step 3: Apply auto-remediation
echo.
echo [STEP 2] Applying auto-fixes...
python -m server.fixes.val5_auto_remediate logs\val5_diagnostic.json

echo.
echo ============================================================
echo FIX COMPLETE
echo ============================================================
echo.
echo Next steps:
echo   1. Restart backend: python -m server.main
echo   2. Restart emulator: python emulator_loader.py
echo   3. Run diagnostics again: .\fix_val5.bat
echo   4. Test in queue for 5+ minutes
echo.
pause
