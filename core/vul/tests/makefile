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
