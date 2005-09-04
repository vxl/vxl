USES += contrib/prip core
include ../config.mk

LIBRARY := vpyr_tests
SOURCES += my_pyramid.cxx
SOURCES += my_top_level.cxx
MANPAGE_SOURCES += my_pyramid_functions.txx

MINI_PROG_SOURCES += vpyr_test.cxx
MINI_PROG_SOURCES += vpyr_test2.cxx
IULIBS += -lvpyr_tests

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
