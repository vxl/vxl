USES := conversions vxl gel oxl
include ${IUEROOT}/oxl/osl/config.mk
include ${IUEROOT}/gel/vdgl/config.mk
include ${IUEROOT}/vxl/testlib/config.mk

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
