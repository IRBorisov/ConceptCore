# pyconcept Build script
# * using auditwheel to create standardazied packages
set -e # Exit immediately if a command exits with a non-zero status.
set -u # Treat unset variables as an error when substituting.

export CMAKE_BUILD_TYPE="Release" 

packageName='pyconcept'
output='../output/py'
python='python3.12'

# Setup python dependencies
${python} -m pip install -r requirements-build.txt

# Import sources from ccl
mkdir -p ccl
cd ../ccl
cp -r `ls -A | grep -v 'build'` "../pyconcept/ccl"
cd ../pyconcept

# Build pyconcept
rm -rf ${output}/${packageName}
# ${python} -m build --no-isolation --wheel --outdir=${output}/${packageName}
${python} -m pip wheel . -w "${output}/${packageName}"
wheel=$(find ${output}/${packageName} -name '*.whl')

auditwheel \
  repair ${wheel} \
  --plat "${TARGET_PLATFORM}" \
  -w "${output}/${packageName}"

rm ${wheel}
wheel=$(find ${output}/${packageName} -name '*.whl')

# Test pyconcept
${python} -m pip uninstall -y ${packageName}
${python} -m pip install ${wheel}
${python} -m unittest

# rm -rf build

exit 0