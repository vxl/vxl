USES += contrib/mul core
include ../config.mk
include ${IUEROOT}/contrib/mul/vpdfl/config.mk
include ${IUEROOT}/core/vul/config.mk
include $(IUEROOT)/core/vpl/config.mk
include ${IUEROOT}/core/vbl/config.mk
include $(IUEROOT)/core/testlib/config.mk

TESTS += test_mbl.cxx # includes all test .cxx files

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################

ifeq "$(TJ_GCC30)$(optimize)" "1-O2"
# gcc 3.0 needs too much virtual memory with -O2
FLAGS_test_mbl.cxx := -O0
endif
