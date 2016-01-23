# vxl/vpgl/xio/CMakeLists.txt

set(vpgl_xio_sources
  vpgl_xio_lvcs.cxx   vpgl_xio_lvcs.h
)

add_library(vpgl_xio ${vpgl_xio_sources} )
target_link_libraries( vpgl_xio vpgl vsl )

install_targets(/lib vpgl_xio)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vpgl/xio ${vpgl_xio_sources})

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
