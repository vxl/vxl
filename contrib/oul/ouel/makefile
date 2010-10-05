USES += contrib/oul core

LIBRARY := ouel
IMMEDIATE_LIBS += -lvnl-algo -lvnl

SOURCES += kalman_filter.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
