#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl
USES += mul

LIBRARY := vpdfl
VERSION := 1.0

SOURCES += vpdfl_prob_chi2.cxx
SOURCES += vpdfl_pdf_base.cxx 
SOURCES += vpdfl_pdf_builder_base.cxx 
SOURCES += vpdfl_axis_gaussian.cxx 
SOURCES += vpdfl_axis_gaussian_builder.cxx 

# Additional files to document:
# MANPAGE_SOURCES += yyyyyyyyyyy.txx

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
