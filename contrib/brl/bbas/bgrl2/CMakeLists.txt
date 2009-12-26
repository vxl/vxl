# This is bbas/bgrl2/CMakeLists.txt
# Brown Templated Graph/Hypergraph Library

SET(bgrl2_sources

bgrl2_vertex.h             bgrl2_vertex.txx
bgrl2_edge.h               bgrl2_edge.txx
bgrl2_graph.h              bgrl2_graph.txx

bgrl2_hg_vertex.h
bgrl2_hg_edge.h
bgrl2_hg_hyperedge.h

bgrl2_hg_graph.cxx         bgrl2_hg_graph.h
bgrl2_hg_hypergraph.cxx    bgrl2_hg_hypergraph.h

)

ADD_LIBRARY(bgrl2 ${bgrl2_sources})

TARGET_LINK_LIBRARIES( bgrl2 vbl )

#Algorithms on graphs
SUBDIRS(algo)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
