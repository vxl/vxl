USES := vxl gel oxl

LIBRARY := vgel

SOURCES += vgel_kl.cxx
SOURCES += vgel_kl_params.cxx
MANPAGE_SOURCES += vgel_multi_view_data.txx

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
