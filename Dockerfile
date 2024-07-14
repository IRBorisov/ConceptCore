# ubunutu is the base image
FROM quay.io/pypa/manylinux_2_28_x86_64 as builder
LABEL version="1.0"
LABEL maintainer="IRBorisov iborisov@acconcept.ru"
LABEL description="Linux build environment"

ARG DEBIAN_FRONTEND=noninteractive

# Install standard packages
RUN dnf update -y && \
    dnf upgrade -y && \
    dnf install -y \
        nano \
        make \
        wget \
        curl \
        tar && \
    dnf update -y && \
    dnf clean all

# GCC compiler
ARG GCC_VER="13"
RUN dnf install -y \
        gcc-toolset-${GCC_VER} \
        gcc-toolset-${GCC_VER}-gcc-c++ && \
    dnf remove -y gcc-toolset-12-* && \
    dnf clean all

ENV CC="/opt/rh/gcc-toolset-${GCC_VER}/root/bin/gcc"
ENV CXX="/opt/rh/gcc-toolset-${GCC_VER}/root/bin/g++"
ENV PATH="/opt/python/cp312-cp312/bin:/opt/rh/gcc-toolset-13/root/bin:${PATH}"

# Install build tools
RUN python3.12 -m pip install \ 
        auditwheel \
        wheel \
        build \
        ninja \
        cmake \
        conan

# ====================== END OF CPP-Python environment ==================

FROM builder as CCL

ENV TARGET_PLATFORM='manylinux_2_28_x86_64'

WORKDIR /home

COPY . /home

RUN sed -i 's/\r$//g' /home/BuildAll.sh && \
    chmod +x /home/BuildAll.sh
