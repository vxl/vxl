USES += contrib/brl contrib/brl/bbas contrib/brl/bseg /gel core

LIBRARY := bmdl_pro

SOURCES += bmdl_classify_process.cxx
SOURCES += bmdl_generate_mesh_process.cxx
SOURCES += bmdl_trace_boundaries_process.cxx
SOURCES += bmdl_lidar_roi_process.cxx
SOURCES += bmdl_modeling_process.cxx
MANPAGE_SOURCES += bmdl_register.h
MANPAGE_SOURCES += bmdl_processes.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
