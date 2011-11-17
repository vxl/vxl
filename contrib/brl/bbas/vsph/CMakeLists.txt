# Brown view sphere directory
# brl/bbas/vsph/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(vsph_sources
    vsph_view_sphere.h       vsph_view_sphere.txx
    vsph_view_point.h
    vsph_spherical_coord.h   vsph_spherical_coord.cxx     vsph_spherical_coord_sptr.h
    vsph_sph_point_3d.h      vsph_sph_point_3d.cxx
    vsph_camera_bounds.h     vsph_camera_bounds.cxx
   )
AUX_SOURCE_DIRECTORY(Templates vsph_sources)

ADD_LIBRARY(vsph ${vsph_sources})
TARGET_LINK_LIBRARIES(vsph vpgl vpgl_algo vpgl_io vgl_algo vgl_io vgl vbl vnl vcl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
