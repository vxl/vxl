## this little rule overrides the "all: tests" rule
## found in config/static-tests.mk and is necessary
## to get the vbl-test library built before the test
## programs are linked. the vbl-test library is needed
## in order that the instantiations for it can be put
## in the Templates/ directory.
orl: xall tests

USES = vxl
include ${IUEROOT}/vxl/vbl/config.mk
include ${IUEROOT}/vxl/testlib/config.mk


TESTMAIN := test_driver.cxx

SOURCES += vbl_test_classes.cxx
SOURCES += vbl_test_all.cxx
TESTS += vbl_test_array.cxx
TESTS += vbl_test_sparse_array_2d.cxx
TESTS += vbl_test_sparse_array_3d.cxx
TESTS += vbl_test_smart_ptr.cxx
#TESTS += vbl_test_basic_relation.cxx # FIXME - vcl_hash_map does not exist

# The next 4 tests are defined in vbl_test_all.cxx:
TESTS += vbl_test_bounding_box
TESTS += vbl_test_quadruple
TESTS += vbl_test_triple
TESTS += vbl_test_qsort

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
