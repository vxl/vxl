# contrib/brl/bseg/boxm2/util/CMakeLists.txt
include_directories( ${GEL_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(boxm2_class_sources
    boxm2_class_generate_samples.h    boxm2_class_generate_samples.cxx
   )

add_library(boxm2_class  ${boxm2_class_sources})
target_link_libraries(boxm2_class boxm2 bvgl vpgl vnl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl)

#install the .h .txx and libs
install_targets(/lib boxm2_class)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2_class/ ${boxm2_class_sources})

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

add_subdirectory(exe)
