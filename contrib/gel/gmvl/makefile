#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# depends on vcl, vbl, vnl
USES := vxl gel

LIBRARY := gmvl
VERSION := 1.0

SOURCES += gmvl_database.cxx
SOURCES += gmvl_node.cxx
SOURCES += gmvl_node_cache.cxx
SOURCES += gmvl_connection.cxx
SOURCES += gmvl_connection_cache.cxx

# node implementations
SOURCES += gmvl_topology_node.cxx
SOURCES += gmvl_image_node.cxx
SOURCES += gmvl_corner_node.cxx
SOURCES += gmvl_tag_node.cxx

# helpers
SOURCES += gmvl_helpers.cxx

MANPAGE_SOURCES += gmvl_connection_sptr.h
MANPAGE_SOURCES += gmvl_node_sptr.h

ifndef NOTEST
SUBDIRS := examples # tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
