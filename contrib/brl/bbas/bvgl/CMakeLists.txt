# Brown basic geometry library
# brl/bbas/bvgl/CMakeLists.txt

doxygen_add_library(contrib/brl/bbas/bvgl
  DEPENDS core/vgl core/vnl
  PACKAGE contrib/brl
  )

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})

SET(bvgl_sources
 bvgl_articulated_poly.h bvgl_articulated_poly.cxx bvgl_articulated_poly_sptr.h
 bvgl_change_obj.h   bvgl_change_obj.cxx
 bvgl_change_obj_sptr.h
 bvgl_changes.h      bvgl_changes.cxx
 bvgl_changes_sptr.h
 bvgl_point_3d_cmp.h
 bvgl_triangle_interpolation_iterator.h  bvgl_triangle_interpolation_iterator.txx
 bvgl_volume_of_intersection.h           bvgl_volume_of_intersection.txx
 bvgl_ray_pyramid.h                      bvgl_ray_pyramid.cxx
)
AUX_SOURCE_DIRECTORY(Templates bvgl_sources)

ADD_LIBRARY(bvgl ${bvgl_sources})
INSTALL_TARGETS(/lib bvgl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bvgl ${bvgl_sources})
TARGET_LINK_LIBRARIES(bvgl vsol vgl_algo vgl_io vgl vnl_algo vnl vil vbl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
