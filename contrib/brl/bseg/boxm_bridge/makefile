USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm_bridge

SOURCES += boxm2_to_boxm.cxx
SOURCES += boxm_to_boxm2.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
