USES := conversions vxl gel
USE_NETLIB := 1
IULIBS := -lvdgl -lvsol -lvgl -lvnl-algo -lvnl -lvbl -lvul

ifneq (,$(wildcard $(IUEROOT)/SpatialObjects))
USES += COOL GeneralUtility SpatialObjects Numerics
IULIBS += -lDigitalGeometry -lCompGeom -lTopology -lGroups -lGeometry -lSpatialBasics -lBasics -lSolver -lNumerics
TESTS += test_DigitalCurve_to_vdgl.cxx
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
