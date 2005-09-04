USES += contrib/brl contrib/brl/bbas core

LIBRARY := bugl
VERSION := 1.0

MANPAGE_SOURCES += bugl_gaussian_point_2d.txx
MANPAGE_SOURCES += bugl_gaussian_point_3d.txx
MANPAGE_SOURCES += bugl_normal_point_2d.h
MANPAGE_SOURCES += bugl_normal_point_2d_sptr.h
MANPAGE_SOURCES += bugl_normal_point_3d.h
MANPAGE_SOURCES += bugl_normal_point_3d_sptr.h
MANPAGE_SOURCES += bugl_uncertainty_point_2d.h
MANPAGE_SOURCES += bugl_uncertainty_point_3d.h
SOURCES += bugl_curve_3d.cxx
MANPAGE_SOURCES += bugl_random_transform_set_2d.txx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
