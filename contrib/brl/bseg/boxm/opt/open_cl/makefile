USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := open_cl

SOURCES += boxm_ray_trace_manager.cxx
MANPAGE_SOURCES += boxm_opencl_manager.txx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
