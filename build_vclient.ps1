# Build script for vClient.exe
# Compiles the C++ tunnel client with proper configuration

param(
    [string]$VpsHost = "192.168.1.136",
    [string]$AuthKey = "feqxYc-ilusao",
    [int]$VpsPort = 51820,
    [string]$BuildType = "Release"
)

Write-Host "[*] Building vClient.exe..." -ForegroundColor Cyan
Write-Host "    VPS Host: $VpsHost" -ForegroundColor Gray
Write-Host "    VPS Port: $VpsPort" -ForegroundColor Gray
Write-Host "    Auth Key: $AuthKey" -ForegroundColor Gray

# Create build directory
$buildDir = "build"
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# Find compiler
$compilerType = "none"
$vcvarsPath = ""
$clangPath = ""

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
    $vsPath = & $vswhere -latest -property installationPath
    $testVcvars = Join-Path $vsPath "VC\Auxiliary\Build\vcvars64.bat"
    if (Test-Path $testVcvars) {
        $compilerType = "msvc"
        $vcvarsPath = $testVcvars
    }
}

if ($compilerType -eq "none") {
    $clangCmd = Get-Command clang++ -ErrorAction SilentlyContinue
    if ($clangCmd) {
        $compilerType = "clang"
        $clangPath = $clangCmd.Source
    } else {
        $gxxCmd = Get-Command g++ -ErrorAction SilentlyContinue
        if ($gxxCmd) {
            $compilerType = "gcc"
            $clangPath = $gxxCmd.Source
        }
    }
}

if ($compilerType -eq "none") {
    Write-Host "[!] No C++ compiler found (MSVC, Clang, or GCC)." -ForegroundColor Red
    exit 1
}

Write-Host "[+] Using compiler: $compilerType ($clangPath $vcvarsPath)" -ForegroundColor Green

# Compile
Write-Host "[*] Compiling vClient.exe..." -ForegroundColor Cyan

if ($compilerType -eq "msvc") {
    $compileCmd = @"
call "$vcvarsPath" && ^
cl server\main.cpp /EHsc /std:c++17 /O2 /W3 /Fo"$buildDir\\" /Fe"$buildDir\vClient.exe" ^
   /link winhttp.lib ws2_32.lib secur32.lib bcrypt.lib Crypt32.lib Advapi32.lib ^
   2>&1
"@
    $result = cmd /c $compileCmd
    $exitCode = $LASTEXITCODE
} else {
    & $clangPath server/main.cpp -std=c++17 -O2 -o "$buildDir/vClient.exe" -lwinhttp -lws2_32 -lsecur32 -lbcrypt -lcrypt32 -ladvapi32
    $exitCode = $LASTEXITCODE
}

if ($exitCode -eq 0 -and (Test-Path "$buildDir\vClient.exe")) {
    Write-Host ""
    Write-Host "[+] Build successful!" -ForegroundColor Green
    Write-Host "[*] Output: $buildDir\vClient.exe" -ForegroundColor Cyan
    
    Copy-Item "$buildDir\vClient.exe" "$PSScriptRoot\vClient.exe" -Force
    Write-Host "[*] Copied to: $PSScriptRoot\vClient.exe" -ForegroundColor Cyan
    
    # Show file size
    $fileSize = (Get-Item "$buildDir\vClient.exe").Length
    $fileSizeKB = [math]::Round($fileSize / 1KB, 2)
    Write-Host "[*] Size: $fileSizeKB KB" -ForegroundColor Gray
} else {
    Write-Host ""
    Write-Host "[!] Build failed" -ForegroundColor Red
    if ($result) { Write-Host $result }
    exit 1
}

# Create run script
$runScript = @"
@echo off
echo Starting vClient...
echo VPS: $VpsHost`:$VpsPort
echo Auth Key: $AuthKey
echo.
"%~dp0build\vClient.exe"
pause
"@

$runScript | Out-File -FilePath "run_vclient.bat" -Encoding ASCII
Write-Host "[*] Created run_vclient.bat for easy testing" -ForegroundColor Cyan
