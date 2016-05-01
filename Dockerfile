FROM debian:8
MAINTAINER Alexander Leinoff <alexander-leinoff@uiowa.edu>

RUN apt-get update && apt-get install -y \
  build-essential \
  clang \
  curl \
  git \
  libncurses-dev \
  vim \
  zlib1g-dev


# Install CMake 2.8.9
WORKDIR /tmp/
RUN curl -O https://cmake.org/files/v2.8/cmake-2.8.9.tar.gz && \
    tar xvzf cmake-2.8.9.tar.gz && \
    mkdir /tmp/cmake-build && \
    cd /tmp/cmake-build && \
    ../cmake-2.8.9/bootstrap && \
    make -j$(nproc) && \
    ./bin/cmake -DCMAKE_BUILD_TYPE:STRING=Release . && \
    make -j$(nproc) && \
    make install && \
    cd /tmp && \
    rm -rf cmake*


# Normal user
RUN useradd -m vxl


USER vxl

RUN mkdir -p /home/vxl/vxl
RUN mkdir -p /home/vxl/build

WORKDIR /home/vxl/vxl

CMD /bin/bash
