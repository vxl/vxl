#
# crossge@crd.ge.com
#
#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl

LIBRARY := gmvl
VERSION := 1.0

INCDIRS = ..

SOURCES += gmvl_database.cxx
SOURCES += gmvl_node.cxx
SOURCES += gmvl_node_cache.cxx
SOURCES += gmvl_connection.cxx
SOURCES += gmvl_connection_cache.cxx

# node implementations
SOURCES += gmvl_image_node.cxx
SOURCES += gmvl_corner_node.cxx
SOURCES += gmvl_tag_node.cxx

# helpers
SOURCES += gmvl_helpers.cxx

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
