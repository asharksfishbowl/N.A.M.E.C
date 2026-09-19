#Requires -RunAsAdministrator
<#
.SYNOPSIS
  Enables OpenSSH Server on the reference PC and authorises the Dex container's key,
  so pipeline agents can compile and test N.A.M.E.C here instead of handing over.

.USAGE
  Right-click PowerShell -> Run as administrator, then:
    powershell -ExecutionPolicy Bypass -File "<repo>\Tools\Dex\setup-ssh-for-dex.ps1"
  Optional: -Repo "C:\path\to\N.A.M.E.C" if the script is run from outside the repo.

  Idempotent: safe to run again. Prints the details the Director needs at the end.
#>
param([string]$Repo)

$ErrorActionPreference = 'Stop'
$key = 'ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIND9t6VT3cKAYmzGB5osQ42uRUkrs6hyHrwBE4lkrZ4H dex-container->namec-reference-pc'

Write-Host "`n== 1. OpenSSH Server ==" -ForegroundColor Cyan
$cap = Get-WindowsCapability -Online -Name 'OpenSSH.Server*'
if ($cap.State -ne 'Installed') {
    Add-WindowsCapability -Online -Name $cap.Name | Out-Null
    Write-Host "installed $($cap.Name)"
} else { Write-Host "already installed" }
Set-Service -Name sshd -StartupType Automatic
if ((Get-Service sshd).Status -ne 'Running') { Start-Service sshd }
Write-Host "sshd: $((Get-Service sshd).Status), startup $((Get-Service sshd).StartType)"

Write-Host "`n== 2. Firewall ==" -ForegroundColor Cyan
$rule = Get-NetFirewallRule -Name 'OpenSSH-Server-In-TCP' -ErrorAction SilentlyContinue
if (-not $rule) {
    New-NetFirewallRule -Name 'OpenSSH-Server-In-TCP' -DisplayName 'OpenSSH Server (sshd)' `
        -Enabled True -Direction Inbound -Protocol TCP -Action Allow -LocalPort 22 -Profile Any | Out-Null
    Write-Host "rule created (all profiles)"
} else {
    # Docker Desktop's network often lands in the Public profile; allow all profiles.
    Set-NetFirewallRule -Name 'OpenSSH-Server-In-TCP' -Enabled True -Profile Any
    Write-Host "rule enabled for all profiles"
}

Write-Host "`n== 3. Authorised key ==" -ForegroundColor Cyan
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()
           ).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if ($isAdmin) {
    $authFile = 'C:\ProgramData\ssh\administrators_authorized_keys'
} else {
    $authFile = Join-Path $env:USERPROFILE '.ssh\authorized_keys'
    New-Item -ItemType Directory -Force (Split-Path $authFile) | Out-Null
}
if (-not (Test-Path $authFile)) { New-Item -ItemType File -Force $authFile | Out-Null }
if (-not (Select-String -Path $authFile -SimpleMatch 'dex-container->namec-reference-pc' -Quiet)) {
    Add-Content -Path $authFile -Value $key
    Write-Host "key added to $authFile"
} else { Write-Host "key already present in $authFile" }
if ($isAdmin) {
    icacls $authFile /inheritance:r /grant 'Administrators:F' /grant 'SYSTEM:F' | Out-Null
    Write-Host "ACLs set (Administrators + SYSTEM only)"
}

Write-Host "`n== 4. Details to send back to the Director ==" -ForegroundColor Cyan
if (-not $Repo) {
    # Walk up from the script's folder until a .git directory is found.
    $dir = $PSScriptRoot
    while ($dir -and -not (Test-Path (Join-Path $dir '.git'))) { $dir = Split-Path $dir -Parent }
    $Repo = $dir
}
$ue = Get-ChildItem 'C:\Program Files\Epic Games' -Directory -Filter 'UE_*' -ErrorAction SilentlyContinue |
      Sort-Object Name -Descending | Select-Object -First 1 -ExpandProperty FullName
Write-Host "USERNAME : $env:USERNAME"
Write-Host "HOSTNAME : $env:COMPUTERNAME"
Write-Host "REPO     : $(if ($Repo) { $Repo } else { 'not found - pass -Repo C:\path\to\N.A.M.E.C' })"
Write-Host "UNREAL   : $(if ($ue) { $ue } else { 'NOT FOUND under C:\Program Files\Epic Games - tell the Director where it is' })"
Write-Host "`nDone. Paste the four lines above back into the chat." -ForegroundColor Green
