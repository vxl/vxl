#depends on OPENCL being found...
  INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
  IF(OPENCL_FOUND)

INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(boxm2_ocl_sources
    boxm2_opencl_processor.h                boxm2_opencl_processor.cxx
    boxm2_opencl_cache.h                    boxm2_opencl_cache.cxx
   )

AUX_SOURCE_DIRECTORY(Templates boxm2_ocl_sources)

ADD_LIBRARY(boxm2_ocl  ${boxm2_ocl_sources})
TARGET_LINK_LIBRARIES(boxm2_ocl boxm2 boxm2_basic boxm2_io bocl boct brdb vbl_io expatpp vpgl vgl vgl_xio vbl vsl vcl)

#pro
SUBDIRS(pro)


#exe
SUBDIRS(exe)
ENDIF(OPENCL_FOUND)