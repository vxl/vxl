USES := conversions vxl
IULIBS := -lvnl -lvbl -ltestlib

TESTMAIN := test_driver.cxx

TESTS += test_matrix_conversions.cxx

ifneq (,$(wildcard $(IUEROOT)/COOL))
USES += COOL Numerics
IULIBS += -lTJCOOL
TESTS += test_CoolMatrix_conversions.cxx
endif

ifneq (,$(wildcard $(IUEROOT)/Numerics))
USES += Numerics
IULIBS += -lNumerics
TESTS += test_IUE_matrix_conversions.cxx
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
