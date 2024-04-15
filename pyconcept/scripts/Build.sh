# Build script for linux distro
set -e 

packageName='pyconcept'
output='../../output/py'
pythonEnv='venv/bin/python3'

python3.12 -m venv venv
$pythonEnv -m pip install -r requirements-build.txt

mkdir -p import
cp -r ../../output/include import/
cp -r ../../output/lib import/

$pythonEnv -m build --no-isolation --wheel --outdir=$output/$packageName

wheel=$(find $output/$packageName -name '*.whl')

$pythonEnv -m pip uninstall -y $packageName
$pythonEnv -m pip install $wheel
$pythonEnv -m unittest

rm -rf venv

exit 0