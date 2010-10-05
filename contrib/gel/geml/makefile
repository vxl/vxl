USES += core contrib/gel

IMMEDIATE_LIBS := -lvil -lvnl

LIBRARY := geml
VERSION := 1.0

SOURCES += geml_matcher.cxx
SOURCES += geml_matcher_correlation.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
