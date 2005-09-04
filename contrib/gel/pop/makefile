USES += contrib/gel core

LIBRARY := pop
VERSION := 1.0

SOURCES += pop_edge.cxx
SOURCES += pop_geometric_cost_function.cxx
SOURCES += pop_geometric_object.cxx
SOURCES += pop_graph_cost_function.cxx
SOURCES += pop_homography_2d.cxx
SOURCES += pop_manager.cxx
SOURCES += pop_object.cxx
SOURCES += pop_parameter.cxx
SOURCES += pop_point_2d.cxx
SOURCES += pop_point_3d.cxx
SOURCES += pop_projective.cxx
SOURCES += pop_rigid_3d.cxx
SOURCES += pop_transform.cxx
SOURCES += pop_vertex.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
