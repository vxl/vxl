USES += contrib/oxl core
include ${IUEROOT}/contrib/oxl/mvl/config.mk

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

TESTS += oxltest.cpp
ifdef WIN32
SOURCES += StdAfx.cpp
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
