VXL := 1

################################
include ${IUEROOT}/config/top-params.mk
################################

USES := vxl

LIBRARY := vgl
VERSION := 1.0

SOURCES += vgl_point_2d.cxx
SOURCES += vgl_homg_point_2d.cxx
SOURCES += vgl_point_3d.cxx
SOURCES += vgl_homg_point_3d.cxx
SOURCES += vgl_plane_3d.cxx
SOURCES += vgl_homg_plane_3d.cxx
SOURCES += vgl_line_2d.cxx
SOURCES += vgl_homg_line_2d.cxx
SOURCES += vgl_box_2d.cxx
SOURCES += vgl_box_3d.cxx
SOURCES += vgl_homg_line_3d_2_points.cxx

SUBDIRS += algo

ifneq (1, $(NOTEST))
SUBDIRS += tests
endif

IULIBS :=

################################
include ${IUEROOT}/config/top-rules.mk
################################
