# core/vgl/CMakeLists.txt

if( NOT BUILD_NONCOMMERCIAL )
  include(${VXL_CMAKE_DIR}/FindClipper.cmake)
  if(CLIPPER_FOUND)
    add_definitions( ${CLIPPER_DEFINITIONS} )
    add_definitions( -DHAS_CLIPPER )
    include_directories( ${CLIPPER_INCLUDE_DIR} )
  endif()
endif()

set( vgl_sources
  # General
  vgl_fwd.h                      vgl_export.h
  vgl_tolerance.h                vgl_tolerance.txx
  vgl_box_2d.h                   vgl_box_2d.txx
  vgl_box_3d.h                   vgl_box_3d.txx
  vgl_vector_2d.h                vgl_vector_2d.txx
  vgl_vector_3d.h                vgl_vector_3d.txx
  vgl_1d_basis.h                 vgl_1d_basis.cxx  vgl_1d_basis.txx
  vgl_homg.h                     vgl_homg.cxx

  # Points
  vgl_homg_point_1d.h            vgl_homg_point_1d.txx
  vgl_point_2d.h                 vgl_point_2d.txx
  vgl_homg_point_2d.h            vgl_homg_point_2d.txx
  vgl_point_3d.h                 vgl_point_3d.txx
  vgl_homg_point_3d.h            vgl_homg_point_3d.txx
  vgl_pointset_3d.h
  # Lines
  vgl_line_2d.h                  vgl_line_2d.txx
  vgl_homg_line_2d.h             vgl_homg_line_2d.txx
  vgl_homg_line_3d_2_points.h    vgl_homg_line_3d_2_points.txx
  vgl_line_3d_2_points.h         vgl_line_3d_2_points.txx
  vgl_line_segment_2d.h          vgl_line_segment_2d.txx
  vgl_line_segment_3d.h          vgl_line_segment_3d.txx
  vgl_infinite_line_3d.h         vgl_infinite_line_3d.txx
  vgl_ray_3d.h                   vgl_ray_3d.txx

  # Other curves
  vgl_conic.h                    vgl_conic.txx
  vgl_conic_segment_2d.h         vgl_conic_segment_2d.txx
  vgl_polygon.h                  vgl_polygon.txx
  vgl_sphere_3d.h                vgl_sphere_3d.txx
  vgl_cylinder.h                 vgl_cylinder.txx

  # Planes
  vgl_plane_3d.h                 vgl_plane_3d.txx
  vgl_homg_plane_3d.h            vgl_homg_plane_3d.txx

  # Spline
  vgl_cubic_spline_3d.h
  vgl_cubic_spline_2d.h

  # Functions
  vgl_closest_point.h            vgl_closest_point.txx
  vgl_distance.h                 vgl_distance.txx
  vgl_clip.h                     vgl_clip.txx
  vgl_area.h                     vgl_area.txx
  vgl_convex.h                   vgl_convex.txx
  vgl_intersection.h             vgl_intersection.txx
  vgl_bounding_box.h             vgl_bounding_box.txx
  vgl_lineseg_test.h             vgl_lineseg_test.txx
  vgl_triangle_test.h            vgl_triangle_test.txx
  vgl_polygon_test.h             vgl_polygon_test.txx
  vgl_triangle_3d.h              vgl_triangle_3d.cxx
  vgl_frustum_3d.h               vgl_frustum_3d.txx

  vgl_region_scan_iterator.h
  vgl_triangle_scan_iterator.h   vgl_triangle_scan_iterator.txx
  vgl_window_scan_iterator.h     vgl_window_scan_iterator.txx
  vgl_polygon_scan_iterator.h    vgl_polygon_scan_iterator.txx
  vgl_ellipse_scan_iterator.h    vgl_ellipse_scan_iterator.txx
)

if( BUILD_NONCOMMERCIAL )
  set( vgl_sources ${vgl_sources}
  # internals
  internals/gpc.h                internals/gpc.c
  )
endif()

aux_source_directory(Templates vgl_sources)

add_library(vgl ${vgl_sources})
target_link_libraries( vgl vcl )

doxygen_add_library(core/vgl
  #DEPENDS core/vsl core/vnl
  PACKAGE core
  DESCRIPTION "Geometry Library"
  )
if( NOT BUILD_NONCOMMERCIAL )
  if(CLIPPER_FOUND)
    target_link_libraries( vgl ${CLIPPER_LIBRARIES} )
  endif()
endif()

install_targets(/lib vgl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vgl ${vgl_sources})

add_subdirectory(algo)

if( BUILD_EXAMPLES )
  add_subdirectory(examples)
endif()

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
