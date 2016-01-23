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
    bugl_gaussian_point_2d.h     bugl_gaussian_point_2d.txx
    bugl_normal_point_2d.h
    bugl_normal_point_2d_sptr.h

    bugl_uncertainty_point_3d.h
    bugl_gaussian_point_3d.h     bugl_gaussian_point_3d.txx
    bugl_normal_point_3d.h
    bugl_normal_point_3d_sptr.h

    bugl_curve_3d.h             bugl_curve_3d.cxx
    bugl_random_transform_set_2d.h     bugl_random_transform_set_2d.txx
)
aux_source_directory(Templates bugl_sources)

add_library(bugl ${bugl_sources})
install_targets(/lib bugl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bugl ${bugl_sources})
target_link_libraries(bugl vnl vgl_algo vgl vbl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)
