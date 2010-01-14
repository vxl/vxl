USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm-algo

SOURCES += boxm_merge_mog.cxx
MANPAGE_SOURCES += boxm_split_sample.h
MANPAGE_SOURCES += boxm_vrml_util.h
MANPAGE_SOURCES += boxm_crop_scene.h

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
