## this little rule overrides the "all: tests" rule
## found in config/static-tests.mk and is necessary
## to get the vul-test library built before the test
## programs are linked. the vul-test library is needed
## in order that the instantiations for it can be put
## in the Templates/ directory.
orl: xall tests

USES = vxl

LIBRARY := vul-test
SOURCES += vul_test_classes.cxx

TESTS += vul_test_all.cxx
TESTS += vul_test_arg.cxx
TESTS += vul_test_sprintf.cxx

IULIBS := -lvul-test -lvul

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
# in Templates, so make sure the vul-test-templates library is
# built and linked in.
# Do not put the instantiations in the .cxx file; it is abomination.
#
# PVr:
# Also make sure that the full implementation of the classes for which
# template instantiations are made goes into the library.  Otherwise you
# will end up with unresolved symbols in the library (which are resolved
# in one test file, but the other tests also link the library...)
