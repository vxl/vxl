USES += contrib/brl contrib/brl/bbas core

LIBRARY := bcal
VERSION := 1.0

SOURCES += camera.cpp
SOURCES += camera_node.cpp
SOURCES += euclidean_transformation.cpp
MANPAGE_SOURCES += camera_graph.h
SOURCES += calibrate_object.cpp
SOURCES += calibrate_plane.cpp
SOURCES += zhang_camera_node.cpp
SOURCES += zhang_linear_calibrate.cpp

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
