USES += core
include ../config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

SOURCES += vbl_test_classes.cxx
SOURCES += vbl_test_array_x.cxx
TESTS += vbl_test_array.cxx
TESTS += vbl_test_sparse_array_2d.cxx
TESTS += vbl_test_sparse_array_3d.cxx
TESTS += vbl_test_smart_ptr.cxx
#TESTS += vbl_test_basic_relation.cxx # FIXME - vcl_hash_map does not exist
TESTS += vbl_test_bounding_box.cxx
TESTS += vbl_test_quadruple.cxx
TESTS += vbl_test_triple.cxx
TESTS += vbl_test_qsort.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
