USES = vxl

TESTS += vbl_test_arg.cxx
TESTS += vbl_test_sparse_array_2d.cxx
TESTS += vbl_test_sprintf.cxx
TESTS += vbl_test_smart_ptr.cxx

IULIBS := -lvbl-test-templates -lvbl -lvcl
VXL := 1

##########################################
include ${IUEROOT}/config/static-tests.mk
##########################################
