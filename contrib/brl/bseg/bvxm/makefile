USES += contrib/brl contrib/brl/bseg core

LIBRARY := bvxm
VERSION := 1.0

SOURCES += bvxm_image_metadata.cxx
SOURCES += bvxm_mog_grey_processor.cxx
SOURCES += bvxm_mog_rgb_processor.cxx
SOURCES += bvxm_util.cxx
SOURCES += bvxm_voxel_world.cxx
SOURCES += bvxm_world_params.cxx
SOURCES += bvxm_lidar_processor.cxx
MANPAGE_SOURCES += bvxm_voxel_traits.h
MANPAGE_SOURCES += bvxm_mog_mc_processor.txx
MANPAGE_SOURCES += bvxm_float_processor.h
SOURCES += bvxm_edge_util.cxx
SOURCES += bvxm_edge_ray_processor.cxx
MANPAGE_SOURCES += bvxm_voxel_world_sptr.h
MANPAGE_SOURCES += bvxm_von_mises_tangent_processor.txx

SUBDIRS += io pro algo grid

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
