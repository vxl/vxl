# This is the geometry conversions directory makefile

USES := vxl gel
IMMEDIATE_LIBS := -lvdgl -vbl

SUBDIRS := tests

ifneq (,$(wildcard $(IUEROOT)/SpatialObjects))
USES += COOL GeneralUtility SpatialObjects Numerics
IMMEDIATE_LIBS += -lDigitalGeometry -lSpatialBasics -lBasics
MANPAGE_SOURCES += DigitalCurve_to_vdgl.h
endif

################################
include ${IUEROOT}/config/top.mk
################################
