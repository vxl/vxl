#boxm2/ocl source dir
SET(BOXM2_OPENCL_SRC_DIR CACHE STRING "Location of OpenCL source files for brl/bseg/boxm2/ (defaults to source tree)" )

# Set up boxm2_ocl_where_root_dir.h to have a macro set to $BOXM2_OPENCL_SRC_DIR
CONFIGURE_FILE(
  boxm2_ocl_where_root_dir.h.in
  ${vxl_BINARY_DIR}/contrib/brl/bseg/boxm2/ocl/boxm2_ocl_where_root_dir.h ESCAPE_QUOTES
)
INCLUDE_DIRECTORIES( ${vxl_BINARY_DIR}/contrib/brl/bseg/boxm2/ocl/ )


#depends on OPENCL being found...
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)

ADD_DEFINITIONS(-DHAS_OPENCL=1)
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(boxm2_ocl_sources
    boxm2_opencl_cache.h     boxm2_opencl_cache.cxx
    boxm2_opencl_cache2.h     boxm2_opencl_cache2.cxx
    boxm2_ocl_util.h         boxm2_ocl_util.cxx
   )

AUX_SOURCE_DIRECTORY(Templates boxm2_ocl_sources)

ADD_LIBRARY(boxm2_ocl ${boxm2_ocl_sources})
TARGET_LINK_LIBRARIES(boxm2_ocl boxm2 boxm2_basic boxm2_io bocl boct bsta brdb expatpp vpgl vgl_xio vgl vil vnl_algo vnl vbl_io vbl vsl vcl)

#install the .h .txx and libs
INSTALL_TARGETS(/lib boxm2_ocl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/ocl ${boxm2_ocl_sources})

#pro
SUBDIRS(pro)

#exe
SUBDIRS(exe)

#algo
SUBDIRS(algo)

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(OPENCL_FOUND)
