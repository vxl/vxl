USES += contrib/gel contrib/gel/mrc core

LIBRARY := vpgl_ihog
VERSION := 1.0

SOURCES += ihog_minimizer.cxx
SOURCES += ihog_cost_func.cxx
SOURCES += ihog_lsqr_cost_func.cxx
SOURCES += ihog_region.cxx
SOURCES += ihog_super_res.cxx
SOURCES += ihog_world_roi.cxx
MANPAGE_SOURCES += ihog_transform_2d.h

MANPAGE_SOURCES += ihog_region_sptr.h
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
