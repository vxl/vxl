USES += vxl mul
include ${IUEROOT}/vxl/vnl/algo/config.mk
include ${IUEROOT}/vxl/vil/config.mk

IULIBS := -lmil-algo -lmil -lvpdfl -lmbl -lvnl-io -lvnl-algo -lvgl -lvnl -lvil -lvsl -lvpl

TESTS += test_mil.cxx
TESTS += test_gauss_reduce_2d.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################

ifeq "$(TJ_GCC30)$(optimize)" "1-O2"
# gcc 3.0 needs too much virtual memory with -O2
FLAGS_test_mil.cxx += -O0
endif
