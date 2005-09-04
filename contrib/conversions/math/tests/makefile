USES += contrib/conversions core
include ${VXLROOT}/core/vnl/config.mk
include ${VXLROOT}/core/vbl/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_matrix_conversions.cxx

ifneq (,$(wildcard $(VXLROOT)/COOL))
USES += COOL Numerics
IULIBS += -lTJCOOL
TESTS += test_CoolMatrix_conversions.cxx
endif

ifneq (,$(wildcard $(VXLROOT)/Numerics))
USES += Numerics
IULIBS += -lNumerics
TESTS += test_IUE_matrix_conversions.cxx
endif

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
