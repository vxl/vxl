USES += core
include ../config.mk
include ${IUEROOT}/core/vpl/config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

SOURCES += vbl_io_test_all.cxx

TESTS += test_smart_ptr_io.cxx
TESTS += test_bounding_box_io.cxx
TESTS += test_array_1d_io.cxx
TESTS += test_array_2d_io.cxx
TESTS += test_array_3d_io.cxx
TESTS += test_sparse_array_io.cxx
TESTS += golden_test_vbl_io.cxx

SOURCES += vbl_io_test_classes.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
