# This is bbas/bmsh3d/pro/CMakeLists.txt
# Brown Mesh Processing library
include_directories( ${BRL_INCLUDE_DIR} )

set( bmsh3d_pro_sources
    bmsh3d_cmdpara.cxx         bmsh3d_cmdpara.h
)

vxl_add_library(LIBRARY_NAME bmsh3d_pro LIBRARY_SOURCES ${bmsh3d_pro_sources} )
target_link_libraries( bmsh3d_pro bmsh3d ${VXL_LIB_PREFIX}vul )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
