USES := conversions vxl
include ${IUEROOT}/vxl/vil/config.mk
IULIBS := -lvil -lvpl

ifneq (,$(wildcard $(IUEROOT)/mul))
USES += mul
IULIBS += -lmil -lmbl -lvsl -lvnl-algo -lvnl-io -lvnl -lvgl
#TESTS += test_mil_conversions.cxx
endif
ifneq (,$(wildcard $(IUEROOT)/Image))
USES += Image COOL Numerics GeneralUtility
IULIBS += -lImageClasses -lBasics -lNumerics
TESTS += test_image_conversions.cxx
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
