# mul/msm/CMakeLists.txt
#
#   Tim Cootes
#

doxygen_add_library(contrib/mul/msm_utils
  DEPENDS contrib/mul/msm
  PACKAGE contrib/mul
  DESCRIPTION "Utils for Manchester's SM Library"
)

set(msm_utils_sources
  msm_draw_shape_to_eps.h       msm_draw_shape_to_eps.cxx
  msm_shape_mode_view.h         msm_shape_mode_view.cxx
  msm_make_grid.h               msm_make_grid.cxx
)

# aux_source_directory(Templates msm_utils_sources)
vxl_add_library(LIBRARY_NAME msm_utils LIBRARY_SOURCES ${msm_utils_sources} )
target_link_libraries(msm_utils msm ${VXL_LIB_PREFIX}vgl)

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()
