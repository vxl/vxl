USES += contrib/gel v3p/netlib core

LIBRARY = gtrl

# main class
SOURCES += gtrl_triangulation.cxx

# helpers
SOURCES += gtrl_triangle.cxx
MANPAGE_SOURCES += gtrl_vertex.h
SOURCES += gtrl_polygon.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
