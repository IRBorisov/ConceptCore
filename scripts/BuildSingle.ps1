# Build single project
# Copy file to specific project and run the script
Set-Location $PSScriptRoot

& conan profile detect --force
& conan install . --build=missing

& cmake --preset conan-default
# & cmake --preset conan-default -DCC_BuildTests=False
& cmake --build --preset conan-release
& ctest --test-dir build --output-on-failure
# & cmake --install build --prefix ../output
& cmake --install build --prefix ../../output