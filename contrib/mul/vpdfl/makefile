USES += contrib/mul core

LIBRARY := vpdfl
VERSION := 1.0

MANPAGE_SOURCES += vpdfl_all.h

SOURCES += vpdfl_prob_chi2.cxx
SOURCES += vpdfl_pdf_base.cxx
SOURCES += vpdfl_builder_base.cxx
SOURCES += vpdfl_sampler_base.cxx
SOURCES += vpdfl_gaussian.cxx
SOURCES += vpdfl_gaussian_builder.cxx
SOURCES += vpdfl_gaussian_sampler.cxx
SOURCES += vpdfl_pc_gaussian.cxx
SOURCES += vpdfl_pc_gaussian_builder.cxx
SOURCES += vpdfl_pc_gaussian_sampler.cxx
SOURCES += vpdfl_axis_gaussian.cxx
SOURCES += vpdfl_axis_gaussian_builder.cxx
SOURCES += vpdfl_axis_gaussian_sampler.cxx
SOURCES += vpdfl_mixture.cxx
SOURCES += vpdfl_mixture_builder.cxx
SOURCES += vpdfl_mixture_sampler.cxx
SOURCES += vpdfl_calc_mean_var.cxx
SOURCES += vpdfl_gaussian_kernel_pdf.cxx
SOURCES += vpdfl_gaussian_kernel_pdf_builder.cxx
SOURCES += vpdfl_gaussian_kernel_pdf_sampler.cxx
SOURCES += vpdfl_kernel_pdf.cxx
SOURCES += vpdfl_kernel_pdf_builder.cxx
SOURCES += vpdfl_resample.cxx
SOURCES += vpdfl_add_all_binary_loaders.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
