USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_multi

SOURCES += boxm2_multi_util.cxx
SOURCES += boxm2_multi_cache.cxx

SUBDIRS += algo exe

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
