USES += contrib/brl contrib/brl/bbas contrib/brl/bseg contrib/gel core

LIBRARY := brip
VERSION := 1.0

MANPAGE_SOURCES += brip_histogram.txx
MANPAGE_SOURCES += brip_mutual_info.txx
MANPAGE_SOURCES += brip_vil_ops.h
SOURCES += brip_vil_float_ops.cxx
SOURCES += brip_vil1_float_ops.cxx
SOURCES += brip_para_cvrg.cxx
SOURCES += brip_para_cvrg_params.cxx
SOURCES += brip_region_pixel.cxx
MANPAGE_SOURCES += brip_region_pixel_sptr.h
SOURCES += brip_roi.cxx
MANPAGE_SOURCES += brip_roi_sptr.h
SOURCES += brip_watershed.cxx
SOURCES += brip_watershed_params.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
