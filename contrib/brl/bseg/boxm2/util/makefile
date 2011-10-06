USES += contrib/brl contrib/brl/bseg core

LIBRARY := boxm2_util

SOURCES += boxm2_convert_bundle.cxx
SOURCES += boxm2_cams_and_box_to_scene.cxx
SOURCES += boxm2_convert_nvm.cxx
SOURCES += boxm2_point_util.cxx
SOURCES += boxm2_detect_change_blobs.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
