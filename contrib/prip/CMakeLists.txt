# prip/CMakeLists.txt

PROJECT( prip )

SET( PRIP_INCLUDE_DIR ${prip_SOURCE_DIR} CACHE STATIC "PRIP package include files" )

INCLUDE_DIRECTORIES(${VXLCORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PRIP_INCLUDE_DIR})

# Combinatorial Maps and Topological Maps handling
SUBDIRS( vmap )

# Digital Topology
SUBDIRS( vdtop )

# Combinatorial Pyramids and Topological Pyramids handling
SUBDIRS( vpyr )
