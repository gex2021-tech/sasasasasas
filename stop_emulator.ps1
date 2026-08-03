# Stop VGC Emulator server

Write-Host ""
Write-Host "[*] Stopping VGC Emulator server..." -ForegroundColor Cyan
Write-Host ""

# Check for job info file
if (Test-Path ".server_job_info.json") {
    $jobInfo = Get-Content ".server_job_info.json" | ConvertFrom-Json
    
    try {
        $job = Get-Job -Id $jobInfo.JobId -ErrorAction SilentlyContinue
        if ($job) {
            Stop-Job -Id $jobInfo.JobId
            Remove-Job -Id $jobInfo.JobId
            Write-Host "[+] Stopped server job (ID: $($jobInfo.JobId))" -ForegroundColor Green
        } else {
            Write-Host "[!] Job not found (may have already stopped)" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "[!] Failed to stop job: $_" -ForegroundColor Red
    }
    
    Remove-Item ".server_job_info.json" -ErrorAction SilentlyContinue
}

# Also kill any python processes running server.main
$serverProcesses = Get-Process -Name python -ErrorAction SilentlyContinue |
    Where-Object { $_.CommandLine -like "*server.main*" }

foreach ($proc in $serverProcesses) {
    Write-Host "[*] Stopping server process (PID: $($proc.Id))..." -ForegroundColor Gray
    Stop-Process -Id $proc.Id -Force
    Write-Host "    [STOPPED]" -ForegroundColor Red
}

if (-not $serverProcesses -and -not (Test-Path ".server_job_info.json")) {
    Write-Host "[*] No server processes found" -ForegroundColor Gray
}

Write-Host ""
Write-Host "[+] Server stopped" -ForegroundColor Green
Write-Host ""
