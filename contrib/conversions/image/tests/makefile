USES += contrib/conversions core
include ${IUEROOT}/core/vil1/config.mk
include ${IUEROOT}/core/vul/config.mk
include ${IUEROOT}/core/vbl/config.mk
include ${IUEROOT}/core/vpl/config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_vil1_vbl_conversions.cxx

ifneq (,$(wildcard $(IUEROOT)/Image))
USES += Image COOL GeneralUtility
include ${IUEROOT}/core/vnl/algo/config.mk
IULIBS += -lImageClasses -lBasics
TESTS += test_image_conversions.cxx
endif

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
