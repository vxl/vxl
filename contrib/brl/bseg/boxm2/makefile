USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2

SOURCES += boxm2_block.cxx
SOURCES += boxm2_scene.cxx
MANPAGE_SOURCES += boxm2_data.txx
MANPAGE_SOURCES += boxm2_data_traits.h
SOURCES += boxm2_data_base.cxx
SOURCES += boxm2_util.cxx
SOURCES += boxm2_block_metadata.cxx
SOURCES += boxm2_scene_parser.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
