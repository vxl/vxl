USES += contrib/gel contrib/oxl core

LIBRARY := vgel

SOURCES += vgel_kl.cxx
SOURCES += vgel_kl_params.cxx
MANPAGE_SOURCES += vgel_multi_view_data.txx
MANPAGE_SOURCES += vgel_multi_view_data_vertex_sptr.h

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

SUBDIRS += kl
ifndef NOTEST
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
