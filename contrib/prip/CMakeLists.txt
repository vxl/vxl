# prip/CMakeLists.txt

project( prip )

set( PRIP_INCLUDE_DIR ${prip_SOURCE_DIR} CACHE STATIC "PRIP package include files" )
set( PRIP_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/prip CACHE INTERNAL "PRIP installed include files" )

include_directories(${PRIP_INCLUDE_DIR})

# Combinatorial Maps and Topological Maps handling
add_subdirectory(vmap)

# Digital Topology
add_subdirectory(vdtop)

# Combinatorial Pyramids and Topological Pyramids handling
add_subdirectory(vpyr)
