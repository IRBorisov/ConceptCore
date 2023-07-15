# ConceptCore
C++ library for manipulating concepts in formal language of advanced set theory

Use Dockerfile to setup Clang / GCC build for Ubuntu developement.
Use VS Solution for Windows development.

Windows development requires Bison installed and Visual Studio 2022+


# This project was made possible because of these incredible projects and tools
This project is based on multiple projects and works listed below. If you notice any problems with licensing or missing acknowledgements please inform repository maintainer.

- [Re-flex](https://github.com/Genivia/RE-flex) provides clear way to generate lexical analysers for ASCII and Unicode syntax variations
- [Bison](https://www.gnu.org/software/bison/) is used to generate language parser
- [nlohmann-json](https://github.com/nlohmann/json) is embedded as C++ JSON parser for conceptual schema persistence and high level JSON strings API
- [Clang-tidy](https://clang.llvm.org/extra/clang-tidy/) along with Visual Studio analyser are used for C++ static code analysis
- [pybind11](https://github.com/pybind/pybind11) is embedded in pyconcept library example to generate Python module wrapper for C++ integration
- [CMake](https://cmake.org/) provides C++ projects build toolchain
- [conan](https://conan.io/) and NuGet are used to manage package dependencies
- Docker container is used to provide consistent build environment for Linux builds
- Microsoft Visual Studio 2022 and Visual Studio Code are used as IDE's and build environments for Windows builds
