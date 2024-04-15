#
# Build script for linux distro
#
set -e # Exit immediately if a command exits with a non-zero status.
set -u # Treat unset variables as an error when substituting.

# Set-Location $PSScriptRoot

# & conan profile detect --force
# & conan install .

# & cmake --preset conan-default
# # & cmake --preset conan-default -DCC_BuildTests=False
# & cmake --build --preset conan-release
# & ctest --test-dir build --output-on-failure
# & cmake --install build --prefix ../output

echo 'Done'
exit 0



# packageName='pyconcept'
# output='../../output/py'
# pythonEnv='venv/bin/python3'

# python3.12 -m venv venv
# $pythonEnv -m pip install -r requirements-build.txt

# mkdir -p import
# cp -r ../../output/include import/
# cp -r ../../output/lib import/

# $pythonEnv -m build --no-isolation --wheel --outdir=$output/$packageName

# wheel=$(find $output/$packageName -name '*.whl')

# $pythonEnv -m pip uninstall -y $packageName
# $pythonEnv -m pip install $wheel
# $pythonEnv -m unittest

# rm -rf venv




# COPY cclCommons /home/cclCommons
# WORKDIR /home/cclCommons/build
# RUN cmake ../ && \
#     cmake --build . && \
#     ctest --output-on-failure && \
#     cmake --install . --prefix /home/output
# WORKDIR /home

# COPY rslang /home/rslang
# WORKDIR /home/rslang/build
# RUN cmake ../ && \
#     cmake --build . && \
#     ctest --output-on-failure && \
#     cmake --install . --prefix /home/output
# WORKDIR /home

# COPY cclLang /home/cclLang
# WORKDIR /home/cclLang/build
# RUN cmake ../ && \
#     cmake --build . && \
#     ctest --output-on-failure && \
#     cmake --install . --prefix /home/output
# WORKDIR /home

# COPY cclGraph /home/cclGraph
# WORKDIR /home/cclGraph/build
# RUN cmake ../ && \
#     cmake --build . && \
#     ctest --output-on-failure && \
#     cmake --install . --prefix /home/output
# WORKDIR /home

# COPY core /home/core
# WORKDIR /home/core/build
# RUN cmake ../ && \
#     cmake --build . && \
#     ctest --output-on-failure && \
#     cmake --install . --prefix /home/output
# WORKDIR /home

# # Build python library
# COPY examples/pyconcept /home/examples/pyconcept
# WORKDIR  /home/examples/pyconcept
# RUN script/Build.sh
# WORKDIR /home
