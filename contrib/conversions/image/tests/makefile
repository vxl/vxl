USES := conversions vxl
include ${IUEROOT}/vxl/vil/config.mk
include ${IUEROOT}/vxl/vul/config.mk
include ${IUEROOT}/vxl/vbl/config.mk
include ${IUEROOT}/vxl/vpl/config.mk
include ${IUEROOT}/vxl/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_vil_vbl_conversions.cxx

ifneq (,$(wildcard $(IUEROOT)/Image))
USES += Image COOL Numerics GeneralUtility
IULIBS += -lImageClasses -lBasics -lNumerics
TESTS += test_image_conversions.cxx
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
