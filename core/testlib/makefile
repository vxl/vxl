USES += core

LIBRARY := testlib
VERSION := 1.0

# Useful utilities for the tests
SOURCES += testlib_test.cxx
SOURCES += testlib_root_dir.cxx

# For use by the driver program
MANPAGE_SOURCES += testlib_register.h

# The main function of the driver executable
SOURCES += testlib_main.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
