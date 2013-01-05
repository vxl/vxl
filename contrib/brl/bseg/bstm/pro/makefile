USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bstm_pro

SOURCES += bstm_register.cxx
MANPAGE_SOURCES += bstm_processes.h

SOURCES += processes/bstm_create_scene_process.cxx
SOURCES += processes/bstm_scene_statistics_process.cxx
SOURCES += processes/bstm_write_cache_process.cxx
SOURCES += processes/bstm_add_block_process.cxx
SOURCES += processes/bstm_create_cache_process.cxx
SOURCES += processes/bstm_load_scene_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
