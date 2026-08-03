# Self-elevate to admin
if (-not ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Start-Process PowerShell -ArgumentList "-NoProfile -ExecutionPolicy Bypass -File `"$PSCommandPath`"" -Verb RunAs
    exit
}

Write-Host "VGC Emulator - Gaming PC Setup" -ForegroundColor Cyan

# Ask for IP with validation
$ipValid = $false
while (-not $ipValid) {
    $serverIp = Read-Host "Enter the server PC's IP address"
    if ($serverIp -match "^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$") {
        $ipValid = $true
    } else {
        Write-Host "Invalid IP address format. Please try again." -ForegroundColor Red
    }
}

$hostsPath = "C:\Windows\System32\drivers\etc\hosts"
$backupPath = "$hostsPath.bak"

# Backup hosts file
if (-not (Test-Path $backupPath)) {
    Copy-Item $hostsPath -Destination $backupPath
    Write-Host "Created hosts backup at $backupPath" -ForegroundColor Green
} else {
    Write-Host "Backup already exists at $backupPath" -ForegroundColor Yellow
}

# Domains to add
$domains = @(
    "vgc.live.riotgames.com",
    "riot-geo.pas.si.riotgames.com",
    "la.vg.ac.pvp.net"
)

# Add entries
$hostsContent = Get-Content $hostsPath
$changed = $false

foreach ($domain in $domains) {
    $entry = "$serverIp`t$domain"
    if ($hostsContent -notcontains $entry) {
        Add-Content -Path $hostsPath -Value $entry
        $changed = $true
    }
}

if ($changed) {
    Write-Host "Successfully added entries to hosts file." -ForegroundColor Green
} else {
    Write-Host "Entries already exist in hosts file." -ForegroundColor Yellow
}

# Create restore script
$restoreScriptPath = Join-Path $PSScriptRoot "restore_hosts.ps1"
$restoreScriptContent = @"
# Self-elevate to admin
if (-not ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Start-Process PowerShell -ArgumentList "-NoProfile -ExecutionPolicy Bypass -File `"`$PSCommandPath`"" -Verb RunAs
    exit
}

`$hostsPath = "C:\Windows\System32\drivers\etc\hosts"
`$domains = @(
    "vgc.live.riotgames.com",
    "riot-geo.pas.si.riotgames.com",
    "la.vg.ac.pvp.net"
)

`$content = Get-Content `$hostsPath
`$newContent = @()

foreach (`$line in `$content) {
    `$remove = `$false
    foreach (`$domain in `$domains) {
        if (`$line -match `$domain) {
            `$remove = `$true
            break
        }
    }
    if (-not `$remove) {
        `$newContent += `$line
    }
}

Set-Content -Path `$hostsPath -Value `$newContent
Write-Host "Removed VGC Emulator entries from hosts file." -ForegroundColor Green
Read-Host "Press Enter to exit"
"@

Set-Content -Path $restoreScriptPath -Value $restoreScriptContent
Write-Host "Created restore script at $restoreScriptPath" -ForegroundColor Green

# Test connectivity
Write-Host "Testing connectivity to $serverIp on port 51820..." -ForegroundColor Cyan
try {
    $tcpClient = New-Object System.Net.Sockets.TcpClient
    $tcpClient.Connect($serverIp, 51820)
    if ($tcpClient.Connected) {
        Write-Host "Connection successful!" -ForegroundColor Green
        $tcpClient.Close()
    }
} catch {
    Write-Host "Could not connect to $serverIp`:51820. Make sure the server is running." -ForegroundColor Red
}

Write-Host "Setup complete!" -ForegroundColor Cyan
Read-Host "Press Enter to exit"
