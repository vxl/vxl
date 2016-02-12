# contrib/brl/bseg/boxm2/util/CMakeLists.txt
include_directories( ${GEL_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(boxm2_class_sources
    boxm2_class_generate_samples.h    boxm2_class_generate_samples.cxx
   )

add_library(boxm2_class  ${boxm2_class_sources})
target_link_libraries(boxm2_class boxm2 bvgl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl)

#install the .h .txx and libs
install_targets(/lib boxm2_class)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2_class/ ${boxm2_class_sources})

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

add_subdirectory(exe)
