# This is contrib/brl/bbas/bsta/vis/CMakeLists.txt
# The Brown Statistics Algorithms Library Visualization Tools

include_directories( ${BRL_INCLUDE_DIR}/bbas )

set( bsta_vis_sources
  bsta_svg_tools.h bsta_svg_tools.cxx
)

vxl_add_library(LIBRARY_NAME bsta_vis LIBRARY_SOURCES  ${bsta_vis_sources} )

target_link_libraries( bsta_vis bsta bsvg ${VXL_LIB_PREFIX}vnl )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
