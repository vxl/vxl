#   DESCRIPTION:
#
#   This is the vbl (Vision Basics) library.
#
#   DEPENDENCIES:  libvcl

VXL := 1 

##########################################
include ${IUEROOT}/config/top-params.mk
##########################################

USES += vxl
IULIBS := -lvcl

LIBRARY := vtol
VERSION := 1.0

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

SOURCES += vtol_topology_hierarchy_node.cxx
SOURCES += top_hold.cxx


# DEFINES += -DBUILDING_VTOL_DLL

##########################################
include ${IUEROOT}/config/top-rules.mk
##########################################
