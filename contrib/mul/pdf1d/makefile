USES += contrib/mul core

LIBRARY := pdf1d
VERSION := 1.0

SOURCES += pdf1d_bhat_overlap.cxx
SOURCES += pdf1d_builder.cxx
SOURCES += pdf1d_calc_mean_var.cxx
SOURCES += pdf1d_compare_samples.cxx
SOURCES += pdf1d_compare_to_pdf.cxx
SOURCES += pdf1d_compare_to_pdf_bhat.cxx
SOURCES += pdf1d_compare_to_pdf_ks.cxx
SOURCES += pdf1d_epanech_kernel_pdf.cxx
SOURCES += pdf1d_epanech_kernel_pdf_builder.cxx
SOURCES += pdf1d_epanech_kernel_pdf_sampler.cxx
SOURCES += pdf1d_exponential.cxx
SOURCES += pdf1d_exponential_builder.cxx
SOURCES += pdf1d_exponential_sampler.cxx
SOURCES += pdf1d_flat.cxx
SOURCES += pdf1d_flat_builder.cxx
SOURCES += pdf1d_flat_sampler.cxx
SOURCES += pdf1d_gaussian.cxx
SOURCES += pdf1d_gaussian_builder.cxx
SOURCES += pdf1d_gaussian_kernel_pdf.cxx
SOURCES += pdf1d_gaussian_kernel_pdf_builder.cxx
SOURCES += pdf1d_gaussian_kernel_pdf_sampler.cxx
SOURCES += pdf1d_gaussian_sampler.cxx
SOURCES += pdf1d_kernel_pdf.cxx
SOURCES += pdf1d_kernel_pdf_builder.cxx
SOURCES += pdf1d_mixture.cxx
SOURCES += pdf1d_mixture_builder.cxx
SOURCES += pdf1d_mixture_sampler.cxx
SOURCES += pdf1d_pdf.cxx
SOURCES += pdf1d_prob_chi2.cxx
SOURCES += pdf1d_prob_ks.cxx
SOURCES += pdf1d_resample.cxx
SOURCES += pdf1d_sampler.cxx
SOURCES += pdf1d_select_pdf.cxx
SOURCES += pdf1d_weighted_epanech_kernel_pdf.cxx
SOURCES += pdf1d_weighted_epanech_kernel_sampler.cxx
SOURCES += pdf1d_weighted_kernel_pdf.cxx
MANPAGE_SOURCES += pdf1d_all.h

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

SUBDIRS += tools
ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
