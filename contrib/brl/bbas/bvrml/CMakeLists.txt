# brl/bbas/bvrml/CMakeLists.txt
# VRML utilities for Brown University Libraries

set(bvrml_sources
  bvrml_export.h bvrml_write.h bvrml_write.cxx
)

vxl_add_library( LIBNAME bvrml LIBSRCS  ${bvrml_sources})

target_link_libraries(bvrml ${VXL_LIB_PREFIX}vgl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

add_subdirectory(pro)

