USES += contrib/brl core

LIBRARY := bcal
VERSION := 1.0

SOURCES += bcal_camera.cxx
SOURCES += bcal_camera_node.cxx
SOURCES += bcal_euclidean_transformation.cxx
MANPAGE_SOURCES += bcal_camera_graph.h
MANPAGE_SOURCES += bcal_calibrate_object.h
SOURCES += bcal_calibrate_plane.cxx
SOURCES += bcal_zhang_camera_node.cxx
SOURCES += bcal_zhang_linear_calibrate.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
