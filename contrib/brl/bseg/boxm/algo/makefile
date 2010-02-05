USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm-algo

SOURCES += boxm_merge_mog.cxx
MANPAGE_SOURCES += boxm_split_sample.h
MANPAGE_SOURCES += boxm_vrml_util.h
MANPAGE_SOURCES += boxm_crop_scene.h
MANPAGE_SOURCES += boxm_scene_to_bvxm_grid.h
MANPAGE_SOURCES += boxm_fill_internal_cells.h

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
