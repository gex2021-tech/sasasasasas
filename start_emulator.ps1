# Start VGC Emulator - Complete startup script

param(
    [switch]$ValidateFirst = $true,
    [switch]$OpenLogs = $false
)

$ErrorActionPreference = "Stop"

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  VGC Emulator Startup" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check Python
Write-Host "[*] Checking Python installation..." -ForegroundColor Gray
try {
    $pythonVersion = python --version 2>&1
    Write-Host "    $pythonVersion" -ForegroundColor Green
} catch {
    Write-Host "[!] Python not found. Please install Python 3.9+" -ForegroundColor Red
    exit 1
}

# Validate if requested
if ($ValidateFirst) {
    Write-Host "[*] Running validation checks..." -ForegroundColor Gray
    $validateResult = python validate_emulator.py
    $exitCode = $LASTEXITCODE
    
    if ($exitCode -ne 0) {
        Write-Host ""
        Write-Host "[!] Validation failed. Fix the issues above before starting." -ForegroundColor Red
        exit $exitCode
    }
    Write-Host ""
}

# Load config
Write-Host "[*] Loading configuration..." -ForegroundColor Gray
try {
    $config = Get-Content config.yaml -Raw | ConvertFrom-Yaml
    $host_addr = $config.tunnel.host
    $port = $config.tunnel.port
    $auth_key = $config.tunnel.auth_key
    
    Write-Host "    Host: $host_addr" -ForegroundColor Green
    Write-Host "    Port: $port" -ForegroundColor Green
    Write-Host "    Auth: $($auth_key.Substring(0, [Math]::Min(8, $auth_key.Length)))..." -ForegroundColor Green
} catch {
    Write-Host "[!] Failed to parse config.yaml: $_" -ForegroundColor Red
    exit 1
}

# Create logs directory
if (-not (Test-Path "logs")) {
    New-Item -ItemType Directory -Path "logs" | Out-Null
    Write-Host "[*] Created logs directory" -ForegroundColor Green
}

if (-not (Test-Path "logs/sessions")) {
    New-Item -ItemType Directory -Path "logs/sessions" | Out-Null
}

# Check if server is already running
Write-Host "[*] Checking if server is already running..." -ForegroundColor Gray
$existingProcess = Get-Process -Name python -ErrorAction SilentlyContinue | 
    Where-Object { $_.CommandLine -like "*server.main*" }

if ($existingProcess) {
    Write-Host "[!] Server appears to be already running (PID: $($existingProcess.Id))" -ForegroundColor Yellow
    $choice = Read-Host "    Kill and restart? (y/N)"
    if ($choice -eq 'y' -or $choice -eq 'Y') {
        Stop-Process -Id $existingProcess.Id -Force
        Start-Sleep -Seconds 2
        Write-Host "    [STOPPED]" -ForegroundColor Red
    } else {
        Write-Host "[*] Using existing server process" -ForegroundColor Green
        exit 0
    }
}

# Start server
Write-Host ""
Write-Host "[*] Starting VGC Emulator server..." -ForegroundColor Cyan
Write-Host ""

# Create a job to run the server
$serverJob = Start-Job -ScriptBlock {
    param($rootPath)
    Set-Location $rootPath
    python -m server.main 2>&1
} -ArgumentList (Get-Location).Path

# Wait a bit for startup
Start-Sleep -Seconds 2

# Check if server started successfully
$jobState = Get-Job -Id $serverJob.Id | Select-Object -ExpandProperty State

if ($jobState -eq "Failed") {
    $errorOutput = Receive-Job -Id $serverJob.Id
    Write-Host "[!] Server failed to start:" -ForegroundColor Red
    Write-Host $errorOutput -ForegroundColor Red
    Remove-Job -Id $serverJob.Id
    exit 1
}

Write-Host "[+] Server started (Job ID: $($serverJob.Id))" -ForegroundColor Green
Write-Host ""

# Test connection
Write-Host "[*] Testing server connection..." -ForegroundColor Gray
Start-Sleep -Seconds 1

try {
    $testResult = python test_client.py --host $host_addr --port $port 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "[+] Server is responding!" -ForegroundColor Green
    } else {
        Write-Host "[!] Server not responding yet..." -ForegroundColor Yellow
        Write-Host "    Give it a few more seconds to initialize" -ForegroundColor Gray
    }
} catch {
    Write-Host "[!] Connection test failed" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Server Status" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Server is running in background" -ForegroundColor Green
Write-Host "Job ID: $($serverJob.Id)" -ForegroundColor Gray
Write-Host ""
Write-Host "Listening on: $host_addr`:$port" -ForegroundColor Cyan
Write-Host "Auth Key: $auth_key" -ForegroundColor Cyan
Write-Host ""
Write-Host "Commands:" -ForegroundColor Yellow
Write-Host "  View output:  Receive-Job -Id $($serverJob.Id)" -ForegroundColor White
Write-Host "  Stop server:  Stop-Job -Id $($serverJob.Id); Remove-Job -Id $($serverJob.Id)" -ForegroundColor White
Write-Host "  Test client:  python test_client.py --host $host_addr --port $port" -ForegroundColor White
Write-Host ""

if ($OpenLogs) {
    Write-Host "[*] Opening logs directory..." -ForegroundColor Gray
    Invoke-Item "logs"
}

# Save job info for later
$jobInfo = @{
    JobId = $serverJob.Id
    Host = $host_addr
    Port = $port
    AuthKey = $auth_key
    StartedAt = (Get-Date).ToString("yyyy-MM-dd HH:mm:ss")
} | ConvertTo-Json

$jobInfo | Out-File -FilePath ".server_job_info.json" -Encoding UTF8

Write-Host "[*] Next steps:" -ForegroundColor Cyan
Write-Host "  1. Build vClient:  .\build_vclient.ps1 -VpsHost $host_addr" -ForegroundColor White
Write-Host "  2. Run vClient:    .\run_vclient.bat" -ForegroundColor White
Write-Host ""
Write-Host "Press Ctrl+C to stop monitoring. Server will continue in background." -ForegroundColor Gray
Write-Host ""

# Monitor server output
try {
    while ($true) {
        $output = Receive-Job -Id $serverJob.Id
        if ($output) {
            Write-Host $output
        }
        
        $jobState = Get-Job -Id $serverJob.Id | Select-Object -ExpandProperty State
        if ($jobState -eq "Failed" -or $jobState -eq "Stopped") {
            Write-Host ""
            Write-Host "[!] Server stopped unexpectedly" -ForegroundColor Red
            $errorOutput = Receive-Job -Id $serverJob.Id
            Write-Host $errorOutput -ForegroundColor Red
            Remove-Job -Id $serverJob.Id
            break
        }
        
        Start-Sleep -Seconds 1
    }
} catch {
    Write-Host ""
    Write-Host "[*] Stopped monitoring. Server continues in background." -ForegroundColor Yellow
    Write-Host "[*] Use 'Receive-Job -Id $($serverJob.Id)' to view output" -ForegroundColor Gray
}
