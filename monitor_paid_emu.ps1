# Real-time System & Network Sniffer for Paid Emulator Analysis
Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "[*] MONITORING ACTIVE - RUN THE PAID EMULATOR NOW" -ForegroundColor Green
Write-Host "[*] Capturing network connections, pipes, and processes..." -ForegroundColor Yellow
Write-Host "==========================================================" -ForegroundColor Cyan

$logPath = "c:\Users\gex20\OneDrive\Escritorio\amulator\paid_emu_capture.log"
"--- CAPTURE SESSION STARTED AT $(Get-Date) ---" | Out-File -FilePath $logPath -Encoding utf8

$knownPipes = @()
$knownConns = @()

while ($true) {
    # 1. Capture Named Pipes
    try {
        $pipes = [System.IO.Directory]::GetFiles("\\.\\pipe\\")
        foreach ($p in $pipes) {
            $pName = $p.Replace("\\.\pipe\", "")
            if ($knownPipes -notcontains $pName -and ($pName -match "vgc|vgk|vanguard|emu|auth|hook|pipe|riot|offset|inject|933823D3")) {
                $knownPipes += $pName
                $msg = "[PIPE DETECTED] $pName at $(Get-Date -Format 'HH:mm:ss.fff')"
                Write-Host $msg -ForegroundColor Magenta
                $msg | Out-File -FilePath $logPath -Append -Encoding utf8
            }
        }
    } catch {}

    # 2. Capture Network Connections
    try {
        $conns = Get-NetTCPConnection -ErrorAction SilentlyContinue | Where-Object { $_.State -eq "Established" -or $_.State -eq "SynSent" }
        foreach ($c in $conns) {
            $key = "$($c.OwningProcess):$($c.LocalPort)->$($c.RemoteAddress):$($c.RemotePort)"
            if ($knownConns -notcontains $key) {
                $knownConns += $key
                $pName = (Get-Process -Id $c.OwningProcess -ErrorAction SilentlyContinue).ProcessName
                if ($pName -match "Emu|vgc|val|riot|loader|wand|python|vClient" -or $c.RemotePort -in @(51820, 8443, 443, 2083, 8080, 50051)) {
                    $msg = "[NET] PID $($c.OwningProcess) ($pName) -> $($c.RemoteAddress):$($c.RemotePort) [State: $($c.State)] at $(Get-Date -Format 'HH:mm:ss.fff')"
                    Write-Host $msg -ForegroundColor Green
                    $msg | Out-File -FilePath $logPath -Append -Encoding utf8
                }
            }
        }
    } catch {}

    Start-Sleep -Milliseconds 200
}
