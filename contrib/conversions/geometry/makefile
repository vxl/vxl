# This is the geometry conversions directory makefile

USES := vxl gel oxl
IMMEDIATE_LIBS := -losl -lvdgl -vbl

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
