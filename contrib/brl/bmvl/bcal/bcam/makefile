USES += contrib/brl contrib/brl/bbas core

LIBRARY := bcal
VERSION := 1.0

SOURCES += camera.cpp
SOURCES += CameraNode.cpp
SOURCES += CameraGraph.cpp
SOURCES += CalibrationObject.cpp
SOURCES += CalibratePlane.cpp
SOURCES += ZhangCameraNode.cpp
SOURCES += ZhangLinearCalibrate.cpp

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
