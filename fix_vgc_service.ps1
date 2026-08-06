# Script to register legitimate VGC and VGK service configuration for Vanguard bypass
# Run this as Administrator once if services are missing or corrupted.

$vgcPath = "C:\Program Files\Riot Vanguard\vgc.exe"
$vgkPath = "C:\Program Files\Riot Vanguard\vgk.sys"

Write-Host "[*] Configuring Vanguard Services (vgc & vgk)..." -ForegroundColor Cyan

# 1. Configure vgc service
$vgcService = Get-Service -Name vgc -ErrorAction SilentlyContinue
if (-not $vgcService) {
    Write-Host "[*] Creating vgc service..." -ForegroundColor Yellow
    & sc.exe create vgc binPath= "`"$vgcPath`"" start= demand DisplayName= "vgc"
} else {
    Write-Host "[*] Reconfiguring existing vgc service..." -ForegroundColor Yellow
    & sc.exe stop vgc >$null 2>&1
    & sc.exe config vgc binPath= "`"$vgcPath`"" start= demand DisplayName= "vgc"
}

# 2. Configure vgk driver service
$vgkService = Get-Service -Name vgk -ErrorAction SilentlyContinue
if (-not $vgkService) {
    Write-Host "[*] Creating vgk kernel driver service..." -ForegroundColor Yellow
    & sc.exe create vgk binPath= "`"$vgkPath`"" type= kernel start= demand DisplayName= "vgk"
} else {
    Write-Host "[*] Reconfiguring existing vgk service..." -ForegroundColor Yellow
    & sc.exe config vgk binPath= "`"$vgkPath`"" type= kernel start= demand DisplayName= "vgk"
}

# 3. Ensure Riot Vanguard registry entry
$uninstallKey = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Riot Vanguard"
if (-not (Test-Path $uninstallKey)) {
    New-Item -Path $uninstallKey -Force | Out-Null
    Set-ItemProperty -Path $uninstallKey -Name "DisplayName" -Value "Riot Vanguard"
    Set-ItemProperty -Path $uninstallKey -Name "DisplayIcon" -Value "C:\Program Files\Riot Vanguard\vgc.ico"
    Set-ItemProperty -Path $uninstallKey -Name "InstallLocation" -Value "C:\Program Files\Riot Vanguard"
    Set-ItemProperty -Path $uninstallKey -Name "UninstallString" -Value "`"C:\Program Files\Riot Vanguard\uninstall.exe`""
    Set-ItemProperty -Path $uninstallKey -Name "Publisher" -Value "Riot Games, Inc."
    Set-ItemProperty -Path $uninstallKey -Name "NoModify" -Value 1 -Type DWord
    Set-ItemProperty -Path $uninstallKey -Name "NoRepair" -Value 1 -Type DWord
}

Write-Host "[+] Vanguard services successfully configured!" -ForegroundColor Green
& sc.exe qc vgc
& sc.exe qc vgk
