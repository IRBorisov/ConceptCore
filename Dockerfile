# ubunutu is the base image
FROM ubuntu:jammy as cpp-builder
LABEL version="1.0"
LABEL maintainer="IRBorisov iborisov@acconcept.ru"
LABEL description="Linux build environment"

ARG LINUX_FLAVOR=ubuntu
ARG LINUX_DISTR=jammy
ARG  DEBIAN_FRONTEND=noninteractive

# Install standard packages
RUN apt-get update -qq && \
    apt-get full-upgrade -y && \
    apt-get install -y --no-install-recommends \
        nano \
        wget \
        curl \
        tar \
        unzip \
        git \
        software-properties-common \
        build-essential \
        gpg-agent && \
    rm -rf /var/lib/apt/lists/*

# Install Python and Conan
RUN add-apt-repository -y ppa:deadsnakes/ppa && \
    apt-get update -qq && \
    apt-get install -y --no-install-recommends \
        python3.12 \
        python3.12-venv \
        python3.12-dev && \
    curl -sS https://bootstrap.pypa.io/get-pip.py | python3.12 && \
    python3.12 -m pip install --upgrade pip setuptools && \
    python3.12 -m pip install conan && \
    rm -rf /var/lib/apt/lists/*

# Add GCC compiler
ARG GCC_VER="13"
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test && \
    apt-get update -qq && \
    apt-get install -y --no-install-recommends \
        gcc-${GCC_VER} \
        g++-${GCC_VER} && \
    update-alternatives --install /usr/bin/gcc gcc $(which gcc-${GCC_VER}) 100 && \
    update-alternatives --install /usr/bin/g++ g++ $(which g++-${GCC_VER}) 100 && \
    rm -rf /var/lib/apt/lists/*

# Add Clang compiler
ARG LLVM_VER="18"
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - 2>/dev/null
RUN add-apt-repository -y "deb http://apt.llvm.org/${LINUX_DISTR}/ llvm-toolchain-${LINUX_DISTR}-${LLVM_VER} main" && \
    apt-get update -qq && \
    apt-get install -y --no-install-recommends \
        clang-${LLVM_VER} \
        clang-tools-${LLVM_VER} \
        clangd-${LLVM_VER} \
        lld-${LLVM_VER} \
        lldb-${LLVM_VER} \
        llvm-${LLVM_VER} \
        llvm-${LLVM_VER}-dev \
        llvm-${LLVM_VER}-runtime \
        libc++-${LLVM_VER}-dev \
        libc++abi-${LLVM_VER}-dev \
        libclang-${LLVM_VER}-dev \
        libclang-common-${LLVM_VER}-dev \
        libfuzzer-${LLVM_VER}-dev \
        clang-tidy-${LLVM_VER} && \
    update-alternatives --install /usr/bin/clang clang $(which clang-${LLVM_VER}) 100 && \
    update-alternatives --install /usr/bin/clang++ clang++ $(which clang++-${LLVM_VER}) 100 && \
    update-alternatives --install /usr/bin/clang-tidy clang-tidy $(which clang-tidy-${LLVM_VER}) 1 && \
    rm -rf /var/lib/apt/lists/*

# Add CMake
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null \
        | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null && \
    apt-add-repository "deb https://apt.kitware.com/${LINUX_FLAVOR}/ ${LINUX_DISTR} main" && \
    apt-get update -qq && \
    apt-get install -y --no-install-recommends \
        cmake && \
    rm -rf /var/lib/apt/lists/*

# Cleanup cached apt data we don't need anymore
RUN apt-get autoclean -y && \
    apt-get autoremove -y && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# ====================== END OF CPP-Python environment ==================

FROM cpp-builder as CCL
ARG CMAKE_BUILD_TYPE="Release"

# Choose between clang/clang++ or gcc/g++
ENV CC="gcc"
ENV CXX="g++"

ENV BUILD_HOME=/home
WORKDIR $BUILD_HOME

COPY . /home

ENTRYPOINT ["sh", "/home/entrypoint.sh"]