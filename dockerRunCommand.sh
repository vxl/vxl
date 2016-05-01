#!/bin/bash

set -ev

GIT_SRC_DIR=/home/vxl/vxl

BUILD_DIR=/home/vxl/build
CLANG_BUILD_DIR=/home/travis/build/vxl/vxl-build-clang

which cmake;
cmake --version;

# pull requests are typically made by branch name
GIT_BRANCH=$(git rev-parse --abbrev-ref HEAD);
SITE_NAME=docker_${GIT_BRANCH}_${C_COMPILER};

echo ${C_COMPILER};

#mkdir -p ${GCC_BUILD_DIR}
echo pwd;
echo "ehllo";
cd ${BUILD_DIR};
pwd
echo $HOME

cmake ${GIT_SRC_DIR} \
  -DCMAKE_CXX_STANDARD=98 \
  -DSITE=${SITE_NAME} \
  -DCMAKE_C_COMPILER=${C_COMPILER} \
  -DCMAKE_CXX_COMPILER=${CXX_COMPILER}

ctest -D ExperimentalStart
ctest -D ExperimentalConfigure
ctest -D ExperimentalBuild -j2
#make -j12 -k
ctest -D ExperimentalTest -j2
ctest -D ExperimentalSubmit


