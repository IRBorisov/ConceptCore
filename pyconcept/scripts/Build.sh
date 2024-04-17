# Build script for Linux
set -e

packageName='pyconcept'
output='../output/py'
pythonEnv='venv/bin/python3'

# Setup python env
python3.12 -m venv venv
${pythonEnv} -m pip install -r requirements-build.txt

# Import sources from ccl
mkdir -p ccl
cd ../ccl
cp -r `ls -A | grep -v 'build'` "../pyconcept/ccl"
cd ../pyconcept

# Build pyconcept
${pythonEnv} -m build --no-isolation --wheel --outdir=${output}/${packageName}

wheel=$(find ${output}/${packageName} -name '*.whl')

# Test pyconcept
${pythonEnv} -m pip uninstall -y ${packageName}
${pythonEnv} -m pip install ${wheel}
${pythonEnv} -m unittest

# rm -rf venv
# rm -rf build

exit 0