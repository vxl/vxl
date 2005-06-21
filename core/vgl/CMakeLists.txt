# core/vgl/CMakeLists.txt

SET( vgl_sources
  # General
  vgl_fwd.h
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

  # Lines
  vgl_line_2d.h                  vgl_line_2d.txx
  vgl_homg_line_2d.h             vgl_homg_line_2d.txx
  vgl_homg_line_3d_2_points.h    vgl_homg_line_3d_2_points.txx
  vgl_line_3d_2_points.h         vgl_line_3d_2_points.txx
  vgl_line_segment_2d.h          vgl_line_segment_2d.txx
  vgl_line_segment_3d.h          vgl_line_segment_3d.txx

  # Other curves
  vgl_conic.h                    vgl_conic.txx
  vgl_conic_segment_2d.h         vgl_conic_segment_2d.txx
  vgl_polygon.h                  vgl_polygon.txx

  # Planes
  vgl_plane_3d.h                 vgl_plane_3d.txx
  vgl_homg_plane_3d.h            vgl_homg_plane_3d.txx

  # Functions
  vgl_closest_point.h            vgl_closest_point.txx
  vgl_distance.h                 vgl_distance.txx
  vgl_clip.h                     vgl_clip.txx
  vgl_area.h                     vgl_area.txx
  vgl_convex.h                   vgl_convex.txx

  vgl_lineseg_test.h             vgl_lineseg_test.txx
  vgl_triangle_test.h            vgl_triangle_test.txx
  vgl_polygon_test.h             vgl_polygon_test.txx

  vgl_region_scan_iterator.h
  vgl_triangle_scan_iterator.h   vgl_triangle_scan_iterator.txx
  vgl_window_scan_iterator.h     vgl_window_scan_iterator.txx
  vgl_polygon_scan_iterator.h    vgl_polygon_scan_iterator.txx
  vgl_ellipse_scan_iterator.h    vgl_ellipse_scan_iterator.txx

  # internals
  internals/gpc.h                internals/gpc.c
)

AUX_SOURCE_DIRECTORY(Templates vgl_sources)

ADD_LIBRARY(vgl ${vgl_sources})
TARGET_LINK_LIBRARIES( vgl vcl )

SUBDIRS( algo )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
