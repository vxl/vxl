#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl
LIBRARY := vgl-io
VERSION := 1.0
IMMEDIATE_LIBS := -lvgl-io -lvgl -lvsl

SOURCES += vgl_io_polygon.cxx

MANPAGE_SOURCES += vgl_io_box_2d.txx
MANPAGE_SOURCES += vgl_io_box_3d.txx
MANPAGE_SOURCES += vgl_io_homg_line_2d.txx
MANPAGE_SOURCES += vgl_io_homg_line_3d_2_points.txx
MANPAGE_SOURCES += vgl_io_homg_plane_3d.txx
MANPAGE_SOURCES += vgl_io_homg_point_1d.txx
MANPAGE_SOURCES += vgl_io_homg_point_2d.txx
MANPAGE_SOURCES += vgl_io_homg_point_3d.txx
MANPAGE_SOURCES += vgl_io_line_2d.txx
MANPAGE_SOURCES += vgl_io_line_3d_2_points.txx
MANPAGE_SOURCES += vgl_io_line_segment_2d.txx
MANPAGE_SOURCES += vgl_io_line_segment_3d.txx
MANPAGE_SOURCES += vgl_io_point_2d.txx
MANPAGE_SOURCES += vgl_io_point_3d.txx
MANPAGE_SOURCES += vgl_io_vector_2d.txx
MANPAGE_SOURCES += vgl_io_vector_3d.txx
MANPAGE_SOURCES += vgl_io_conic.txx

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
