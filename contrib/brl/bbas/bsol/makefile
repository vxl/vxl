USES += contrib/brl contrib/brl/bbas contrib/gel core

LIBRARY := bsol
VERSION := 1.0

SOURCES += bsol_algs.cxx
SOURCES += bsol_distance_histogram.cxx
SOURCES += bsol_point_index_2d.cxx
SOURCES += bsol_point_index_3d.cxx
SOURCES += bsol_hough_line_index.cxx
MANPAGE_SOURCES += bsol_hough_line_index_sptr.h
SOURCES += bsol_intrinsic_curve_2d.cxx
MANPAGE_SOURCES += bsol_intrinsic_curve_2d_sptr.h
SOURCES += bsol_intrinsic_curve_3d.cxx
MANPAGE_SOURCES += bsol_intrinsic_curve_3d_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
