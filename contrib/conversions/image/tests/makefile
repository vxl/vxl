USES += contrib/conversions core
include ${VXLROOT}/core/vil1/config.mk
include ${VXLROOT}/core/vul/config.mk
include ${VXLROOT}/core/vbl/config.mk
include ${VXLROOT}/core/vpl/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_vil1_vbl_conversions.cxx

ifneq (,$(wildcard $(VXLROOT)/Image))
USES += Image COOL GeneralUtility
include ${VXLROOT}/core/vnl/algo/config.mk
IULIBS += -lImageClasses -lBasics
TESTS += test_image_conversions.cxx
endif

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
