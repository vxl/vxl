USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2-ocl

SOURCES += boxm_ocl_utils.cxx
SOURCES += octree_library_functions.cl
MANPAGE_SOURCES += boxm_ray_trace_manager.txx
MANPAGE_SOURCES += boxm_ocl_render_expected.h

SUBDIRS += pro

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
