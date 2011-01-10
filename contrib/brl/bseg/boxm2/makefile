USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2

SOURCES += boxm2_block.cxx
SOURCES += boxm2_scene.cxx
MANPAGE_SOURCES += boxm2_data.txx
MANPAGE_SOURCES += boxm2_data_traits.h
MANPAGE_SOURCES += boxm2_process.h
MANPAGE_SOURCES += boxm2_processor.h
SOURCES += boxm2_data_base.cxx
SOURCES += boxm2_util.cxx
MANPAGE_SOURCES += boxm2_block_metadata.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
