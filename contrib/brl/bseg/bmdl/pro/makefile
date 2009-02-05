USES += contrib/brl contrib/brl/bbas contrib/brl/bseg /gel core

LIBRARY := bmdl_pro

SOURCES += bmdl_register.cxx
MANPAGE_SOURCES += bmdl_defs.h

MANPAGE_SOURCES += bmdl_processes.h
SOURCES += processes/bmdl_classify_process.cxx
SOURCES += processes/bmdl_generate_mesh_process.cxx
SOURCES += processes/bmdl_trace_boundaries_process.cxx
SOURCES += processes/bmdl_lidar_roi_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
