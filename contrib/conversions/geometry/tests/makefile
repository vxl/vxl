USES := conversions vxl gel
IULIBS := -lvdgl -lvsol -lvgl -lvnl -lvbl

ifneq (,$(wildcard $(IUEROOT)/SpatialObjects))
USES += COOL GeneralUtility SpatialObjects Numerics
IULIBS += -lDigitalGeometry -lCompGeom -lTopology -lGroups -lGeometry -lSpatialBasics -lBasics -lSolver -lNumerics
TESTS += test_DigitalCurve_to_vdgl.cxx
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
