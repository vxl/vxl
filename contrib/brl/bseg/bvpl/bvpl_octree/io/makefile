USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bvpl_octree_io

SOURCES += bvpl_io_pca_error_scenes.cxx
SOURCES += bvpl_io_discover_pca_kernels.cxx
SOURCES += bvpl_io_taylor_scenes_map.cxx
SOURCES += bvpl_io_global_pca.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
