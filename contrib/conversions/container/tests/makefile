USES += contrib/conversions core
include ${IUEROOT}/core/vnl/config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_container.cxx

ifneq (,$(wildcard $(IUEROOT)/COOL))
USES += COOL Numerics
IULIBS += -lTJCOOL -liue-math
TESTS += test_COOL_container.cxx
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
