USES := conversions vxl
include ${IUEROOT}/vxl/vil/config.mk

TESTMAIN := test_driver.cxx

IULIBS := -lvil -lvbl -lvpl -lvul -ltestlib

TESTS += test_vil_vbl_conversions.cxx

ifneq (,$(wildcard $(IUEROOT)/Image))
USES += Image COOL Numerics GeneralUtility
IULIBS += -lImageClasses -lBasics -lNumerics
TESTS += test_image_conversions.cxx
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
