USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bstm_cpp_pro

SOURCES += bstm_cpp_register.cxx
MANPAGE_SOURCES += bstm_cpp_processes.h

SOURCES += processes/bstm_cpp_ingest_boxm2_scene_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
