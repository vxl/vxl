#!/bin/bash

VXL_SRC_DIR=/home/travis/build/vxl/vxl
VXL_BASE_DIR=/home/travis/build/vxl/

VXL_BUILD_DIR=/home/travis/build/vxl/vxl-build

DOCKER_RUN_COMMAND=(
  docker run
#    --rm
    -v ${VXL_BASE_DIR}:${VXL_BASE_DIR}
    myvxl:m7
    /bin/bash -c "
      pwd;
      ls;
      whoami;
      uname -a;
      cd;
      pwd;
      ls;
      ls -R;
      echo 'hello';
      cmake --version
#     git clone http://github.com/vxl/vxl.git;
      cd vxl-build;
      cmake ../vxl -DCMAKE_CXX_STANDARD=98;
      ctest -D ExperimentalStart
      ctest -D ExperimetnalConfigure
      ctest -D ExperimentalBuild -j2
      ctest -D ExperimentalTest --schedule-random -j2
      ctest -D ExperimentalSubmit
#     make install
   "
)

"${DOCKER_RUN_COMMAND[@]}"
      
