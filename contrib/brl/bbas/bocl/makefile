USES += contrib/brl contrib/brl/bbas

LIBRARY := bocl

MANPAGE_SOURCES += bocl_cl.h
MANPAGE_SOURCES += bocl_manager.h
MANPAGE_SOURCES += bocl_manager.txx
SOURCES += bocl_utils.cxx
SOURCES += bocl_global_memory_bandwidth_manager.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
