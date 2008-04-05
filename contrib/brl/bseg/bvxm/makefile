USES += contrib/brl contrib/brl/bseg core

LIBRARY := bvxm
VERSION := 1.0

SOURCES += bvxm_image_metadata.cxx
SOURCES += bvxm_lidar_camera.cxx
SOURCES += bvxm_memory_chunk.cxx
SOURCES += bvxm_mog_grey_processor.cxx
SOURCES += bvxm_mog_rgb_processor.cxx
SOURCES += bvxm_util.cxx
SOURCES += bvxm_voxel_world.cxx
SOURCES += bvxm_world_params.cxx
MANPAGE_SOURCES += bvxm_voxel_grid.h
MANPAGE_SOURCES += bvxm_voxel_grid.txx
MANPAGE_SOURCES += bvxm_voxel_grid_base.h
MANPAGE_SOURCES += bvxm_voxel_slab.h
MANPAGE_SOURCES += bvxm_voxel_slab.txx
MANPAGE_SOURCES += bvxm_voxel_slab_base.h
MANPAGE_SOURCES += bvxm_voxel_slab_iterator.h
MANPAGE_SOURCES += bvxm_voxel_slab_iterator.txx
MANPAGE_SOURCES += bvxm_voxel_storage.h
MANPAGE_SOURCES += bvxm_voxel_storage_disk.h
MANPAGE_SOURCES += bvxm_voxel_storage_disk.txx
MANPAGE_SOURCES += bvxm_voxel_storage_mem.h
MANPAGE_SOURCES += bvxm_voxel_storage_mem.txx
MANPAGE_SOURCES += bvxm_voxel_traits.h

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
