USES += contrib/brl contrib/brl/bbas core

LIBRARY := bcal
VERSION := 1.0

SOURCES += camera.cxx
SOURCES += camera_node.cxx
SOURCES += euclidean_transformation.cxx
MANPAGE_SOURCES += camera_graph.h
SOURCES += calibrate_object.cxx
SOURCES += calibrate_plane.cxx
SOURCES += zhang_camera_node.cxx
SOURCES += zhang_linear_calibrate.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
