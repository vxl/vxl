LIBRARY := b3p_netlib
VERSION := 1.0

SOURCES += simpson.c
SOURCES += adaquad.c
SOURCES += trapezod.c
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
