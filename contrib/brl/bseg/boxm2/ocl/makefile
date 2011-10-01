USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_ocl

SOURCES += boxm2_opencl_cache.cxx
SOURCES += boxm2_ocl_util.cxx

MANPAGE_SOURCES += boxm2_ocl_where_root_dir.h.in

SUBDIRS += algo pro exe

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
