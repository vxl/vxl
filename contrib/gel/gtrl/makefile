USES += contrib/gel v3p/netlib core

LIBRARY = gtrl

# main class
SOURCES += gtrl_triangulation.cxx

# helpers
SOURCES += gtrl_triangle.cxx
SOURCES += gtrl_vertex.cxx
SOURCES += gtrl_polygon.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
