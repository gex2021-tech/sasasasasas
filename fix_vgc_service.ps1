# Script to restore legitimate VGC service configuration and unfreeze Riot Client
try {
    Stop-Process -Id 2852 -Force -ErrorAction SilentlyContinue
} catch {}

try {
    Get-Process -Name vgc, svchost, RiotClientServices, VALORANT -ErrorAction SilentlyContinue | Where-Object { $_.Path -like "*vgc*" -or $_.CommandLine -like "*netsvcs*" } | Stop-Process -Force -ErrorAction SilentlyContinue
} catch {}

# Reconfigure vgc service
& sc.exe config vgc binPath= "C:\Program Files\Riot Vanguard\vgc.exe" start= demand DisplayName= "Vanguard Service"
Write-Host "vgc service reconfigured to legitimate Vanguard binary"
