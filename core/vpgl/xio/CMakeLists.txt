# vxl/vpgl/xio/CMakeLists.txt

set(vpgl_xio_sources
  vpgl_xio_lvcs.cxx   vpgl_xio_lvcs.h
)

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vpgl_xio LIBRARY_SOURCES ${vpgl_xio_sources} )
target_link_libraries( ${VXL_LIB_PREFIX}vpgl_xio ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vsl )

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
