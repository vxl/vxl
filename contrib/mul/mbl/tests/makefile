USES += vxl mul
include ${IUEROOT}/vxl/vnl/algo/config.mk
include ${IUEROOT}/vxl/vul/config.mk

IULIBS := -lmbl -lvnl-algo -lvnl -lvbl -lvsl -lvnl-io -lvul

TESTS += test_mbl.cxx # includes test_mz_random.cxx test_matrix_products.cxx test_matxvec.cxx test_index_sort.cxx test_lru_cache.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################

ifeq "$(TJ_GCC30)$(optimize)" "1-O2"
# gcc 3.0 needs too much virtual memory with -O2
FLAGS_test_mbl.cxx += -O0
endif
