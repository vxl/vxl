VXL := 1

################################
include ${IUEROOT}/config/top-params.mk
################################

USES := vxl

LIBRARY := vgl
VERSION := 1.0

#MANPAGE_SOURCES += vgl_point_2d.txx
#MANPAGE_SOURCES += vgl_homg_point_2d.txx
#MANPAGE_SOURCES += vgl_point_3d.txx
#MANPAGE_SOURCES += vgl_homg_point_3d.txx
#MANPAGE_SOURCES += vgl_plane_3d.txx
#MANPAGE_SOURCES += vgl_homg_plane_3d.txx
#MANPAGE_SOURCES += vgl_line_2d.txx
#MANPAGE_SOURCES += vgl_homg_line_2d.txx
#MANPAGE_SOURCES += vgl_box_2d.txx
#MANPAGE_SOURCES += vgl_box_3d.txx
#MANPAGE_SOURCES += vgl_homg_line_3d_2_points.txx

SUBDIRS += algo

ifndef NOTEST
SUBDIRS += tests
endif

IULIBS :=

################################
include ${IUEROOT}/config/top-rules.mk
################################
