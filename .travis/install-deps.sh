#!/bin/sh
set -e

REQUIRED_CMAKE_VERSION="2.8.9"
export INSTALL_DIR=${HOME}/deps
export PATH=$INSTALL_DIR/bin:$PATH

if [ ! -f "${INSTALL_DIR}" ]; then
  mkdir -p "${INSTALL_DIR}"
fi

INSTALL_NEW_CMAKE=1
if [ -f $INSTALL_DIR/bin/cmake ] ; then
  CMAKE_VERSION=$(cmake --version |awk '{print $3}')
  if [ "${CMAKE_VERSION}" == "${REQUIRED_CMAKE_VERSION}" ]; then
    INSTALL_NEW_CMAKE=0 # ONLY if exists, and is exactly 2.8.9
  fi
fi

# check if directory is cached
if [ ${INSTALL_NEW_CMAKE} -ne 0 ]; then
  cd /tmp
  rm -rf ${INSTALL_DIR}/deps/* # remove wrong versions of cmake
  ## NOTE: We are purposefully using the minimum supported cmake version
  ##       for testing. When cmake_minimum_version is updated in top
  ##       level CMakeLists.txt, this should also be updated.
  ## This is 32bit and does not work: wget --no-check-certificate https://cmake.org/files/v2.8/cmake-2.8.9-Linux-i386.sh
  ## Needed to build custom version of cmake for ubuntu 12.04 at 64 bit specifically for dashboards.
  wget --no-check-certificate  http://slicer.kitware.com/midas3/download/bitstream/507778/cmake-2.8.9-Linux-x86_64.sh
  bash cmake-2.8.9-Linux-x86_64.sh --skip-license --prefix=$INSTALL_DIR/
else
  echo "Using cached CMake directory: $(cmake --version)"
fi
