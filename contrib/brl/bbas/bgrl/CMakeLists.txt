# Brown Graph Library
# brl/bbas/bgrl/CMakeLists.txt

doxygen_add_library(contrib/brl/bbas/bgrl
  DEPENDS core/vbl core/vsl
  PACKAGE contrib/brl
  DESCRIPTION "Brown Graph Library"
  )

include_directories(${BRL_INCLUDE_DIR} )
include_directories(${BRL_INCLUDE_DIR}/bbas )

set(bgrl_sources
    bgrl_vertex.cxx          bgrl_vertex.h           bgrl_vertex_sptr.h
    bgrl_edge.cxx            bgrl_edge.h             bgrl_edge_sptr.h
    bgrl_graph.cxx           bgrl_graph.h            bgrl_graph_sptr.h
    bgrl_search_func.cxx     bgrl_search_func.h      bgrl_search_func_sptr.h
)
aux_source_directory(Templates bgrl_sources)

vxl_add_library(LIBRARY_NAME bgrl LIBRARY_SOURCES ${bgrl_sources})

target_link_libraries(bgrl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
