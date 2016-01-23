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

add_library(ipts ${ipts_sources})
install_targets(/lib ipts)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/mul/ipts ${ipts_sources})
target_link_libraries(ipts vimt_algo vimt vgl vil_algo vil)

if(BUILD_MUL_TOOLS)
  add_subdirectory(tools)
endif()


if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
