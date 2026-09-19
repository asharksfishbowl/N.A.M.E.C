# OSS Null loopback session test.
# Proves create/find/join under DefaultPlatformService=Null on one machine.
# Does NOT prove cross-machine LAN broadcast — that is a Phase 3 user sitting
# with a second device.
# Run: .\test-oss-loopback.ps1 "C:\Users\ashar\repos\N.A.M.E.C" <sha>
# Run only when no other build or editor session is in flight.

param(
    [Parameter(Mandatory=$true,Position=0)] [string]$ProjectRoot,
    [Parameter(Mandatory=$false,Position=1)] [string]$Sha = "nosha"
)

$Engine  = "C:\Program Files\Epic Games\UE_5.8"
$Editor  = "$Engine\Engine\Binaries\Win64\UnrealEditor.exe"
$Uproject = "$ProjectRoot\NAMEC.uproject"
$LogDir  = "$ProjectRoot\Saved\Logs"

New-Item -ItemType Directory -Force -Path $LogDir | Out-Null

$HostLog   = "$LogDir\oss-loopback-host-$Sha.log"
$ClientLog = "$LogDir\oss-loopback-client-$Sha.log"

Write-Host "Starting host instance..."
$HostProc = Start-Process -FilePath $Editor -PassThru -ArgumentList @(
    "`"$Uproject`"",
    "-game", "-nullrhi",
    "-log=oss-loopback-host-$Sha",
    "-ExecCmds=`"online test sessions NumConnections=2, quit`"",
    "-nosteam", "-nosplash", "-unattended"
)

Start-Sleep -Seconds 2

Write-Host "Starting client instance..."
$ClientProc = Start-Process -FilePath $Editor -PassThru -ArgumentList @(
    "`"$Uproject`"",
    "-game", "-nullrhi",
    "-log=oss-loopback-client-$Sha",
    "-ExecCmds=`"online test sessions NumConnections=2 ServerName=LoopbackTest, quit`"",
    "-nosteam", "-nosplash", "-unattended"
)

Write-Host "Waiting for both instances to exit..."
$HostProc.WaitForExit()
$ClientProc.WaitForExit()

if ($HostProc.ExitCode -ne 0) {
    Write-Error "Host process exited with code $($HostProc.ExitCode)"
    exit 1
}
if ($ClientProc.ExitCode -ne 0) {
    Write-Error "Client process exited with code $($ClientProc.ExitCode)"
    exit 1
}

# Check client log for successful session join.
$ClientLogContent = Get-Content $ClientLog -Raw -ErrorAction SilentlyContinue
if ($ClientLogContent -match "online test sessions" -and $ClientLogContent -match "Successful") {
    Write-Host "OSS Null loopback: session join SUCCESSFUL"
    exit 0
} else {
    Write-Error "OSS Null loopback: session join NOT found in client log. Check $ClientLog"
    exit 1
}
