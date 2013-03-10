# Brown view sphere directory
# brl/bbas/vsph/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(vsph_sources
    vsph_view_sphere.h       vsph_view_sphere.txx
    vsph_view_point.h
    vsph_spherical_coord.h   vsph_spherical_coord.cxx     vsph_spherical_coord_sptr.h
    vsph_sph_point_3d.h      vsph_sph_point_3d.cxx
    vsph_sph_point_2d.h      vsph_sph_point_2d.cxx
    vsph_sph_box_2d.h        vsph_sph_box_2d.cxx
    vsph_camera_bounds.h     vsph_camera_bounds.cxx
    vsph_utils.h             vsph_utils.cxx
    vsph_unit_sphere.h       vsph_unit_sphere.cxx
    vsph_segment_sphere.h    vsph_segment_sphere.cxx
    vsph_grid_index_2d.h     vsph_grid_index_2d.cxx
    vsph_sph_cover_2d.h      vsph_sph_cover_2d.cxx
  )
AUX_SOURCE_DIRECTORY(Templates vsph_sources)

ADD_LIBRARY(vsph ${vsph_sources})
TARGET_LINK_LIBRARIES(vsph vpgl vpgl_algo vpgl_io vgl_algo vgl_io vgl vbl vnl vcl bpgl_algo bvrml)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)



INSTALL_TARGETS(/lib vsph)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/vsph ${vsph_sources})
