USES += vxl
include ${IUEROOT}/vxl/vsl/config.mk
include ${IUEROOT}/vxl/testlib/config.mk

TESTS += vsl_test_all.cxx

TESTS += test_indent.cxx
TESTS += test_vsl_io.cxx
TESTS += test_binary_io.cxx

TESTS += test_arbitrary_length_int_conversion.cxx
TESTS += test_clipon_polymorphic_io.cxx
TESTS += test_complex_io.cxx
TESTS += test_deque_io.cxx
TESTS += test_list_io.cxx
TESTS += test_map_io.cxx
TESTS += test_polymorphic_io.cxx
TESTS += test_set_io.cxx
TESTS += test_stack_io.cxx
TESTS += test_string_io.cxx
TESTS += test_vector_io.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifeq "$(TJ_GCC30)" "1" # otherwise gcc 3.0 needs too much virtual memory
FLAGS_test_vsl_io.cxx += -O0
FLAGS_test_map_io.cxx += -O0
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
