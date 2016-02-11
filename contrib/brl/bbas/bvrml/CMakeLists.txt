# brl/bbas/bvrml/CMakeLists.txt
# VRML utilities for Brown University Libraries

set(bvrml_sources
  bvrml_export.h bvrml_write.h bvrml_write.cxx
)

add_library(bvrml  ${bvrml_sources})

target_link_libraries(bvrml ${VXL_LIB_PREFIX}vgl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

add_subdirectory(pro)

install_targets(/lib bvrml)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bvrml ${bvrml_sources})
