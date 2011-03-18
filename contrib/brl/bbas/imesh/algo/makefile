USES += contrib/brl contrib/brl/bbas core

LIBRARY := imesh_algo

SOURCES += imesh_detect.cxx
SOURCES += imesh_imls_surface.cxx
MANPAGE_SOURCES += imesh_imls_surface.txx
SOURCES += imesh_intersect.cxx
SOURCES += imesh_kd_tree.cxx
MANPAGE_SOURCES += imesh_kd_tree.txx
SOURCES += imesh_operations.cxx
SOURCES += imesh_pca.cxx
SOURCES += imesh_project.cxx
SOURCES += imesh_transform.cxx
SOURCES += imesh_render.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
