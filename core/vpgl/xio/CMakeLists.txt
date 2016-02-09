# vxl/vpgl/xio/CMakeLists.txt

set(vpgl_xio_sources
  vpgl_xio_lvcs.cxx   vpgl_xio_lvcs.h
)

vxl_add_library(LIBNAME vpgl_xio LIBSRCS ${vpgl_xio_sources} )
target_link_libraries( vpgl_xio vpgl vsl )

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
