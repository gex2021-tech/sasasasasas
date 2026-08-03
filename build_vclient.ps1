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

# Check if Visual Studio is available
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    Write-Host "[!] Visual Studio not found. Please install Visual Studio 2019 or later with C++ tools." -ForegroundColor Red
    exit 1
}

$vsPath = & $vswhere -latest -property installationPath
$vcvarsPath = Join-Path $vsPath "VC\Auxiliary\Build\vcvars64.bat"

if (-not (Test-Path $vcvarsPath)) {
    Write-Host "[!] vcvars64.bat not found. Ensure C++ workload is installed." -ForegroundColor Red
    exit 1
}

# Create build directory
$buildDir = "build"
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# Create config header with parameters
$configHeader = @"
// Auto-generated config - DO NOT EDIT MANUALLY
#pragma once
#define VPS_HOST_AUTO "$VpsHost"
#define VPS_PORT_AUTO $VpsPort
#define AUTH_KEY_AUTO "$AuthKey"
#define TLS_SKIP_VERIFY_AUTO true
"@

$configPath = "server\vclient_config.h"
$configHeader | Out-File -FilePath $configPath -Encoding UTF8

Write-Host "[*] Generated config header: $configPath" -ForegroundColor Green

# Update main.cpp to use auto-config if it exists
$mainCppPath = "server\main.cpp"
if (Test-Path $mainCppPath) {
    $content = Get-Content $mainCppPath -Raw
    
    # Check if auto-config support exists
    if ($content -notmatch "#ifdef VPS_HOST_AUTO") {
        Write-Host "[*] Patching main.cpp for auto-config support..." -ForegroundColor Yellow
        
        # Add include after other includes
        $includeMarker = '#include <vector>'
        if ($content -match [regex]::Escape($includeMarker)) {
            $patchedContent = $content -replace [regex]::Escape($includeMarker), @"
$includeMarker

// Auto-generated config support
#if __has_include("vclient_config.h")
#include "vclient_config.h"
#endif
"@
            
            # Replace hardcoded values with auto-config
            $patchedContent = $patchedContent -replace 'static std::string\s+g_vps_host\s*=\s*"[^"]*";', @"
#ifdef VPS_HOST_AUTO
static std::string g_vps_host = VPS_HOST_AUTO;
#else
static std::string g_vps_host = "192.168.1.136";
#endif
"@
            
            $patchedContent = $patchedContent -replace 'constexpr uint16_t\s+VPS_PORT\s*=\s*\d+;', @"
#ifdef VPS_PORT_AUTO
constexpr uint16_t VPS_PORT = VPS_PORT_AUTO;
#else
constexpr uint16_t VPS_PORT = 51820;
#endif
"@
            
            $patchedContent = $patchedContent -replace 'constexpr const char\* AUTH_KEY\s*=\s*"[^"]*";', @"
#ifdef AUTH_KEY_AUTO
constexpr const char* AUTH_KEY = AUTH_KEY_AUTO;
#else
constexpr const char* AUTH_KEY = "feqxYc-ilusao";
#endif
"@
            
            $patchedContent = $patchedContent -replace 'constexpr bool\s+TLS_SKIP_VERIFY\s*=\s*(true|false);', @"
#ifdef TLS_SKIP_VERIFY_AUTO
constexpr bool TLS_SKIP_VERIFY = TLS_SKIP_VERIFY_AUTO;
#else
constexpr bool TLS_SKIP_VERIFY = true;
#endif
"@
            
            $patchedContent | Out-File -FilePath $mainCppPath -Encoding UTF8 -NoNewline
            Write-Host "[+] main.cpp patched successfully" -ForegroundColor Green
        }
    }
}

# Compile
Write-Host "[*] Compiling vClient.exe..." -ForegroundColor Cyan

$compileCmd = @"
call "$vcvarsPath" && ^
cl server\main.cpp /EHsc /std:c++17 /O2 /W3 /Fo"$buildDir\\" /Fe"$buildDir\vClient.exe" ^
   /link winhttp.lib ws2_32.lib secur32.lib bcrypt.lib Crypt32.lib Advapi32.lib ^
   2>&1
"@

$result = cmd /c $compileCmd

$exitCode = $LASTEXITCODE

if ($exitCode -eq 0 -and (Test-Path "$buildDir\vClient.exe")) {
    Write-Host "" -NoNewline
    Write-Host "[+] Build successful!" -ForegroundColor Green
    Write-Host "[*] Output: $buildDir\vClient.exe" -ForegroundColor Cyan
    
    # Show file size
    $fileSize = (Get-Item "$buildDir\vClient.exe").Length
    $fileSizeKB = [math]::Round($fileSize / 1KB, 2)
    Write-Host "[*] Size: $fileSizeKB KB" -ForegroundColor Gray
    
    # Test configuration
    Write-Host "" -NoNewline
    Write-Host "[*] Testing configuration..." -ForegroundColor Cyan
    $testOutput = & "$buildDir\vClient.exe" --help 2>&1
    if ($testOutput -match "vClient" -or $?) {
        Write-Host "[+] vClient.exe is ready to use" -ForegroundColor Green
    } else {
        Write-Host "[!] Warning: vClient.exe may not be functional" -ForegroundColor Yellow
    }
    
} else {
    Write-Host "" -NoNewline
    Write-Host "[!] Build failed" -ForegroundColor Red
    Write-Host $result
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

Write-Host "" -NoNewline
Write-Host "[*] Created run_vclient.bat for easy testing" -ForegroundColor Cyan
