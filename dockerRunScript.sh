#!/bin/bash



VXL_SRC_DIR=/home/vxl/vxl
VXL_SRC_AAA=/home/aleinoff/code/vxl/vxl
VXL_SRC_TRV=/home/travis/build/vxl/vxl
#VXL_BASE_DIR=/home/travis/build/vxl/

#VXL_BUILD_DIR=/home/travis/build/vxl/vxl-build

docker run -v ${VXL_SRC_TRV}:${VXL_SRC_DIR} -it myvxl:m7  /bin/bash -c "export C_COMPILER=$(which gcc); export CXX_COMPILER=$(which g++); bash " #dockerRunCommand.sh " #-c " bash dockerRunCommand.sh "
