USES := conversions vxl
include ${IUEROOT}/vxl/vil/config.mk

IULIBS := -lvil -lvbl -lvpl
TESTS += test_vil_vbl_conversions.cxx

ifneq (,$(wildcard $(IUEROOT)/Image))
USES += Image COOL Numerics GeneralUtility
IULIBS += -lImageClasses -lBasics -lNumerics
TESTS += test_image_conversions.cxx
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
