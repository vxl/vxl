USES += contrib/mul core
include ${IUEROOT}/contrib/mul/mil/algo/config.mk
include $(IUEROOT)/core/vpl/config.mk
include $(IUEROOT)/core/testlib/config.mk

TESTS += test_mil.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################

ifeq "$(TJ_GCC30)$(optimize)" "1-O2"
# gcc 3.0 needs too much virtual memory with -O2
FLAGS_test_mil.cxx := -O0
endif
