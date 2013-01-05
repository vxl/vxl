USES += contrib/brl contrib/brl/bseg core

LIBRARY := volm_pro

SOURCES += boxm2_volm_register.cxx
MANPAGE_SOURCES += boxm2_volm_processes.h

SOURCES += processes/boxm2_hypotheses_process.cxx
SOURCES += processes/boxm2_create_index_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
