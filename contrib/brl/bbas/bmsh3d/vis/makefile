USES += contrib/brl core

LIBRARY := bmsh3d_pro
VERSION := 1.0

ifeq ($(HAS_BGUI3D),1)
SOURCES += bmsh3d_vis_backpt.cxx
SOURCES += bmsh3d_vis_edge.cxx
SOURCES += bmsh3d_vis_face.cxx
SOURCES += bmsh3d_vis_mesh.cxx
SOURCES += bmsh3d_vis_utils.cxx
SOURCES += bmsh3d_vis_vertex.cxx
endif

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
