# contrib/brl/bseg/boxm2/CMakeLists.txt
#SET( boxm2_MULTI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )


#depends on OPENCL being found...
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)

ADD_DEFINITIONS(-DHAS_OPENCL=1)
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} ) 

SET(boxm2_multi_sources
    boxm2_multi_cache.h         boxm2_multi_cache.cxx
    boxm2_multi_cache_group.h   boxm2_multi_cache_group.cxx
    boxm2_multi_util.h          boxm2_multi_util.cxx
   )

ADD_LIBRARY(boxm2_multi  ${boxm2_multi_sources})
TARGET_LINK_LIBRARIES(boxm2_multi boxm2 boxm2_ocl)

#multi gpu implementations
SUBDIRS(algo)

#executables
SUBDIRS(exe)

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(OPENCL_FOUND)
