USES += contrib/brl contrib/brl/bbas core

LIBRARY := bvxm_grid

SOURCES += bvxm_memory_chunk.cxx
MANPAGE_SOURCES += bvxm_voxel_storage.h
MANPAGE_SOURCES += bvxm_voxel_storage_disk.txx
MANPAGE_SOURCES += bvxm_voxel_storage_mem.txx
MANPAGE_SOURCES += bvxm_voxel_grid_base.h
MANPAGE_SOURCES += bvxm_voxel_slab_base.h
MANPAGE_SOURCES += bvxm_voxel_grid.txx
MANPAGE_SOURCES += bvxm_voxel_storage_disk_cached.txx
MANPAGE_SOURCES += bvxm_voxel_slab_iterator.txx
MANPAGE_SOURCES += bvxm_voxel_slab.txx
MANPAGE_SOURCES += bvxm_voxel_grid_basic_ops.h
MANPAGE_SOURCES += bvxm_opinion.h
SOURCES += bvxm_voxel_grid_opinion_basic_ops.cxx

SUBDIRS += io pro

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
