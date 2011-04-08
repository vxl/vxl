USES += contrib/brl contrib/brl/bseg core

LIBRARY := boxm2_view_pro

SOURCES += boxm2_view_register.cxx

MANPAGE_SOURCES += boxm2_view_processes.h
SOURCES += processes/boxm2_view_expected_image_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
