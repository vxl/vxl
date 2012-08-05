USES += contrib/brl contrib/brl/bbas core

LIBRARY := bpgl
VERSION := 1.0

MANPAGE_SOURCES += bpgl_segmented_rolling_shutter_camera.txx
MANPAGE_SOURCES += bpgl_radial_distortion.txx
MANPAGE_SOURCES += bpgl_lens_distortion.txx
MANPAGE_SOURCES += bpgl_comp_rational_camera.txx
MANPAGE_SOURCES += bpgl_poly_radial_distortion.txx
MANPAGE_SOURCES += bpgl_reg_fundamental_matrix.txx
MANPAGE_SOURCES += bpgl_camera_estimator.txx

MANPAGE_SOURCES += bpgl_camera_estimator_amoeba.h
MANPAGE_SOURCES += bpgl_rolling_shutter_camera.h

SOURCES += bpgl_camera_utils.cxx

SUBDIRS += algo ihog icam

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
