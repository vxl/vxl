USES += vxl mul
include ${IUEROOT}/mul/mil/algo/config.mk

TESTS += test_mil.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################

ifeq "$(TJ_GCC30)$(optimize)" "1-O2"
# gcc 3.0 needs too much virtual memory with -O2
FLAGS_test_mil.cxx += -O0
endif
