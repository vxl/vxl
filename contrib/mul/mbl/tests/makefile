USES += vxl mul
include ${IUEROOT}/mul/mbl/config.mk
include ${IUEROOT}/vxl/vul/config.mk
include ${IUEROOT}/vxl/vbl/config.mk
include $(IUEROOT)/vxl/testlib/config.mk

TESTS += test_mbl.cxx # includes all test .cxx files

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################

ifeq "$(TJ_GCC30)$(optimize)" "1-O2"
# gcc 3.0 needs too much virtual memory with -O2
FLAGS_test_mbl.cxx += -O0
endif
