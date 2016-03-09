# contrib/brl/bseg/boxm2/util/CMakeLists.txt
include_directories( ${GEL_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(boxm2_class_sources
    boxm2_class_generate_samples.h    boxm2_class_generate_samples.cxx
   )

vxl_add_library(LIBRARY_NAME boxm2_class LIBRARY_SOURCES  ${boxm2_class_sources})
target_link_libraries(boxm2_class boxm2 bvgl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl)

#install the .h .hxx and libs

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

add_subdirectory(exe)
