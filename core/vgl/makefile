#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl

LIBRARY := vgl
VERSION := 1.0

SOURCES += vgl_polygon.cxx

SOURCES += vgl_distance.cxx
SOURCES += vgl_clip.cxx
SOURCES += vgl_lineseg_test.cxx
SOURCES += vgl_triangle_test.cxx
SOURCES += vgl_polygon_test.cxx
SOURCES += vgl_test.cxx

SOURCES += vgl_region_scan_iterator.cxx
SOURCES += vgl_triangle_scan_iterator.cxx
SOURCES += vgl_window_scan_iterator.cxx
SOURCES += vgl_polygon_scan_iterator.cxx
# coming soon: vgl_ellipse_scan_iterator.cxx

MANPAGE_SOURCES += vgl_point_2d.txx
MANPAGE_SOURCES += vgl_homg_point_2d.txx
MANPAGE_SOURCES += vgl_point_3d.txx
MANPAGE_SOURCES += vgl_homg_point_3d.txx
MANPAGE_SOURCES += vgl_plane_3d.txx
MANPAGE_SOURCES += vgl_homg_plane_3d.txx
MANPAGE_SOURCES += vgl_line_2d.txx
MANPAGE_SOURCES += vgl_homg_line_2d.txx
MANPAGE_SOURCES += vgl_box_2d.txx
MANPAGE_SOURCES += vgl_box_3d.txx
MANPAGE_SOURCES += vgl_homg_line_3d_2_points.txx
MANPAGE_SOURCES += vgl_line_segment_3d.txx

SUBDIRS += algo

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
