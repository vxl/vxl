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
SOURCES += vgl_polygon.cxx

# Planes
MANPAGE_SOURCES += vgl_plane_3d.txx
MANPAGE_SOURCES += vgl_homg_plane_3d.txx

# Functions
SOURCES += vgl_distance.cxx
SOURCES += vgl_clip.cxx
SOURCES += vgl_area.cxx
SOURCES += vgl_lineseg_test.cxx
SOURCES += vgl_triangle_test.cxx
SOURCES += vgl_polygon_test.cxx
#SOURCES += vgl_test.cxx

SOURCES += vgl_region_scan_iterator.cxx
SOURCES += vgl_triangle_scan_iterator.cxx
SOURCES += vgl_window_scan_iterator.cxx
SOURCES += vgl_polygon_scan_iterator.cxx
SOURCES += vgl_ellipse_scan_iterator.cxx

# internals
SOURCES += internals/gpc.c

SUBDIRS += algo
SUBDIRS += io

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
