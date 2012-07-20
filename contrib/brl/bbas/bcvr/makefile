USES += contrib/brl contrib/brl/bbas core

LIBRARY := bcvr

SOURCES += bcvr_cvmatch.cxx
SOURCES += bcvr_clsd_cvmatch.cxx
SOURCES += bcvr_cv_cor.cxx
MANPAGE_SOURCES += bcvr_cv_cor_sptr.h
MANPAGE_SOURCES += bcvr_clsd_cvmatch_sptr.h
MANPAGE_SOURCES += bcvr_cvmatch_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
