USES += contrib/brl contrib/brl/bbas core

LIBRARY := bil_algo
VERSION := 1.0

SOURCES += bil_cedt.cxx
SOURCES += bil_color_conversions.cxx
SOURCES += bil_detect_ridges.cxx
SOURCES += bil_edt.cxx
SOURCES += bil_wshed2d.cxx
MANPAGE_SOURCES += bil_detect_ridges.txx
MANPAGE_SOURCES += bil_edge_indicator.h
MANPAGE_SOURCES += bil_equalize.h
MANPAGE_SOURCES += bil_finite_differences.txx
MANPAGE_SOURCES += bil_finite_second_differences.txx
MANPAGE_SOURCES += bil_harr_wavelet_transform.h
MANPAGE_SOURCES += bil_harr_wavelet_transform.txx
MANPAGE_SOURCES += bil_perform_tensor_decomposition.txx
MANPAGE_SOURCES += bil_roi_mask.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
