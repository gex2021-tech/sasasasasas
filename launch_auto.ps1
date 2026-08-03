# ==============================================================================
#  VGC EMULATOR - 1-CLICK AUTO LAUNCHER
# ==============================================================================
# Automatically handles:
#  1. Stale process termination
#  2. Server connectivity validation (192.168.1.136:51820)
#  3. Hosts file redirection & DNS Flush
#  4. vClient emulator startup & Vanguard Pipe Interception
#  5. Automatic Riot Client / Valorant Launch
# ==============================================================================

# Self-elevate to Administrator
if (-not ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Host "[*] Requesting Administrator Privileges..." -ForegroundColor Yellow
    Start-Process PowerShell -ArgumentList "-NoProfile -ExecutionPolicy Bypass -File `"$PSCommandPath`"" -Verb RunAs
    exit
}

$Host.UI.RawUI.WindowTitle = "VGC Emulator - Auto Launcher"
Clear-Host

$SERVER_IP = "192.168.1.136"
$SERVER_PORT = 51820
$ROOT_DIR = $PSScriptRoot
Set-Location $ROOT_DIR

function Draw-Header {
    Write-Host "=================================================================" -ForegroundColor Magenta
    Write-Host "             VGC EMULATOR 'ESPERANZA' - AUTO LAUNCHER             " -ForegroundColor Cyan
    Write-Host "=================================================================" -ForegroundColor Magenta
    Write-Host " Server Target: $SERVER_IP`:$SERVER_PORT" -ForegroundColor Gray
    Write-Host ""
}

Draw-Header

# ── STEP 1: KILL STALE PROCESSES ──────────────────────────────────────────────
Write-Host "[1/5] Killing stale processes..." -ForegroundColor Yellow
$killList = @("VALORANT", "RiotClientServices", "vgc", "vgc_client", "vClient", "server")
foreach ($proc in $killList) {
    Get-Process -Name $proc -ErrorAction SilentlyContinue | ForEach-Object {
        try {
            Stop-Process -Id $_.Id -Force -ErrorAction SilentlyContinue
            Write-Host "  [-] Terminated: $($_.Name) (PID: $($_.Id))" -ForegroundColor DarkGray
        } catch {}
    }
}
Start-Sleep -Milliseconds 800
Write-Host "  [+] Stale processes cleaned." -ForegroundColor Green
Write-Host ""

# ── STEP 2: VERIFY SERVER CONNECTIVITY ────────────────────────────────────────
Write-Host "[2/5] Validating Server Connection ($SERVER_IP`:$SERVER_PORT)..." -ForegroundColor Yellow
$connected = $false
try {
    $tcpClient = New-Object System.Net.Sockets.TcpClient
    $iar = $tcpClient.BeginConnect($SERVER_IP, $SERVER_PORT, $null, $null)
    $success = $iar.AsyncWaitHandle.WaitOne(3000, $false)
    if ($success -and $tcpClient.Connected) {
        $connected = $true
        $tcpClient.EndConnect($iar)
        $tcpClient.Close()
    }
} catch {}

if ($connected) {
    Write-Host "  [+] Server PC is ONLINE and listening on port $SERVER_PORT!" -ForegroundColor Green
} else {
    Write-Host "  [!] WARNING: Could not connect to Server at $SERVER_IP`:$SERVER_PORT." -ForegroundColor Red
    Write-Host "      Make sure 'server.py' is running on the Server PC and port 51820 is open in Windows Firewall." -ForegroundColor DarkYellow
    $choice = Read-Host "  Do you want to continue anyway? (y/N)"
    if ($choice -notmatch "^[yY]$") {
        Write-Host "Aborted by user." -ForegroundColor Red
        Read-Host "Press Enter to exit..."
        exit
    }
}
Write-Host ""

# ── STEP 3: ENSURE CLEAN HOSTS & FLUSH DNS ───────────────────────────────────
Write-Host "[3/5] Cleaning DNS and ensuring direct Riot connection..." -ForegroundColor Yellow
$hostsPath = "C:\Windows\System32\drivers\etc\hosts"

$domains = @(
    "vgc.live.riotgames.com",
    "riot-geo.pas.si.riotgames.com",
    "la.vg.ac.pvp.net"
)

$hostsLines = Get-Content $hostsPath -ErrorAction SilentlyContinue
$cleanLines = @()
foreach ($line in $hostsLines) {
    $skip = $false
    foreach ($d in $domains) {
        if ($line -match [regex]::Escape($d)) { $skip = $true; break }
    }
    if (-not $skip) { $cleanLines += $line }
}

try {
    if (Test-Path $hostsPath) {
        Set-ItemProperty -Path $hostsPath -Name IsReadOnly -Value $false -ErrorAction SilentlyContinue
    }
    [System.IO.File]::WriteAllLines($hostsPath, [string[]]$cleanLines)
    Write-Host "  [+] Hosts file cleaned of conflicting redirects." -ForegroundColor Green
} catch {}

# Flush DNS
Clear-DnsClientCache -ErrorAction SilentlyContinue
ipconfig /flushdns | Out-Null
Write-Host "  [+] DNS Cache Flushed." -ForegroundColor Green
Write-Host ""

