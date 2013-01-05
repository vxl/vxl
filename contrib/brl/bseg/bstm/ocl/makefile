USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bstm_ocl

SOURCES += bstm_ocl_util.cxx
SOURCES += bstm_opencl_cache.cxx

SUBDIRS += pro algo

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests exe
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
