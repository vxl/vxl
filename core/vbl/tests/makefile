## this little rule overrides the "all: tests" rule
## found in config/static-tests.mk and is necessary
## to get the vbl-test library built before the test
## programs are linked. the vbl-test library is needed
## in order that the instantiations for it can be put
## in the Templates/ directory.
orl: xall tests

USES = vxl
IULIBS := -lvbl-test
include ${IUEROOT}/vxl/vbl/config.mk
include ${IUEROOT}/vxl/testlib/config.mk

LIBRARY := vbl-test
SOURCES += vbl_test_classes.cxx

TESTS += vbl_test_all.cxx
TESTS += vbl_test_array.cxx
TESTS += vbl_test_sparse_array_2d.cxx
TESTS += vbl_test_sparse_array_3d.cxx
TESTS += vbl_test_smart_ptr.cxx
#TESTS += vbl_test_basic_relation.cxx # FIXME - vcl_hash_map does not exist

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################

# PVr:
# Each makefile MUST have include $(IUEROOT)/config/something in
# order for the script generate_class_table to work, so please
# NEVER replace this by something like "all:\t make -f some_makefile.mk"
#
# fsm:
# However, some compilers *need* their template instances to be
# in Templates, so make sure the vbl-test-templates library is
# built and linked in.
# Do not put the instantiations in the .cxx file; it is abomination.
#
# PVr:
# Also make sure that the full implementation of the classes for which
# template instantiations are made goes into the library.  Otherwise you
# will end up with unresolved symbols in the library (which are resolved
# in one test file, but the other tests also link the library...)
