USES := conversions vxl
TRY_TIFF := 1
TRY_JPEG := 1
TRY_PNG := 1
IULIBS := -lvil -lvnl

ifneq (,$(wildcard $(IUEROOT)/Image))
USES += Image COOL Numerics GeneralUtility
IULIBS += -lImageClasses -lBasics -lNumerics
TESTS += test_image_conversions.cxx
PARAMS_test_image_conversions := /tmp/vil_test_memory_image_of.ppm
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
