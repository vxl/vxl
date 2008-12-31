USES += contrib/brl contrib/brl/bbas core

LIBRARY := bsta_algo

MANPAGE_SOURCES += bsta_adaptive_updater.txx
MANPAGE_SOURCES += bsta_bayes_functor.h
MANPAGE_SOURCES += bsta_gaussian_stats.h
MANPAGE_SOURCES += bsta_gaussian_updater.h
MANPAGE_SOURCES += bsta_mixture_functors.h
MANPAGE_SOURCES += bsta_truth_updater.txx
MANPAGE_SOURCES += bsta_parzen_updater.txx
SOURCES += bsta_fit_weibull.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
