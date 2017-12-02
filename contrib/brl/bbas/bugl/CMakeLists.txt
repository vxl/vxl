# brl/bbas/bugl/CMakeLists.txt
# written by Kongbin Kang for creating a Brown uncertainty geometry library.

doxygen_add_library(contrib/brl/bbas/bugl
  DEPENDS contrib/gel/vdgl core/vnl core/vgl core/vbl
  PACKAGE contrib/brl
  )

include_directories(${BRL_INCLUDE_DIR})
include_directories(${BRL_INCLUDE_DIR}/bbas )

set(bugl_sources
    bugl_uncertainty_point_2d.h
    bugl_gaussian_point_2d.h     bugl_gaussian_point_2d.hxx
    bugl_normal_point_2d.h
    bugl_normal_point_2d_sptr.h

    bugl_uncertainty_point_3d.h
    bugl_gaussian_point_3d.h     bugl_gaussian_point_3d.hxx
    bugl_normal_point_3d.h
    bugl_normal_point_3d_sptr.h

    bugl_curve_3d.h             bugl_curve_3d.cxx
    bugl_random_transform_set_2d.h     bugl_random_transform_set_2d.hxx
)
aux_source_directory(Templates bugl_sources)

vxl_add_library(LIBRARY_NAME bugl LIBRARY_SOURCES ${bugl_sources})
target_link_libraries(bugl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
