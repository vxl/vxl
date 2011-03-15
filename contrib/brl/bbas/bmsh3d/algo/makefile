USES += contrib/brl contrib/brl/bbas core
USE_VTHREEP := 1

LIBRARY := bmsh3d_algo
VERSION := 1.0

SOURCES += bmsh3d_fileio.cxx
SOURCES += bmsh3d_fileio_ply.cxx
SOURCES += bmsh3d_mesh_bnd.cxx
SOURCES += bmsh3d_mesh_tri.cxx
SOURCES += bmsh3d_mesh_triangulate.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
