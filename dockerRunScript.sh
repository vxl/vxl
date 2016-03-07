#!/bin/bash



VXL_SRC_DIR=/home/travis/build/vxl/vxl
VXL_BASE_DIR=/home/travis/build/vxl/

VXL_BUILD_DIR=/home/travis/build/vxl/vxl-build

DOCKER_RUN_COMMAND=(
  pwd;
  ls;
  docker run
#    --rm
    -v ${VXL_SRC_DIR}:${VXL_SRC_DIR}:Z
    myvxl:m7
    /bin/bash -c "
      pwd;
      ls;
      whoami;
      uname -a;
      cd ${VXL_BASE_DIR};
      pwd;
      ls;
      ls -la vxl;
#      ls -R;
      echo 'hello';
      cmake --version
#     git clone http://github.com/vxl/vxl.git;
      mkdir -p vxl-build
      ls;
      cd vxl-build;
      cmake ../vxl;
      echo "i am here now";
      ctest -D ExperimentalStart
      ctest -D ExperimetnalConfigure
      ctest -D ExperimentalBuild -j2
      ctest -D ExperimentalTest --schedule-random -j2
      ctest -D ExperimentalSubmit
#     make install
   "
)

"${DOCKER_RUN_COMMAND[@]}"
      
