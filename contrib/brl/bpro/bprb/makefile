USES += contrib/brl contrib/brl/bbas contrib/brl/bpro core

LIBRARY := bprb
VERSION := 1.0

SOURCES += bprb_parameters.cxx
MANPAGE_SOURCES += bprb_parameters.txx
MANPAGE_SOURCES += bprb_parameters_sptr.h
SOURCES += bprb_process.cxx
MANPAGE_SOURCES += bprb_process_sptr.h
SOURCES += bprb_batch_process_manager.cxx
SOURCES += bprb_null_process.cxx
MANPAGE_SOURCES += bprb_macros.h
MANPAGE_SOURCES += bprb_process_manager.txx
SOURCES += bprb_process_ext.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
