USES += contrib/brl contrib/brl/bseg core

LIBRARY := bapl_pro

SOURCES += bapl_register.cxx
MANPAGE_SOURCES += bapl_processes.h

SOURCES += processes/bapl_connectivity_processes.cxx
SOURCES += processes/bapl_extract_keypoints_process.cxx
SOURCES += processes/bapl_match_keypoints_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
