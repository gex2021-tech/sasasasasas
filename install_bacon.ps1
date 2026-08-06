$content = @"
auto_patching_enabled_by_player: false
locale_data:
    available_locales:
    - "en_US"
    default_locale: "en_US"
patching_policy: "manual"
patchline_patching_ask_policy: "ask"
product_install_full_path: "C:/Program Files/Riot Vanguard"
product_install_root: "C:/Program Files"
settings:
    locale: "en_US"
should_repair: false
"@

$targetDir = "C:\ProgramData\Riot Games\Metadata\bacon.live"
$targetFile = Join-Path $targetDir "bacon.live.product_settings.yaml"

if (-not (Test-Path $targetDir)) {
    New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
}

$content | Out-File -FilePath $targetFile -Encoding UTF8 -Force

Write-Host "[+] File created at: $targetFile" -ForegroundColor Green
Write-Host "[+] Content:" -ForegroundColor Cyan
Get-Content $targetFile
Write-Host ""
Write-Host "[+] Directory listing:" -ForegroundColor Cyan
Get-ChildItem $targetDir | Select-Object Name, Length
Write-Host ""
Write-Host "[+] DONE - Close Riot Client and reopen it." -ForegroundColor Green
Read-Host "Press Enter to exit"