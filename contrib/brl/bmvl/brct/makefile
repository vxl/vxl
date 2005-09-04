# 3D reconstruction from multiple views library
USES += contrib/brl contrib/brl/bbas contrib/brl/bmvl contrib/brl/bseg contrib/gel contrib/oxl contrib/rpl core

LIBRARY := brct
VERSION := 1.0

SOURCES += kalman_filter.cpp
SOURCES += brct_algos.cpp
SOURCES += brct_structure_estimator.cxx
SOURCES += brct_epi_reconstructor.cxx
SOURCES += brct_corr.cxx
SOURCES += brct_plane_calibrator.cxx
SOURCES += brct_plane_sweeper.cxx
SOURCES += brct_plane_sweeper_params.cxx
SOURCES += brct_volume_processor.cxx
SOURCES += brct_volume_processor_params.cxx
SOURCES += brct_dense_reconstructor.cxx
SOURCES += brct_plane_corr.cxx
MANPAGE_SOURCES += brct_plane_corr_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
