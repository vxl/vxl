USES += vxl 
include ${IUEROOT}/vxl/vnl/io/config.mk
include ${IUEROOT}/vxl/testlib/config.mk

TESTS += test_vnl_io.cxx # includes all *.cxx test files
MINI_PROG_SOURCES += vnl_io_test_all.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifeq "$(TJ_GCC30)" "1"
FLAGS_test_vnl_io.cxx += -O0 # otherwise gcc 3.0 needs too much virtual memory
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
