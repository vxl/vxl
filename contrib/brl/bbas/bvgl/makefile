# Brown basic geometry library
USES += contrib/brl contrib/gel contrib/brl/bbas core

LIBRARY := bvgl
VERSION := 1.0

MANPAGE_SOURCES += bvgl_h_matrix_1d.txx 
MANPAGE_SOURCES += bvgl_h_matrix_2d.txx 
MANPAGE_SOURCES += bvgl_h_matrix_3d.txx 

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
