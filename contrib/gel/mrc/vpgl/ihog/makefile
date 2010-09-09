USES += contrib/gel contrib/gel/mrc core

LIBRARY := vpgl_ihog
VERSION := 1.0

SOURCES += ihog_minimizer.cxx
SOURCES += ihog_cost_func.cxx
SOURCES += ihog_lsqr_cost_func.cxx
SOURCES += ihog_world_roi.cxx
SOURCES += ihog_transform_2d.cxx
SOURCES += ihog_sample_grid_bilin.cxx
MANPAGE_SOURCES += ihog_image.h
SOURCES += ihog_utils.cxx

MANPAGE_SOURCES += ihog_transform_2d_sptr.h
MANPAGE_SOURCES += ihog_world_roi_sptr.h

SUBDIRS += exe

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
