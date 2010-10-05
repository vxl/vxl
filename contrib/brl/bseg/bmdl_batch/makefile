USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bmdl_batch
VERSION := 1.0

SOURCES += reg_bmdl.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
