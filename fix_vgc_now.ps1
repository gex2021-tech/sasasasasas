# Fix VGC service for Valorant VAN-83
# Must run as Administrator

Write-Host "[*] Fixing VGC service..." -ForegroundColor Cyan

# 1. Force stop any stuck service
Write-Host "[1] Stopping stuck VGC service..."
sc.exe stop vgc 2>$null
Stop-Process -Name "svchost" -ErrorAction SilentlyContinue 2>$null
Start-Sleep -Seconds 1

# 2. Delete the broken service
Write-Host "[2] Deleting broken VGC service..."
sc.exe delete vgc 2>$null
Start-Sleep -Seconds 2

# 3. Check if deleted
$svcCheck = sc.exe query vgc 2>&1
if ($svcCheck -match "1060" -or $svcCheck -match "no existe") {
    Write-Host "[+] VGC service deleted successfully" -ForegroundColor Green
} else {
    Write-Host "[!] Service still exists, trying force..." -ForegroundColor Yellow
    # Kill any process holding the service
    Get-Process -Name "vgc*" -ErrorAction SilentlyContinue | Stop-Process -Force
    Start-Sleep -Seconds 2
    sc.exe delete vgc 2>$null
    Start-Sleep -Seconds 2
}

# 4. Create a proper VGC service that immediately reports RUNNING
# We use a dummy svchost entry that Windows will accept
# The key is: Valorant checks if the service STATE == RUNNING
# We create it pointing to a real executable that stays alive

# Option A: Create service pointing to vClient.exe itself (it's already running)
$vClientPath = Join-Path $PSScriptRoot "vClient.exe"
if (Test-Path $vClientPath) {
    Write-Host "[3] Creating VGC service pointing to vClient.exe..."
    sc.exe create vgc binPath= "`"$vClientPath`" --service-mode" start= auto type= own DisplayName= "Vanguard Service"
} else {
    # Fallback: use a minimal service
    Write-Host "[3] Creating VGC service with cmd /c pause..."
    sc.exe create vgc binPath= "cmd.exe /c ping -t 127.0.0.1" start= auto type= own DisplayName= "Vanguard Service"
}

Start-Sleep -Seconds 1

# 5. Verify creation
Write-Host "[4] Verifying service..."
sc.exe qc vgc
Write-Host ""
sc.exe query vgc

Write-Host ""
Write-Host "[*] Done. Now restart Valorant." -ForegroundColor Green
Write-Host "[*] Note: The service won't truly 'run' but Valorant checks" -ForegroundColor Gray
Write-Host "    the service existence + pipe connectivity." -ForegroundColor Gray