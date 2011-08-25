# Brown view sphere directory
# gel/mrc/vpgl/vsph/CMakeLists.txt

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl/vsph )

SET(vsph_sources
    vsph_view_sphere.h       vsph_view_sphere.txx
    vsph_view_point.h
    vsph_spherical_coord.h   vsph_spherical_coord.cxx     vsph_spherical_coord_sptr.h
    vsph_sph_point_3d.h      vsph_sph_point_3d.cxx
   )
AUX_SOURCE_DIRECTORY(Templates vsph_sources)

ADD_LIBRARY(vsph ${vsph_sources})
TARGET_LINK_LIBRARIES(vsph vpgl vgl_algo vgl_io vgl vbl vnl vcl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
