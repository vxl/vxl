USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bstm

SOURCES += bstm_block_metadata.cxx
SOURCES += bstm_scene.cxx
SOURCES += bstm_time_block.cxx
SOURCES += bstm_time_tree.cxx
SOURCES += bstm_block.cxx
SOURCES += bstm_scene_parser.cxx
SOURCES += bstm_data_base.cxx
SOURCES += bstm_util.cxx
MANPAGE_SOURCES += bstm_data_traits.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
