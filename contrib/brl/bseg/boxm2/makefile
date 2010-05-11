USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm

MANPAGE_SOURCES += boxm_aux_traits.h
SOURCES += boxm_scene_base.cxx
SOURCES += boxm_scene_parser.cxx
SOURCES += boxm_io_scene_base.cxx
SOURCES += boxm_apm_traits.cxx
MANPAGE_SOURCES += boxm_scene.txx
MANPAGE_SOURCES += boxm_block.txx
MANPAGE_SOURCES += boxm_aux_scene.txx

SUBDIRS += basic algo pro sample util

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
