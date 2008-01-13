USES += contrib/brl contrib/brl/bbas core

LIBRARY := reg
VERSION := 1.0

SOURCES += bwm_reg_edge_champher.cxx
SOURCES += bwm_reg_image.cxx
MANPAGE_SOURCES += bwm_reg_image_sptr.h
SOURCES += bwm_reg_matcher.cxx
SOURCES += bwm_reg_processor.cxx
SOURCES += bwm_reg_utils.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
