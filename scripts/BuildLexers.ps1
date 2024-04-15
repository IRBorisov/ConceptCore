# Generate lexers for RSlang

$workDir = Resolve-Path -Path "${PSScriptRoot}\..\ccl\rslang\src"

function BuildLexers {
  Set-Location -Path ${workDir}
  BuildSyntax('AsciiLexer')
  BuildSyntax('MathLexer')
  Write-Host 'Building lexers complete'
  Exit 0
}

function BuildSyntax([string] $lexerName) {
  $lexer = "${lexerName}Impl"
  Write-Host "Building ${lexer}"
  & reflex "${lexer}.l"
  if ($LASTEXITCODE -ne 0) {
    Write-Error 'Please add path to reflex.exe to PATH variable'
    Exit 1
  }
  Move-Item -Path "${lexer}.hpp" -Destination "..\header\${lexer}.hpp" -Force
}

BuildLexers
