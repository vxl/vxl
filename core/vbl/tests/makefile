USES = vxl

LIBRARY := vbl-test
SOURCES += vbl_test_classes.cxx

MINI_PROG_SOURCES += vbl_test_arg.cxx
MINI_PROG_SOURCES += vbl_test_array.cxx
MINI_PROG_SOURCES += vbl_test_sparse_array_2d.cxx
MINI_PROG_SOURCES += vbl_test_sprintf.cxx
MINI_PROG_SOURCES += vbl_test_smart_ptr.cxx
#MINI_PROG_SOURCES += vbl_test_basic_relation.cxx # FIXME

IULIBS := -lvbl-test -lvbl -lvcl

################################
include ${IUEROOT}/config/top.mk
################################

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
