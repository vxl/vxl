# gel/gmvl/CMakeLists.txt

SET(gmvl_sources
    gmvl_connection.cxx       gmvl_connection.h       gmvl_connection_sptr.h
    gmvl_connection_cache.cxx gmvl_connection_cache.h
    gmvl_database.cxx         gmvl_database.h
    gmvl_helpers.cxx          gmvl_helpers.h
    gmvl_node.cxx             gmvl_node.h             gmvl_node_sptr.h
                              gmvl_corner_node.h
                              gmvl_image_node.h
                              gmvl_tag_node.h
                              gmvl_topology_node.h
    gmvl_node_cache.cxx       gmvl_node_cache.h
)
AUX_SOURCE_DIRECTORY(Templates gmvl_sources)

ADD_LIBRARY(gmvl ${gmvl_sources})
TARGET_LINK_LIBRARIES(gmvl vtol vil1 vcl)

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)
