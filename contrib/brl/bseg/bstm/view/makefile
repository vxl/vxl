USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bstm_view

SOURCES += bstm_cam_tableau.cxx
SOURCES += bstm_ocl_render_tableau.cxx
MANPAGE_SOURCES += bstm_include_glew.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
