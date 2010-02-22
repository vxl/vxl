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
MANPAGE_SOURCES += bsta_mean_shift.txx
MANPAGE_SOURCES += bsta_sample_set.txx
MANPAGE_SOURCES += bsta_beta_updater.txx
MANPAGE_SOURCES += bsta_von_mises_updater.txx
MANPAGE_SOURCES += bsta_fit_gaussian.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
