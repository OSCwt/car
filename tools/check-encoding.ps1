param(
    [string]$Root = "."
)

$ErrorActionPreference = "Stop"

function Test-StrictUtf8 {
    param(
        [byte[]]$Bytes
    )

    $utf8 = New-Object System.Text.UTF8Encoding($false, $true)
    try {
        [void]$utf8.GetString($Bytes)
        return $true
    } catch {
        return $false
    }
}

function Get-LineEndingState {
    param(
        [byte[]]$Bytes
    )

    $hasCrLf = $false
    $hasLfOnly = $false
    $hasCrOnly = $false

    for ($i = 0; $i -lt $Bytes.Length; $i++) {
        if ($Bytes[$i] -eq 13) {
            if (($i + 1) -lt $Bytes.Length -and $Bytes[$i + 1] -eq 10) {
                $hasCrLf = $true
                $i++
            } else {
                $hasCrOnly = $true
            }
        } elseif ($Bytes[$i] -eq 10) {
            $hasLfOnly = $true
        }
    }

    if ($hasCrOnly) { return "CR_ONLY" }
    if ($hasCrLf -and $hasLfOnly) { return "MIXED_CRLF_LF" }
    if ($hasCrLf) { return "CRLF" }
    if ($hasLfOnly) { return "LF" }
    return "NONE"
}

function Has-Utf8Bom {
    param(
        [byte[]]$Bytes
    )

    return ($Bytes.Length -ge 3 -and $Bytes[0] -eq 0xEF -and $Bytes[1] -eq 0xBB -and $Bytes[2] -eq 0xBF)
}

$resolvedRoot = Resolve-Path $Root
$targetDir = Join-Path $resolvedRoot "code"

if (-not (Test-Path $targetDir)) {
    Write-Error "Directory not found: $targetDir"
}

$patterns = @("*.c", "*.h")
$files = foreach ($pattern in $patterns) {
    Get-ChildItem -Path $targetDir -Recurse -File -Filter $pattern
}

$files = $files | Sort-Object FullName -Unique

$issues = New-Object System.Collections.Generic.List[object]

foreach ($file in $files) {
    $bytes = [System.IO.File]::ReadAllBytes($file.FullName)
    $utf8Ok = Test-StrictUtf8 -Bytes $bytes
    $hasBom = Has-Utf8Bom -Bytes $bytes
    $eolState = Get-LineEndingState -Bytes $bytes

    if (-not $utf8Ok) {
        $issues.Add([PSCustomObject]@{
            File = $file.FullName
            Issue = "INVALID_UTF8"
            Detail = "File is not strict UTF-8 decodable."
        }) | Out-Null
    }

    if ($hasBom) {
        $issues.Add([PSCustomObject]@{
            File = $file.FullName
            Issue = "UTF8_BOM"
            Detail = "UTF-8 BOM found; repository standard is UTF-8 without BOM."
        }) | Out-Null
    }

    if ($eolState -eq "MIXED_CRLF_LF") {
        $issues.Add([PSCustomObject]@{
            File = $file.FullName
            Issue = "MIXED_LINE_ENDINGS"
            Detail = "Both CRLF and LF line endings detected."
        }) | Out-Null
    } elseif ($eolState -eq "LF") {
        $issues.Add([PSCustomObject]@{
            File = $file.FullName
            Issue = "LF_ONLY"
            Detail = "LF-only line endings detected; project standard is CRLF."
        }) | Out-Null
    } elseif ($eolState -eq "CR_ONLY") {
        $issues.Add([PSCustomObject]@{
            File = $file.FullName
            Issue = "CR_ONLY"
            Detail = "CR-only line endings detected."
        }) | Out-Null
    }
}

if ($issues.Count -gt 0) {
    Write-Host "Encoding/EOL check failed. Issues:" -ForegroundColor Red
    $issues | Format-Table -AutoSize
    exit 1
}

Write-Host "Encoding/EOL check passed for $($files.Count) files under '$targetDir'." -ForegroundColor Green
exit 0

