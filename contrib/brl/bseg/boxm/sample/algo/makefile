USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm

SOURCES += boxm_simple_grey_processor.cxx
SOURCES += boxm_mob_grey_processor.cxx
SOURCES += boxm_mog_grey_processor.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
