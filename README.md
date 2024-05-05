# 🏛️ ConceptCore

[![PyPI pyconcept](https://badge.fury.io/py/pyconcept.svg)](https://badge.fury.io/py/pyconcept)

C++ library for manipulating concepts in formal language of advanced set theory

## 🌲 Structure

- ccl - ConceptCoreLibrary static C++ library
- coredll - ConceptCoreDLL binary for Windows embedding
- pyconcept - PyPi packaged Python wrapper for some core RSLanguage and parsing functions

## 📦 Project build

- ccl + coredll + pyconcept - Dockerfile to setup Clang / GCC build for Ubuntu development (see BuildAll.sh).
- ccl + coredll local dev - Visual Studio Solution for Windows development - 'CCL_full.sln'.
- ccl - VS Code and ccl/CMakeLists.txt for other platforms.
- pyconcept - python setup.py OR pyconcept/scripts

After changing grammar / syntax you can rebuild lexers and parser using 'scripts' folder.

- parser requires [Bison](https://www.gnu.org/software/bison/) installed on through chocolatey: 'choco install winflexbison3'
- lexer requires [Re-flex](https://github.com/Genivia/RE-flex)

To run terminal after building docker container use command

```
docker run --rm -it --entrypoint /bin/bash concept-core-builder

```

## 💝 Acknowledgements

This project is based on multiple projects and works listed below. If you notice any problems with licensing or missing acknowledgements please inform repository maintainer.

- [Re-flex](https://github.com/Genivia/RE-flex) provides clear way to generate lexical analyzers for ASCII and Unicode syntax variations
- [Bison](https://www.gnu.org/software/bison/) is used to generate language parser
- [nlohmann-json](https://github.com/nlohmann/json) is embedded as C++ JSON parser for conceptual schema persistence and high level JSON strings API
- [Clang-tidy](https://clang.llvm.org/extra/clang-tidy/) along with Visual Studio analyzer are used for C++ static code analysis
- [pybind11](https://github.com/pybind/pybind11) is used to generate Python module wrapper for C++ integration
- [CMake](https://cmake.org/) provides C++ projects build toolchain
- [conan](https://conan.io/) and NuGet are used to manage package dependencies
- Docker container is used to provide consistent build environment for Linux builds
- Microsoft Visual Studio 2022 and Visual Studio Code are used as IDE's and build environments for Windows builds
