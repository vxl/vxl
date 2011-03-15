USES += contrib/brl contrib/brl/bseg core

LIBRARY := boxm2_pro

SOURCES += boxm2_register.cxx

MANPAGE_SOURCES += boxm2_processes.h
SOURCES += processes/boxm2_load_scene_process.cxx
SOURCES += processes/boxm2_create_cache_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
