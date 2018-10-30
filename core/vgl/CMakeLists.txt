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
  vgl_fwd.h
  vgl_tolerance.h                vgl_tolerance.hxx
  vgl_box_2d.h                   vgl_box_2d.hxx
  vgl_box_3d.h                   vgl_box_3d.hxx
  vgl_vector_2d.h                vgl_vector_2d.hxx
  vgl_vector_3d.h                vgl_vector_3d.hxx
  vgl_1d_basis.h                 vgl_1d_basis.cxx  vgl_1d_basis.hxx
  vgl_homg.h                     vgl_homg.cxx

  # Points
  vgl_homg_point_1d.h            vgl_homg_point_1d.hxx
  vgl_point_2d.h                 vgl_point_2d.hxx
  vgl_homg_point_2d.h            vgl_homg_point_2d.hxx
  vgl_point_3d.h                 vgl_point_3d.hxx
  vgl_homg_point_3d.h            vgl_homg_point_3d.hxx
  vgl_pointset_3d.h
  # Lines
  vgl_line_2d.h                  vgl_line_2d.hxx
  vgl_homg_line_2d.h             vgl_homg_line_2d.hxx
  vgl_homg_line_3d_2_points.h    vgl_homg_line_3d_2_points.hxx
  vgl_line_3d_2_points.h         vgl_line_3d_2_points.hxx
  vgl_line_segment_2d.h          vgl_line_segment_2d.hxx
  vgl_line_segment_3d.h          vgl_line_segment_3d.hxx
  vgl_infinite_line_3d.h         vgl_infinite_line_3d.hxx
  vgl_ray_3d.h                   vgl_ray_3d.hxx

  # Other curves
  vgl_conic.h                    vgl_conic.hxx
  vgl_conic_segment_2d.h         vgl_conic_segment_2d.hxx
  vgl_polygon.h                  vgl_polygon.hxx
  vgl_sphere_3d.h                vgl_sphere_3d.hxx
  vgl_cylinder.h                 vgl_cylinder.hxx

  # Planes
  vgl_plane_3d.h                 vgl_plane_3d.hxx
  vgl_homg_plane_3d.h            vgl_homg_plane_3d.hxx

  # Spline
  vgl_cubic_spline_3d.h
  vgl_cubic_spline_2d.h

  # Functions
  vgl_closest_point.h            vgl_closest_point.hxx
  vgl_distance.h                 vgl_distance.hxx
  vgl_clip.h                     vgl_clip.hxx
  vgl_area.h                     vgl_area.hxx
  vgl_convex.h                   vgl_convex.hxx
  vgl_intersection.h             vgl_intersection.hxx
  vgl_bounding_box.h             vgl_bounding_box.hxx
  vgl_oriented_box_2d.h          vgl_oriented_box_2d.hxx
  vgl_fit_oriented_box_2d.h      vgl_fit_oriented_box_2d.hxx
  vgl_lineseg_test.h             vgl_lineseg_test.hxx
  vgl_triangle_test.h            vgl_triangle_test.hxx
  vgl_polygon_test.h             vgl_polygon_test.hxx
  vgl_triangle_3d.h              vgl_triangle_3d.cxx
  vgl_frustum_3d.h               vgl_frustum_3d.hxx
  vgl_affine_coordinates.h       vgl_affine_coordinates.hxx

  vgl_region_scan_iterator.h
  vgl_triangle_scan_iterator.h   vgl_triangle_scan_iterator.hxx
  vgl_window_scan_iterator.h     vgl_window_scan_iterator.hxx
  vgl_polygon_scan_iterator.h    vgl_polygon_scan_iterator.hxx
  vgl_ellipse_scan_iterator.h    vgl_ellipse_scan_iterator.hxx
)

if( BUILD_NONCOMMERCIAL )
  set( vgl_sources ${vgl_sources}
  # internals
  internals/gpc.h                internals/gpc.c
  )
endif()

aux_source_directory(Templates vgl_sources)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vgl LIBRARY_SOURCES ${vgl_sources})
target_link_libraries( ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vcl )
set(CURR_LIB_NAME vgl)
set_vxl_library_properties(
     TARGET_NAME ${VXL_LIB_PREFIX}${CURR_LIB_NAME}
     BASE_NAME ${CURR_LIB_NAME}
     EXPORT_HEADER_FILE ${VXLCORE_BINARY_INCLUDE_DIR}/${CURR_LIB_NAME}/${CURR_LIB_NAME}_export.h
     INSTALL_DIR   ${VXL_INSTALL_INCLUDE_DIR}/core/${CURR_LIB_NAME}
     #USE_HIDDEN_VISIBILITY
)

doxygen_add_library(core/vgl
  #DEPENDS core/vsl core/vnl
  PACKAGE core
  DESCRIPTION "Geometry Library"
  )
if( NOT BUILD_NONCOMMERCIAL )
  if(CLIPPER_FOUND)
    target_link_libraries( ${VXL_LIB_PREFIX}vgl ${CLIPPER_LIBRARIES} )
  endif()
endif()

add_subdirectory(algo)

if( VXL_BUILD_EXAMPLES )
  add_subdirectory(examples)
endif()

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(vgl-obj OBJECT ${vgl_sources})
endif()
