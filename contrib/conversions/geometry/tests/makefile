USES += contrib/conversions contrib/gel contrib/oxl core
include ${IUEROOT}/contrib/oxl/osl/config.mk
include ${IUEROOT}/contrib/gel/vdgl/config.mk
include ${IUEROOT}/core/testlib/config.mk

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
