USES += contrib/brl contrib/brl/bbas core

LIBRARY := bsta
VERSION := 1.0

SOURCES += bsta_gauss.cxx
MANPAGE_SOURCES += bsta_histogram_base.h
MANPAGE_SOURCES += bsta_histogram.txx
MANPAGE_SOURCES += bsta_joint_histogram_base.h
MANPAGE_SOURCES += bsta_joint_histogram.txx
SOURCES += bsta_k_medoid.cxx
MANPAGE_SOURCES += bsta_otsu_threshold.txx
SOURCES += bsta_int_histogram_1d.cxx
SOURCES += bsta_int_histogram_2d.cxx

MANPAGE_SOURCES += bsta_histogram_sptr.h
MANPAGE_SOURCES += bsta_joint_histogram_sptr.h

MANPAGE_SOURCES += bsta_attributes.h
MANPAGE_SOURCES += bsta_basic_functors.h
MANPAGE_SOURCES += bsta_detector_gaussian.h
MANPAGE_SOURCES += bsta_detector_mixture.h
MANPAGE_SOURCES += bsta_distribution.h
MANPAGE_SOURCES += bsta_gauss_f1.h
MANPAGE_SOURCES += bsta_gauss_f2.h
MANPAGE_SOURCES += bsta_gauss_fd3.h
MANPAGE_SOURCES += bsta_gauss_ff3.h
MANPAGE_SOURCES += bsta_gauss_id3.h
MANPAGE_SOURCES += bsta_gauss_if3.h
MANPAGE_SOURCES += bsta_gauss_sd3.h
MANPAGE_SOURCES += bsta_gauss_sf3.h
MANPAGE_SOURCES += bsta_gaussian.h
MANPAGE_SOURCES += bsta_gaussian_full.h
MANPAGE_SOURCES += bsta_gaussian_full.txx
MANPAGE_SOURCES += bsta_gaussian_indep.h
MANPAGE_SOURCES += bsta_gaussian_indep.txx
MANPAGE_SOURCES += bsta_gaussian_sphere.h
MANPAGE_SOURCES += bsta_gaussian_sphere.txx
MANPAGE_SOURCES += bsta_mixture.h
MANPAGE_SOURCES += bsta_mixture_fixed.h

SUBDIRS += algo io

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
