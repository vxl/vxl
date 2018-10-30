# gel/gmvl/CMakeLists.txt

doxygen_add_library(contrib/gel/gmvl
  DEPENDS contrib/gel/vtol core/vil1
  PACKAGE contrib/gel
  DESCRIPTION "Multi-View Database"
  )

set(gmvl_sources
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
aux_source_directory(Templates gmvl_sources)

vxl_add_library(LIBRARY_NAME gmvl LIBRARY_SOURCES ${gmvl_sources})
target_link_libraries(gmvl vtol ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
