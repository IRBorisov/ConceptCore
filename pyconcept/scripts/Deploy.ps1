Set-Location $PSScriptRoot\..

$token = Get-Content -Path ./secrets/API_TOKEN.txt
$python = 'python'
& ${python} -m pip install twine
& ${python} -m twine upload ../output/py/pyconcept/*  -u __token__ -p ${token}
