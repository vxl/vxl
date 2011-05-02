USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2-view

SOURCES += boxm2_cam_tableau.cxx
MANPAGE_SOURCES += boxm2_include_glew.h
SOURCES += boxm2_trajectory.cxx
SOURCES += boxm2_view_utils.cxx
SOURCES += boxm2_ocl_render_tableau.cxx
SOURCES += boxm2_ocl_update_tableau.cxx

SUBDIRS += pro

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
