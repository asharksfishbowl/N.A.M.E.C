# SSH-side caller: drops a trigger file in the job queue directory and polls for the result.
# Requires the interactive-launcher-watcher.ps1 task to be running in ashar's logon session.
# Usage:
#   $r = & .\interactive-launch.ps1 -Exe $Editor -Args @(arg1, arg2) -ProjectRoot $ProjectRoot
#   if ($r.exitCode -ne 0) { throw "UE launch failed: $($r.exitCode)" }

param(
    [Parameter(Mandatory=$true)]  [string]  $Exe,
    [Parameter(Mandatory=$true)]  [string[]]$Args,
    [string] $ProjectRoot = "C:\Users\ashar\repos\N.A.M.E.C",
    [int]    $TimeoutSec  = 600
)

# Fail fast if no interactive session — watcher won't be running.
if (-not (Get-Process explorer -ErrorAction SilentlyContinue)) {
    throw "No interactive session on reference PC — GPU job cannot run (explorer.exe not found)"
}

$watchDir = "$ProjectRoot\Saved\UEJobs"
New-Item -ItemType Directory -Force -Path $watchDir | Out-Null

$jobId = [Guid]::NewGuid().ToString("N")
@{ exe = $Exe; args = $Args } | ConvertTo-Json |
    Set-Content "$watchDir\$jobId.trigger"

$deadline = (Get-Date).AddSeconds($TimeoutSec)
while ((Get-Date) -lt $deadline) {
    $done = "$watchDir\$jobId.done"
    if (Test-Path $done) {
        $result = Get-Content $done | ConvertFrom-Json
        Remove-Item $done
        return $result    # { exitCode, log }
    }
    Start-Sleep 2
}

# Clean up orphaned trigger before throwing.
Remove-Item "$watchDir\$jobId.trigger" -ErrorAction SilentlyContinue
throw "Timeout: UE job $jobId did not complete in ${TimeoutSec}s"
