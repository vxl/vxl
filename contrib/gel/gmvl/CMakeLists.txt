#
# ./gel/gmvl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


ADD_LIBRARY(gmvl gmvl_sources)

SOURCE_FILES(gmvl_sources
gmvl_connection        gmvl_helpers     gmvl_tag_node
gmvl_connection_cache  gmvl_image_node  gmvl_topology_node
gmvl_corner_node       gmvl_node
gmvl_database          gmvl_node_cache
)

AUX_SOURCE_DIRECTORY(Templates gmvl_sources)
