# Build single project
# TODO: setup project folder as parameter
Set-Location $PSScriptRoot

& conan profile detect --force
& conan install .

& cmake --preset conan-default
& cmake --build --preset conan-release
& ctest --test-dir build --output-on-failure
& cmake --install build --prefix ../output