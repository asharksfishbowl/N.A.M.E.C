# Watcher: runs as ashar in the interactive session (Session 1 with GPU access).
# Watches $WatchDir for *.trigger files dropped by interactive-launch.ps1 via SSH.
# Register as a logon scheduled task once (one user sitting):
#   schtasks /create /TN NamecInteractiveLauncher /SC ONLOGON /RU MSI\ashar /IT /RP <password> `
#     /TR "powershell -NoProfile -NonInteractive -File C:\Users\ashar\repos\N.A.M.E.C\Tools\Build\interactive-launcher-watcher.ps1"
# Then start: schtasks /run /TN NamecInteractiveLauncher

param(
    [string]$ProjectRoot = "C:\Users\ashar\repos\N.A.M.E.C"
)

$WatchDir = "$ProjectRoot\Saved\UEJobs"
New-Item -ItemType Directory -Force -Path $WatchDir | Out-Null

while ($true) {
    Get-ChildItem "$WatchDir\*.trigger" | ForEach-Object {
        $job    = Get-Content $_.FullName | ConvertFrom-Json
        $jobId  = $_.BaseName
        $logOut = "$WatchDir\$jobId.log"
        $proc   = Start-Process $job.exe `
                    -ArgumentList $job.args `
                    -RedirectStandardOutput $logOut `
                    -PassThru -Wait -WindowStyle Hidden
        @{ exitCode = $proc.ExitCode; log = $logOut } |
            ConvertTo-Json | Set-Content "$WatchDir\$jobId.done"
        Remove-Item $_.FullName
    }
    Start-Sleep 2
}
