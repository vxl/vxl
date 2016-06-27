# Brown view sphere directory
# brl/bbas/vsph/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(vsph_sources
    vsph_view_sphere.h       vsph_view_sphere.hxx
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
aux_source_directory(Templates vsph_sources)

vxl_add_library(LIBRARY_NAME vsph LIBRARY_SOURCES ${vsph_sources})
target_link_libraries(vsph ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vcl bpgl_algo bvrml)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()