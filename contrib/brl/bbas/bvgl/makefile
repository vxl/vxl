# Brown basic geometry library
USES += contrib/brl contrib/brl/bbas core

LIBRARY := bvgl
VERSION := 1.0

MANPAGE_SOURCES += bvgl_h_matrix_1d.txx 
MANPAGE_SOURCES += bvgl_h_matrix_2d.txx 
MANPAGE_SOURCES += bvgl_h_matrix_3d.txx 
MANPAGE_SOURCES += bvgl_norm_trans_2d.txx
SOURCES += bvgl_h_matrix_1d_compute.cxx
SOURCES += bvgl_h_matrix_1d_compute_linear.cxx
SOURCES += bvgl_h_matrix_1d_compute_3point.cxx
SOURCES += bvgl_h_matrix_1d_compute_optimize1.cxx
SOURCES += bvgl_h_matrix_2d_compute.cxx
SOURCES += bvgl_h_matrix_2d_compute_linear.cxx
SOURCES += bvgl_h_matrix_2d_compute_4point.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
