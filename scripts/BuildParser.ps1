# Generate lexers for RSlang

$bison = 'C:\GnuWin32\win_flex_bison3\win_bison.exe'
$workDir = Resolve-Path -Path "${PSScriptRoot}\..\ccl\rslang\src"
$grammar = 'RSParserImpl.y'

function BuildParser {
  if (-not (Test-Path -Path $bison)) {
    Write-Error "Cannot find bison at ${bison}"
    Exit 1
  }
  Set-Location -Path ${workDir}
  Write-Host "Building ${grammar}"
  & $bison $grammar
  if ($LASTEXITCODE -ne 0) {
    Write-Error 'Grammar generation failed'
    Exit 1
  }
  Exit 0
}

BuildParser
