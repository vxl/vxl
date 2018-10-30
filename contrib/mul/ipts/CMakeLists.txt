# mul/ipts/CMakeLists.txt

doxygen_add_library(contrib/mul/ipts
  DEPENDS contrib/mul/vimt contrib/mul/mbl
  PACKAGE contrib/mul
  DESCRIPTION "Interesting Points - Find interesting/salient image points"
  )

set(ipts_sources
    ipts_scale_space_peaks.h
    ipts_draw.h
    ipts_local_entropy.cxx ipts_local_entropy.h
    ipts_corner_pyramid.cxx ipts_corner_pyramid.h
    ipts_entropy_pyramid.cxx ipts_entropy_pyramid.h
    ipts_orientation_pyramid.cxx ipts_orientation_pyramid.h
)

vxl_add_library(LIBRARY_NAME ipts LIBRARY_SOURCES ${ipts_sources})
target_link_libraries(ipts vimt_algo vimt ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil)

if(VXL_BUILD_MUL_TOOLS)
  add_subdirectory(tools)
endif()


if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
