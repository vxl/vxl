USES += contrib/brl core

LIBRARY := bwm_pro

SOURCES += bwm_register.cxx
MANPAGE_SOURCES += bwm_processes.h

SOURCES += processes/bwm_create_corr_file_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
