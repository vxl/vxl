USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bpro_batch

MANPAGE_SOURCES += bpro_defs.h
SOURCES += bpro_batch.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
