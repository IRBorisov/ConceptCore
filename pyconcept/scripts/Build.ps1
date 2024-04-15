Set-Location $PSScriptRoot\..

$packageName = 'pyconcept'
$output = "${PSScriptRoot}\..\..\..\output\py\x64\${packageName}"
$python = '.\venv\Scripts\python.exe'
$conan = 'conan'

function Build {
    PrepareEnv
    PrepareImports
    PrepareOutput
    $wheel = BuildWheel
    if (-not (${wheel}) -or $LASTEXITCODE -ne 0) {
        Write-Error "No wheel generated for ${packageName}"
        Exit 1
    }
    TestWheel("${output}\${wheel}")
    Exit $LASTEXITCODE
}

function PrepareEnv {
    Write-Host "Setting up environment for ${python}" -ForegroundColor DarkGreen
    if (-not (Test-Path -Path ${python} -PathType Leaf)) {
        & 'python' -m venv .\venv
        & $python -m pip install --upgrade pip
        & $python -m pip install -r requirements-build.txt
    }
}

function PrepareImports {
    Write-Host 'Copy imports' -ForegroundColor DarkGreen
    if (Test-Path -Path 'import') {
        Remove-Item 'import' -Recurse -Force
    }
    Copy-Item -Path "..\..\output\include" -Destination "import\include" -Recurse
    Copy-Item -Path "..\..\output\lib\x64" -Destination "import\lib" -Recurse
}

function PrepareOutput {
    if (Test-Path -Path ${output}) {
        Remove-Item ${output} -Recurse -Force
    }
}

function BuildWheel {
    Write-Host 'Building project...' -ForegroundColor DarkGreen
    & ${conan} profile detect --force
    & ${conan} install .
    & $python -m build --no-isolation --wheel --outdir="${output}"
    return Get-Childitem -Path "${output}\*.whl" -Name
}

function TestWheel([string] $wheelPath) {
    Write-Host 'Running tests...' -ForegroundColor DarkGreen
    & ${python} -m pip uninstall -y ${packageName}
    & ${python} -m pip install -I ${wheelPath}
    & ${python} -m unittest
}

Build