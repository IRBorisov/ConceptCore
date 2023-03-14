Set-Location $PSScriptRoot\..

$packageName = 'pyconcept'
$output = "$PSScriptRoot\..\..\..\output\py\x64"
$python = '.\venv\Scripts\python.exe'

if (-not (Test-Path -Path $python -PathType Leaf)) {
    & 'python' -m venv .\venv
    & $python -m pip install --upgrade pip
    & $python -m pip install -r requirements.txt
    & $python -m pip install -r requirements-build.txt
}

if (Test-Path -Path $output\$packageName) {
    Remove-Item $output\$packageName -Recurse -Force
}
if (Test-Path -Path 'import') {
    Remove-Item 'import' -Recurse -Force
}

Copy-Item -Path "..\..\output\include" -Destination "import\include" -Recurse
Copy-Item -Path "..\..\output\lib\x64" -Destination "import\lib" -Recurse

# py setup.py bdist_wheel
& $python -m build --no-isolation --wheel --outdir=$output\$packageName
$wheel = Get-Childitem -Path $output\$packageName\*.whl -Name
if (-not $wheel) {
    Write-Error "No wheel generated for $packageName"
    Exit 1
}

& $python -m pip uninstall -y $packageName
& $python -m pip install -I $output\$packageName\$wheel
& $python -m unittest
Exit $LASTEXITCODE