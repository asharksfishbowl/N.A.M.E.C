$count = (Get-Process -Name UnrealEditor -ErrorAction SilentlyContinue | Measure-Object).Count
if ($count -gt 0) {
    Write-Error "UnrealEditor is running ($count instance(s)). Stop it before building or merging."
    exit 1
}
exit 0
