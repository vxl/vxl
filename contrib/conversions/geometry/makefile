# This is the geometry conversions directory makefile
ifneq (,$(wildcard $(IUEROOT)/SpatialObjects))

USES := vxl gel COOL GeneralUtility SpatialObjects Numerics
IMMEDIATE_LIBS := -lvdgl -vbl -lDigitalGeometry -lSpatialBasics -lBasics

SUBDIRS := tests

MANPAGE_SOURCES += DigitalCurve_to_vdgl.h
endif

################################
include ${IUEROOT}/config/top.mk
################################
