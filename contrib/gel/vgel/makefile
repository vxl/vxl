USES += contrib/gel contrib/oxl core

LIBRARY := vgel

SOURCES += vgel_kl.cxx
SOURCES += vgel_kl_params.cxx
MANPAGE_SOURCES += vgel_multi_view_data.txx
MANPAGE_SOURCES += vgel_multi_view_data_vertex_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
