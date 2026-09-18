# Headless Lumen stand-in screenshot for the voxel mesher output.
# Proves: TestChunk mesh loads, chunks draw through the custom scene proxy,
#         D3D12 + HWRT + Lumen path is active on the reference PC GPU.
# Cannot prove: visual quality, Lumen settling over time, sitting-2 kill-switch judgements.
#
# Prerequisites:
#   1. feature/benchmark-milestone/base merged through the mesher task (451852d).
#   2. /Game/Benchmark/TestChunk.uasset created: run NamecMeshExport -Chunk=0,0 first.
#   3. No other build or editor session in flight on the reference PC.
#   4. Run from a .worktrees/ checkout of base (e.g. .worktrees/0918-5-lumen-screenshot).
#   5. interactive-launcher-watcher.ps1 registered and running in ashar's logon session.
#
# Usage (via SSH from the container):
#   .\headless-lumen-screenshot.ps1 "C:\Users\ashar\repos\worktrees\0918-5-lumen-screenshot" <sha>
#
# Output:
#   - Key RHI/Lumen log lines printed to stdout.
#   - PNG path printed if screenshot was produced.
#   - Exits non-zero if UE exits with an error code.

param(
    [Parameter(Mandatory=$true,  Position=0)] [string]$ProjectRoot,
    [Parameter(Mandatory=$false, Position=1)] [string]$Sha = "nosha"
)

$Engine   = "C:\Program Files\Epic Games\UE_5.8"
$Editor   = "$Engine\Engine\Binaries\Win64\UnrealEditor.exe"
$Uproject = "$ProjectRoot\NAMEC.uproject"
$LogDir   = "$ProjectRoot\Saved\Logs"
$ShotDir  = "$ProjectRoot\Saved\Screenshots\Windows"
$Log      = "$LogDir\lumen-headless-$Sha.log"

New-Item -ItemType Directory -Force -Path $LogDir  | Out-Null
New-Item -ItemType Directory -Force -Path $ShotDir | Out-Null

# ExecCmds sequence:
#   1. Spawn test scene (TestChunk + white cube + directional light)
#   2. Enable Lumen Surface Cache visualizer
#   3. Capture 1920x1080 screenshot (lands in Saved/Screenshots/Windows/)
#   4. Quit
$ExecCmds = "NamecBenchmark.SpawnLumenTest,r.Lumen.Visualize.Overview 1,HighResShot 1920x1080,Quit"

$r = & "$PSScriptRoot\interactive-launch.ps1" `
        -Exe $Editor `
        -Args @($Uproject, "/Engine/Maps/Entry",
                "-game", "-d3d12", "-RenderOffScreen",
                "-nosplash", "-unattended",
                "-ExecCmds=$ExecCmds",
                "-log=lumen-headless-$Sha") `
        -ProjectRoot $ProjectRoot

$Exit = $r.exitCode

if (Test-Path $Log)
{
    Write-Host ""
    Write-Host "=== RHI / Lumen log lines ==="
    Select-String -Path $Log `
        -Pattern "D3D12|RHI:|DXR|HWRT|Lumen|RHI init|LogRHI|failed|error" `
        | Select-Object -First 40 `
        | ForEach-Object { Write-Host $_.Line }
}

Write-Host ""
Write-Host "=== Screenshot ==="
$PNG = Get-ChildItem -Path $ShotDir -Filter "*.png" -ErrorAction SilentlyContinue `
       | Sort-Object LastWriteTime -Descending `
       | Select-Object -First 1

if ($PNG)
{
    Write-Host "PNG: $($PNG.FullName)"
    Write-Host "headless render possible: yes"
}
else
{
    Write-Host "No PNG found under $ShotDir"
    Write-Host "headless render possible: no"
}

exit $Exit
