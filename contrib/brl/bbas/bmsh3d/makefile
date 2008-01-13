USES += contrib/brl contrib/gel contrib/brl/bbas core

LIBRARY := bmsh3d
VERSION := 1.0

SOURCES += bmsh3d_edge.cxx
SOURCES += bmsh3d_face.cxx
SOURCES += bmsh3d_face_mc.cxx
SOURCES += bmsh3d_halfedge.cxx
SOURCES += bmsh3d_he_mesh.cxx
SOURCES += bmsh3d_mesh.cxx
SOURCES += bmsh3d_mesh_conn_recov.cxx
SOURCES += bmsh3d_mesh_mc.cxx
SOURCES += bmsh3d_pt_set.cxx
SOURCES += bmsh3d_textured_face_mc.cxx
SOURCES += bmsh3d_textured_mesh_mc.cxx
SOURCES += bmsh3d_vertex.cxx
MANPAGE_SOURCES += bmsh3d_dist.h
MANPAGE_SOURCES += bmsh3d_fuzzy_boolean.h
MANPAGE_SOURCES += bmsh3d_graph.h
MANPAGE_SOURCES += bmsh3d_ptr_list.h
MANPAGE_SOURCES += bmsh3d_triangle.h
MANPAGE_SOURCES += bmsh3d_utils.h

SUBDIRS += algo pro vis

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
