Set-Location $PSScriptRoot\..

$packageName = 'pyconcept'
$output = "${PSScriptRoot}\..\..\output\py\${packageName}"
$python = '.\venv\Scripts\python.exe'

$ccl_source = "${PSScriptRoot}\..\..\ccl"
$ccl_destination = "${PSScriptRoot}\..\ccl"
$ccl_include = ('*.cpp','*.hpp','*.h')

function Build {
    PrepareEnv
    PrepareImports
    PrepareOutput
    BuildProject
    $wheel = Get-Childitem -Path "${output}\*.whl" -Name
    if (-not (${wheel}) -or $LASTEXITCODE -ne 0) {
        Write-Error "No wheel generated for ${packageName}"
        Exit 1
    }
    TestWheel("${output}\${wheel}")
    Exit $LASTEXITCODE
}

function PrepareEnv {
    Write-Host "Set up environment for ${python}" -ForegroundColor DarkGreen
    if (-not (Test-Path -Path ${python} -PathType Leaf)) {
        & 'python' -m venv .\venv
        & $python -m pip install --upgrade pip
        & $python -m pip install -r requirements-dev.txt
    }
}

function PrepareImports {
    Write-Host 'Copy imports' -ForegroundColor DarkGreen
    if (Test-Path -Path ${ccl_destination}) {
        Remove-Item ${ccl_destination} -Recurse -Force
    }
    New-Item -Path ${ccl_destination} -ItemType Directory | Out-Null
    $source = Resolve-Path(${ccl_source})
    $destination = Resolve-Path(${ccl_destination})
    Get-ChildItem ${source} -Recurse -Include ${ccl_include} `
        | Where-Object { `
            $_.FullName -CNotLike '*build*' -And `
            $_.FullName -CNotLike '*test*' `
        } `
        | ForEach-Object -Begin{} -End{} -Process{
            $destinationFile = Join-Path ${destination}.Path $_.FullName.Substring(${source}.Path.length)
            New-Item -ItemType File -Path ${destinationFile} -Force | Out-Null
            Copy-Item -Path $_.FullName -Destination ${destinationFile} -Force
        }

    Copy-Item -Path "${source}/CMakeLists.txt" -Destination ${destination}
    Copy-Item -Path "${source}/conanfile.txt" -Destination ${destination}
    Copy-Item -Path "${source}/cmake" -Destination ${destination} -Recurse
}

function PrepareOutput {
    Write-Host "Clear output at ${output}" -ForegroundColor DarkGreen
    if (Test-Path -Path ${output}) {
        Remove-Item ${output} -Recurse -Force
    }
}

function BuildProject {
    Write-Host 'Building project...' -ForegroundColor DarkGreen
    & $python -m build --wheel --no-isolation --outdir="${output}"
    & $python -m build --sdist --no-isolation --outdir="${output}"
}

function TestWheel([string] $wheelPath) {
    Write-Host 'Running tests...' -ForegroundColor DarkGreen
    & ${python} -m pip uninstall -y ${packageName}
    & ${python} -m pip install -I ${wheelPath}
    & ${python} -m unittest
}

Build