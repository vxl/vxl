# Brown Graph Library
# brl/bbas/bgrl/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

SET(bgrl_sources
    bgrl_vertex.cxx          bgrl_vertex.h           bgrl_vertex_sptr.h
    bgrl_edge.cxx            bgrl_edge.h             bgrl_edge_sptr.h
    bgrl_graph.cxx           bgrl_graph.h            bgrl_graph_sptr.h
    bgrl_search_func.cxx     bgrl_search_func.h      bgrl_search_func_sptr.h 
)

AUX_SOURCE_DIRECTORY(Templates bgrl_sources)

ADD_LIBRARY(bgrl ${bgrl_sources})

TARGET_LINK_LIBRARIES(bgrl vbl vsl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
