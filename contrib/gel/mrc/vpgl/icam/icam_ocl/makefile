USES += contrib/gel/mrc/vpgl core

LIBRARY := icam_ocl
VERSION := 1.0

MANPAGE_SOURCES += icam_ocl_cl.h
MANPAGE_SOURCES += icam_ocl_cl_gl.h
SOURCES += icam_ocl_utils.cxx
SOURCES += icam_ocl_mem.cxx
SOURCES += icam_ocl_search_manager.cxx
MANPAGE_SOURCES += icam_ocl_manager.txx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
