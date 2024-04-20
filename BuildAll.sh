#
# Build script for linux distro
#
set -e # Exit immediately if a command exits with a non-zero status.
set -u # Treat unset variables as an error when substituting.

export CMAKE_BUILD_TYPE="Release" 

# Build CCL
cd /home/ccl
conan profile detect --force
conan install . --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
ctest --test-dir build/Release --output-on-failure
cmake --install build/Release --prefix /home/output

# Build pyconcept
cd /home/pyconcept
sh scripts/Build.sh

echo 'Done'
exit 0
