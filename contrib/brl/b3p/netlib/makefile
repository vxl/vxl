USES += v3p/netlib contrib/brl/b3p/netlib

LIBRARY := b3p_netlib
VERSION := 1.0

SOURCES += simpson.cxx
SOURCES += adaquad.cxx
SOURCES += trapezod.cxx
MANPAGE_SOURCES += b3p_netlib.h

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