# ── STEP 4: START VCLIENT EMULATOR ────────────────────────────────────────────
Write-Host "[4/5] Starting vClient Pipe Interceptor..." -ForegroundColor Yellow
$vClientExe = Join-Path $ROOT_DIR "vClient.exe"
if (-not (Test-Path $vClientExe)) {
    $vClientExe = Join-Path $ROOT_DIR "vgc_client.exe"
}

if (-not (Test-Path $vClientExe)) {
    Write-Host "  [-] Error: vClient.exe not found in $ROOT_DIR" -ForegroundColor Red
    Read-Host "Press Enter to exit..."
    exit
}

# Start vClient in background or new window
$vClientProc = Start-Process -FilePath $vClientExe -ArgumentList $SERVER_IP -WorkingDirectory $ROOT_DIR -PassThru -WindowStyle Minimized
Write-Host "  [+] vClient started (PID: $($vClientProc.Id)) pointing to $SERVER_IP" -ForegroundColor Green
Start-Sleep -Seconds 2
Write-Host ""

# ── STEP 5: LAUNCH RIOT CLIENT / VALORANT ──────────────────────────────────────
Write-Host "[5/5] Launching Riot Client / Valorant..." -ForegroundColor Yellow

$riotPaths = @(
    "C:\Riot Games\Riot Client\RiotClientServices.exe",
    "D:\Riot Games\Riot Client\RiotClientServices.exe",
    "E:\Riot Games\Riot Client\RiotClientServices.exe",
    "${env:ProgramFiles}\Riot Games\Riot Client\RiotClientServices.exe",
    "${env:ProgramFiles(x86)}\Riot Games\Riot Client\RiotClientServices.exe"
)

# Also check registry
try {
    $regPath = (Get-ItemProperty "HKLM:\SOFTWARE\WOW6432Node\Riot Games, Inc\Riot Client" -ErrorAction SilentlyContinue).Path
    if ($regPath -and (Test-Path $regPath)) { $riotPaths = @($regPath) + $riotPaths }
} catch {}

$foundRiot = $null
foreach ($path in $riotPaths) {
    if (Test-Path $path) {
        $foundRiot = $path
        break
    }
}

if ($foundRiot) {
    Write-Host "  [+] Found Riot Client at: $foundRiot" -ForegroundColor DarkGray
    Start-Process -FilePath $foundRiot -ArgumentList "--launch-product=valorant --launch-patchline=live"
    Write-Host "  [+] Valorant launch command sent successfully!" -ForegroundColor Green
} else {
    Write-Host "  [!] Could not locate RiotClientServices.exe automatically." -ForegroundColor DarkYellow
    Write-Host "      Please launch Valorant manually now from your shortcut." -ForegroundColor Cyan
}

Write-Host ""
Write-Host "=================================================================" -ForegroundColor Cyan
Write-Host "          [+] ALL SYSTEMS READY! GAME IS LAUNCHING...            " -ForegroundColor Cyan
Write-Host "=================================================================" -ForegroundColor Cyan
Write-Host " vClient is running and intercepting authentication...           " -ForegroundColor Gray
Write-Host " Esperando confirmacion de autenticacion de Riot...             " -ForegroundColor Yellow
Write-Host ""

$logFile = Join-Path $ROOT_DIR "vClient.log"
$readyPrinted = $false

# Monitor loop
$host.UI.RawUI.WindowTitle = "VGC Launcher - Esperando Autenticacion..."

while ($true) {
    if (Test-Path $logFile) {
        $logContent = Get-Content $logFile -ErrorAction SilentlyContinue -Raw
        if ($logContent -and $logContent -match "READY TO QUEUE") {
            if (-not $readyPrinted) {
                $readyPrinted = $true
                $host.UI.RawUI.WindowTitle = "[+] READY TO QUEUE (READY TO Q) - VGC Emulator"
                Write-Host ""
                Write-Host "=================================================================" -ForegroundColor Green
                Write-Host "       [+] >>> READY TO QUEUE (READY TO Q)! <<<                  " -ForegroundColor Green
                Write-Host "   AUTORIZACION EXITOSA CON RIOT - YA PUEDES BUSCAR PARTIDA!     " -ForegroundColor Green
                Write-Host "=================================================================" -ForegroundColor Green
                Write-Host ""
                [Console]::Beep(1000, 300) 2>$null
            }
        }
    }
    
    # Check if user pressed a key to exit
    if ([Console]::KeyAvailable) {
        $key = [Console]::ReadKey($true)
        if ($key.Key -eq [ConsoleKey]::Enter -or $key.Key -eq [ConsoleKey]::Q) {
            break
        }
    }
    
    Start-Sleep -Milliseconds 500
}

# ── CLEANUP ───────────────────────────────────────────────────────────────────
Write-Host "[*] Cleaning up session..." -ForegroundColor Cyan
if ($vClientProc -and -not $vClientProc.HasExited) {
    Stop-Process -Id $vClientProc.Id -Force -ErrorAction SilentlyContinue
    Write-Host "[+] vClient stopped." -ForegroundColor Green
}

Write-Host "[+] All clean! Have a great time, love!" -ForegroundColor Magenta
Start-Sleep -Seconds 2
