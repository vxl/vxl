USES += contrib/brl contrib/brl/bbas contrib/gel contrib/oxl core

LIBRARY := brct
VERSION := 1.0

MANPAGE_SOURCES += brct_algos.h
SOURCES += kalman_filter.cpp

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
