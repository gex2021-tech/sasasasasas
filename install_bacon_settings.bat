@echo off
echo ============================================
echo   INSTALL VANGUARD PRODUCT SETTINGS
echo   Run as Administrator
echo ============================================
echo.

echo [1] Creating bacon.live.product_settings.yaml...
mkdir "C:\ProgramData\Riot Games\Metadata\bacon.live" 2>nul

(
echo auto_patching_enabled_by_player: false
echo locale_data:
echo     available_locales:
echo     - "en_US"
echo     default_locale: "en_US"
echo patching_policy: "manual"
echo patchline_patching_ask_policy: "ask"
echo product_install_full_path: "C:/Program Files/Riot Vanguard"
echo product_install_root: "C:/Program Files"
echo settings:
echo     locale: "en_US"
echo should_repair: false
) > "C:\ProgramData\Riot Games\Metadata\bacon.live\bacon.live.product_settings.yaml"

echo [2] Verifying file...
type "C:\ProgramData\Riot Games\Metadata\bacon.live\bacon.live.product_settings.yaml"
echo.

echo [3] Listing bacon.live directory...
dir "C:\ProgramData\Riot Games\Metadata\bacon.live"
echo.

echo ============================================
echo   DONE - Now close Riot Client completely
echo   and reopen it. Should show PLAY not UPDATE
echo ============================================
pause