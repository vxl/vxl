 #######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl
USES += mul

LIBRARY := vpdfl
VERSION := 1.0

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

# Additional files to document: (none for the moment)
MANPAGE_SOURCES += 

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
