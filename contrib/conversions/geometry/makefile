# This is the geometry conversions directory makefile

USES += contrib/gel contrib/oxl core
IMMEDIATE_LIBS := -losl -lvdgl -vbl
MANPAGE_SOURCES += osl_to_vdgl.h

ifneq (,$(wildcard $(IUEROOT)/SpatialObjects))
USES += COOL GeneralUtility SpatialObjects Numerics
IMMEDIATE_LIBS += -lDigitalGeometry -lSpatialBasics -lBasics
MANPAGE_SOURCES += DigitalCurve_to_vdgl.h
endif

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS := tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
