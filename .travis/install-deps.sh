#!/bin/sh
set -e

INSTALL_DIR=$HOME/deps
export PATH=$INSTALL_DIR/bin:$PATH

# check if directory is cached
if [ ! -f "$INSTALL_DIR/bin/cmake" ]; then
  cd /tmp
  ## NOTE: We are purposefully using the minimum supported cmake version
  ##       for testing. When cmake_minimum_version is updated in top
  ##       level CMakeLists.txt, this should also be updated.
  wget --no-check-certificate https://cmake.org/files/v2.8/cmake-2.8.9-Linux-i386.sh
  bash cmake-2.8.9-Linux-i386.sh --skip-license --prefix="$INSTALL_DIR/"
else
  echo 'Using cached CMake directory.';
fi
