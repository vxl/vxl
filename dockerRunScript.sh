#!/bin/bash

VXL_SRC_DIR=/usr/src/vxl
VXL_BUILD_DIR=/usr/src/vxl-build

DOCKER_RUN_COMMAND=(
  docker run
    --rm
    vxl/travisMarch7_2016
    /bin/bash -c "
      echo '$HOME';
      git clone http://github.com/vxl/vxl.git;
      cd vxl-build;
      cmake ../vxl -DCMAKE_CXX_STANDARD=98;
      ctest -D ExperimentalStart
      ctest -D ExperimetnalConfigure
      ctest -D ExperimentalBuild -j2
      ctest -D ExperimentalTest --schedule-random -j2
      ctest -D ExperimentalSubmit
      make install
   "
)

${DOCKER_RUN_COMMAND[@]}
      
