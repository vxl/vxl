USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm-algo

SOURCES += boxm_merge_mog.cxx
MANPAGE_SOURCES += boxm_crop_scene.h
MANPAGE_SOURCES += boxm_fill_internal_cells.h
MANPAGE_SOURCES += boxm_fill_in_mesh.h
MANPAGE_SOURCES += boxm_save_block_raw.h
MANPAGE_SOURCES += boxm_compute_scene_statistics.h
MANPAGE_SOURCES += boxm_upload_mesh.h
MANPAGE_SOURCES += boxm_refine.h
MANPAGE_SOURCES += boxm_init_scene.h
MANPAGE_SOURCES += boxm_save_scene_raw.h
MANPAGE_SOURCES += boxm_compute_scene_difference.h
MANPAGE_SOURCES += boxm_split_sample.h
MANPAGE_SOURCES += boxm_linear_operations.h

SUBDIRS += pro rt sp

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
