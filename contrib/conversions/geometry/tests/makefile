USES := conversions vxl gel oxl
USE_NETLIB := 1
include ${IUEROOT}/vxl/vil/config.mk
IULIBS := -lvdgl -lvsol -lvgl-algo -lvgl -lvnl-algo -lvnl -losl -lvil -lvbl -lvul -ltestlib

TESTMAIN := test_driver.cxx

TESTS += test_osl_to_vdgl.cxx

ifneq (,$(wildcard $(IUEROOT)/SpatialObjects/DigitalGeometry/DigitalCurve.h))
USES += COOL GeneralUtility SpatialObjects Numerics
IULIBS += -lDigitalGeometry -lCompGeom -lTopology -lGroups -lGeometry -lSpatialBasics -lBasics -lSolver -lNumerics
TESTS += test_DigitalCurve_to_vdgl.cxx
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
