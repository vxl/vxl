USES += contrib/brl contrib/brl/bbas contrib/gel contrib/oxl core

LIBRARY := brct
VERSION := 1.0

SOURCES += kalman_filter.cpp
SOURCES += brct_algos.cpp

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
