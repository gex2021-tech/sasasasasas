# Script to create/restore legitimate VGC service configuration
$vgcPath = "C:\Program Files\Riot Vanguard\vgc.exe"

# Check if vgc exists or create it
$service = Get-Service -Name vgc -ErrorAction SilentlyContinue

if (-not $service) {
    Write-Host "[*] Creating vgc service..."
    & sc.exe create vgc binPath= "`"$vgcPath`"" start= demand DisplayName= "Vanguard Service"
} else {
    Write-Host "[*] Reconfiguring existing vgc service..."
    & sc.exe config vgc binPath= "`"$vgcPath`"" start= demand DisplayName= "Vanguard Service"
}

# Verify service status
& sc.exe query vgc
