USES += contrib/gel/mrc/vpgl core

LIBRARY := icam
VERSION := 1.0

SOURCES += icam_sample.cxx
SOURCES += icam_view_metadata.cxx
SOURCES += icam_view_sphere.cxx
SOURCES += icam_cost_func.cxx
SOURCES += icam_depth_transform.cxx
SOURCES += icam_minimizer.cxx
SOURCES += icam_depth_trans_pyramid.cxx
SOURCES += icam_utils.cxx
SOURCES += icam_transform_2d.cxx
MANPAGE_SOURCES += icam_image.h
MANPAGE_SOURCES += icam_minimizer_params.h
MANPAGE_SOURCES += icam_view_sphere_sptr.h

SUBDIRS += icam_ocl

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
