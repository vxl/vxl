USES += contrib/brl contrib/brl/bseg core

LIBRARY := bvxm_io
VERSION := 1.0

SOURCES += bvxm_io_voxel_world.cxx
SOURCES += bvxm_io_world_params.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
