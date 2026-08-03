# Self-elevate to admin
if (-not ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Start-Process PowerShell -ArgumentList "-NoProfile -ExecutionPolicy Bypass -File "$PSCommandPath"" -Verb RunAs
    exit
}

$hostsPath = "C:\Windows\System32\drivers\etc\hosts"
$domains = @(
    "vgc.live.riotgames.com",
    "riot-geo.pas.si.riotgames.com",
    "la.vg.ac.pvp.net"
)

$content = Get-Content $hostsPath
$newContent = @()

foreach ($line in $content) {
    $remove = $false
    foreach ($domain in $domains) {
        if ($line -match $domain) {
            $remove = $true
            break
        }
    }
    if (-not $remove) {
        $newContent += $line
    }
}

Set-Content -Path $hostsPath -Value $newContent
Write-Host "Removed VGC Emulator entries from hosts file." -ForegroundColor Green
Read-Host "Press Enter to exit"
