USES += contrib/brl contrib/brl/bbas core

LIBRARY := brad
VERSION := 1.0

SOURCES += brad_illum_util.cxx
MANPAGE_SOURCES += brad_illum_cost_function.h
SOURCES += brad_sun_dir_index.cxx
SOURCES += brad_sun_pos.cxx
SOURCES += brad_hist_prob_feature_vector.cxx
SOURCES += brad_eigenspace_base.cxx
SOURCES += brad_phongs_model_est.cxx
MANPAGE_SOURCES += brad_eigenspace.txx
MANPAGE_SOURCES += brad_eigenspace_sptr.h

SUBDIRS += pro

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
