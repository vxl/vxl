USES += contrib/brl contrib/brl/bbas core

LIBRARY := ihog
VERSION := 1.0

SOURCES += ihog_transform_2d.cxx
SOURCES += ihog_sample_grid_bilin.cxx
SOURCES += ihog_minfo_cost_func.cxx
SOURCES += ihog_lsqr_cost_func.cxx
SOURCES += ihog_cost_func.cxx
SOURCES += ihog_world_roi.cxx
SOURCES += ihog_minimizer.cxx
SOURCES += ihog_utils.cxx
MANPAGE_SOURCES += ihog_image.h
MANPAGE_SOURCES += ihog_transform_2d_sptr.h
MANPAGE_SOURCES += ihog_world_roi_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

SUBDIRS += exe

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
