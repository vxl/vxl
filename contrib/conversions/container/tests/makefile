USES := conversions vxl
IULIBS := -lvnl # for vnl_test.h

TESTS += test_container.cxx

ifneq (,$(wildcard $(IUEROOT)/COOL))
USES += COOL Numerics
IULIBS += -lTJCOOL -liue-math
TESTS += test_COOL_container.cxx
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
