# ./gel/gmvl/CMakeLists.txt

ADD_LIBRARY(gmvl gmvl_sources)

SOURCE_FILES(gmvl_sources
  gmvl_connection.cxx
  gmvl_connection_cache.cxx
  gmvl_corner_node.cxx
  gmvl_database.cxx
  gmvl_helpers.cxx
  gmvl_image_node.cxx
  gmvl_node.cxx
  gmvl_node_cache.cxx
  gmvl_tag_node.cxx
  gmvl_topology_node.cxx
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/gmvl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

AUX_SOURCE_DIRECTORY(Templates gmvl_sources)

IF( BUILD_EXAMPLES )
  SUBDIRS(examples)
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING)
# SUBDIRS(tests)
ENDIF( BUILD_TESTING)
