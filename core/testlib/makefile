USES += vxl

LIBRARY := testlib
VERSION := 1.0

# Useful utilities for the tests
SOURCES += testlib_test.cxx

# For use by the driver program
MANPAGE_SOURCES += testlib_register.h

# The main function of the driver executable
SOURCES += testlib_main.cxx

ifndef NOTEST
SUBDIRS += tests
endif

################################
include ${IUEROOT}/config/top.mk
################################
