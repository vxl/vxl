USES += core

LIBRARY := vgl
VERSION := 1.0

MANPAGE_SOURCES += vgl_fwd.h

# General
MANPAGE_SOURCES += vgl_box_2d.txx
MANPAGE_SOURCES += vgl_box_3d.txx
MANPAGE_SOURCES += vgl_vector_2d.txx
MANPAGE_SOURCES += vgl_vector_3d.txx
SOURCES += vgl_1d_basis.cxx
MANPAGE_SOURCES += vgl_1d_basis.txx
SOURCES += vgl_homg.cxx

# Points
MANPAGE_SOURCES += vgl_homg_point_1d.txx
MANPAGE_SOURCES += vgl_point_2d.txx
MANPAGE_SOURCES += vgl_homg_point_2d.txx
MANPAGE_SOURCES += vgl_point_3d.txx
MANPAGE_SOURCES += vgl_homg_point_3d.txx

# Lines
MANPAGE_SOURCES += vgl_line_2d.txx
MANPAGE_SOURCES += vgl_homg_line_2d.txx
MANPAGE_SOURCES += vgl_homg_line_3d_2_points.txx
MANPAGE_SOURCES += vgl_line_3d_2_points.txx
MANPAGE_SOURCES += vgl_line_segment_2d.txx
MANPAGE_SOURCES += vgl_line_segment_3d.txx

# Other curves
MANPAGE_SOURCES += vgl_conic.txx
MANPAGE_SOURCES += vgl_polygon.txx
MANPAGE_SOURCES += vgl_conic_segment_2d.txx

# Surfaces
MANPAGE_SOURCES += vgl_plane_3d.txx
MANPAGE_SOURCES += vgl_homg_plane_3d.txx
MANPAGE_SOURCES += vgl_sphere_3d.txx
MANPAGE_SOURCES += vgl_cylinder.txx

# Volumes
MANPAGE_SOURCES += vgl_orient_box_3d.txx

# Functions
MANPAGE_SOURCES += vgl_closest_point.txx
MANPAGE_SOURCES += vgl_distance.txx
MANPAGE_SOURCES += vgl_convex.txx
MANPAGE_SOURCES += vgl_clip.txx
MANPAGE_SOURCES += vgl_area.txx
MANPAGE_SOURCES += vgl_lineseg_test.txx
MANPAGE_SOURCES += vgl_triangle_test.txx
MANPAGE_SOURCES += vgl_polygon_test.txx

MANPAGE_SOURCES += vgl_region_scan_iterator.h
MANPAGE_SOURCES += vgl_triangle_scan_iterator.txx
MANPAGE_SOURCES += vgl_window_scan_iterator.txx
MANPAGE_SOURCES += vgl_polygon_scan_iterator.txx
MANPAGE_SOURCES += vgl_ellipse_scan_iterator.txx

# internals
SOURCES += internals/gpc.c

SUBDIRS += algo

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
