#bstm/ocl source dir

#depends on OPENCL being found...
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)

MESSAGE("BSTM FOUND OPENCL")

ADD_DEFINITIONS(-DHAS_OPENCL=1)
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(bstm_ocl_sources
    bstm_ocl_util.h         bstm_ocl_util.cxx
    bstm_opencl_cache.h     bstm_opencl_cache.cxx
    bstm_scene_info.h
   )

AUX_SOURCE_DIRECTORY(Templates bstm_ocl_sources)

ADD_LIBRARY(bstm_ocl ${bstm_ocl_sources})
TARGET_LINK_LIBRARIES(bstm_ocl bstm bstm_io bocl vpgl vil vcl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

#pro
SUBDIRS(pro)

#algo
SUBDIRS(algo)

#exe
SUBDIRS(exe)

ENDIF(OPENCL_FOUND)
