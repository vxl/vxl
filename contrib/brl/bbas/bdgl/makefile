USES += contrib/brl contrib/brl/bbas contrib/gel contrib/mul contrib/oxl core

LIBRARY := bdgl
VERSION := 1.0

SOURCES += bdgl_curve_algs.cxx
SOURCES += bdgl_curve_description.cxx
SOURCES += bdgl_curve_matcher.cxx
SOURCES += bdgl_curve_region.cxx
SOURCES += bdgl_curve_tracker.cxx
MANPAGE_SOURCES += bdgl_tracker_curve_sptr.h
SOURCES += bdgl_curve_tracker_primitive.cxx
SOURCES += bdgl_curve_clustering.cxx
SOURCES += bdgl_curve_matching.cxx
SOURCES += bdgl_curve_tracking.cxx
SOURCES += bdgl_tracker_curve.cxx
SOURCES += utils.cxx
SOURCES += curve.cxx
MANPAGE_SOURCES += curveMatch.h
SOURCES += DPMatch.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
