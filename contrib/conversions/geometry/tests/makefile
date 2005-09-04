USES += contrib/conversions contrib/gel contrib/oxl core
include ${VXLROOT}/contrib/oxl/osl/config.mk
include ${VXLROOT}/contrib/gel/vdgl/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_osl_to_vdgl.cxx
DEFINES += -DHAS_VDGL -DHAS_OSL

ifneq (,$(wildcard $(VXLROOT)/SpatialObjects/DigitalGeometry/DigitalCurve.h))
USES += COOL GeneralUtility SpatialObjects Numerics
IULIBS += -lDigitalGeometry -lCompGeom -lTopology -lGroups -lGeometry -lSpatialBasics -lBasics -lSolver -lNumerics
TESTS += test_DigitalCurve_to_vdgl.cxx
endif

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
