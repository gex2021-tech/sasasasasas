# Cleanup script - Remove duplicate/legacy folders and reorganize project

param(
    [switch]$DryRun = $false,
    [switch]$Force = $false
)

Write-Host "VGC Emulator - Project Cleanup" -ForegroundColor Cyan
Write-Host "================================" -ForegroundColor Cyan
Write-Host ""

if ($DryRun) {
    Write-Host "[DRY RUN MODE] No files will be deleted" -ForegroundColor Yellow
    Write-Host ""
}

function Remove-SafePath {
    param(
        [string]$Path,
        [string]$Description
    )
    
    if (Test-Path $Path) {
        $item = Get-Item $Path
        $size = 0
        if ($item.PSIsContainer) {
            $size = (Get-ChildItem $Path -Recurse | Measure-Object -Property Length -Sum).Sum
        } else {
            $size = $item.Length
        }
        $sizeKB = [math]::Round($size / 1KB, 2)
        
        Write-Host "[*] Found: $Description" -ForegroundColor Gray
        Write-Host "    Path: $Path" -ForegroundColor DarkGray
        Write-Host "    Size: $sizeKB KB" -ForegroundColor DarkGray
        
        if (-not $DryRun) {
            if (-not $Force) {
                $confirm = Read-Host "    Delete? (y/N)"
                if ($confirm -ne 'y' -and $confirm -ne 'Y') {
                    Write-Host "    [SKIPPED]" -ForegroundColor Yellow
                    return
                }
            }
            
            try {
                Remove-Item $Path -Recurse -Force -ErrorAction Stop
                Write-Host "    [DELETED]" -ForegroundColor Red
            } catch {
                Write-Host "    [ERROR] $_" -ForegroundColor Red
            }
        } else {
            Write-Host "    [WOULD DELETE]" -ForegroundColor Yellow
        }
    }
}

# Legacy/duplicate folders to clean up
Write-Host "Checking for legacy/duplicate folders..." -ForegroundColor Cyan
Write-Host ""

Remove-SafePath "vgc_emu_temp" "Empty VGC emulator temp folder"
Remove-SafePath "vgc_emulator" "Empty VGC emulator folder"
Remove-SafePath "temp_repo" "Temporary repository folder"
Remove-SafePath "src" "Empty src folder (legacy)"

# Check amulator/src (legacy C++ server)
if (Test-Path "amulator/src") {
    $hasFiles = (Get-ChildItem "amulator/src" -Recurse -File).Count -gt 0
    if ($hasFiles) {
        Write-Host ""
        Write-Host "[!] amulator/src contains legacy C++ server code" -ForegroundColor Yellow
        Write-Host "    This has been replaced by the Python server in server/" -ForegroundColor Yellow
        Write-Host "    It's already backed up in legacy/ folder" -ForegroundColor Yellow
        Remove-SafePath "amulator/src" "Legacy C++ server implementation"
    }
}

# Check emu folder
if (Test-Path "emu") {
    $emuEmpty = (Get-ChildItem "emu" -Recurse -File -Exclude "*.pem","*.key").Count -eq 0
    if ($emuEmpty) {
        Write-Host ""
        Write-Host "[!] emu/ folder is mostly empty (only certs)" -ForegroundColor Yellow
        Remove-SafePath "emu" "Empty emu folder (certs are in certs/ folder)"
    }
}

# Check sheyko folder
if (Test-Path "sheyko (1)") {
    Write-Host ""
    Write-Host "[!] 'sheyko (1)' folder appears to be another cheat/project" -ForegroundColor Yellow
    Write-Host "    Not related to the VGC emulator" -ForegroundColor Yellow
    Remove-SafePath "sheyko (1)" "Unrelated project folder"
}

# Clean build artifacts
Write-Host ""
Write-Host "Checking for build artifacts..." -ForegroundColor Cyan
Write-Host ""

$artifacts = @(
    @{Path="*.obj"; Desc="Object files"},
    @{Path="*.ilk"; Desc="Incremental link files"},
    @{Path="*.pdb"; Desc="Debug symbols"},
    @{Path="*.exp"; Desc="Export files"}
)

foreach ($artifact in $artifacts) {
    $files = Get-ChildItem -Path . -Filter $artifact.Path -Recurse -ErrorAction SilentlyContinue
    if ($files.Count -gt 0) {
        $totalSize = ($files | Measure-Object -Property Length -Sum).Sum
        $totalSizeKB = [math]::Round($totalSize / 1KB, 2)
        
        Write-Host "[*] Found $($files.Count) $($artifact.Desc)" -ForegroundColor Gray
        Write-Host "    Total size: $totalSizeKB KB" -ForegroundColor DarkGray
        
        if (-not $DryRun) {
            if (-not $Force) {
                $confirm = Read-Host "    Delete all? (y/N)"
                if ($confirm -ne 'y' -and $confirm -ne 'Y') {
                    Write-Host "    [SKIPPED]" -ForegroundColor Yellow
                    continue
                }
            }
            
            foreach ($file in $files) {
                try {
                    Remove-Item $file.FullName -Force -ErrorAction Stop
                } catch {
                    Write-Host "    [ERROR] $($file.Name): $_" -ForegroundColor Red
                }
            }
            Write-Host "    [DELETED $($files.Count) files]" -ForegroundColor Red
        } else {
            Write-Host "    [WOULD DELETE $($files.Count) files]" -ForegroundColor Yellow
        }
    }
}

# Clean logs if they're too old/large
Write-Host ""
Write-Host "Checking logs..." -ForegroundColor Cyan
Write-Host ""

if (Test-Path "logs") {
    $logSize = (Get-ChildItem "logs" -Recurse -File | Measure-Object -Property Length -Sum).Sum
    $logSizeMB = [math]::Round($logSize / 1MB, 2)
    
    if ($logSize -gt 10MB) {
        Write-Host "[*] Logs folder is $logSizeMB MB" -ForegroundColor Gray
        Write-Host "    Consider archiving old logs" -ForegroundColor Yellow
        
        if (-not $DryRun -and -not $Force) {
            $clean = Read-Host "    Clear all logs? (y/N)"
            if ($clean -eq 'y' -or $clean -eq 'Y') {
                Remove-Item "logs/*" -Recurse -Force -ErrorAction SilentlyContinue
                Write-Host "    [CLEARED]" -ForegroundColor Red
            }
        }
    } else {
        Write-Host "[*] Logs folder: $logSizeMB MB (OK)" -ForegroundColor Green
    }
}

# Summary
Write-Host ""
Write-Host "================================" -ForegroundColor Cyan
Write-Host "Cleanup complete!" -ForegroundColor Green
Write-Host ""

if ($DryRun) {
    Write-Host "Run without -DryRun to actually delete files" -ForegroundColor Yellow
    Write-Host "Add -Force to skip confirmations" -ForegroundColor Yellow
} else {
    Write-Host "Project structure cleaned." -ForegroundColor Green
    Write-Host ""
    Write-Host "Active folders:" -ForegroundColor Cyan
    Write-Host "  server/     - Python emulator (main)" -ForegroundColor White
    Write-Host "  antivgc/    - Game cheat (separate project)" -ForegroundColor White
    Write-Host "  legacy/     - Archived prototypes" -ForegroundColor White
    Write-Host "  certs/      - TLS certificates" -ForegroundColor White
    Write-Host "  build/      - Build output" -ForegroundColor White
}

Write-Host ""
