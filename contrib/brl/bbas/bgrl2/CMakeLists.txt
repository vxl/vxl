# This is bbas/bgrl2/CMakeLists.txt
# Brown Templated Graph/Hypergraph Library

set(bgrl2_sources

bgrl2_vertex.h             bgrl2_vertex.hxx
bgrl2_edge.h               bgrl2_edge.hxx
bgrl2_graph.h              bgrl2_graph.hxx

bgrl2_hg_vertex.h
bgrl2_hg_edge.h
bgrl2_hg_hyperedge.h

bgrl2_hg_graph.cxx         bgrl2_hg_graph.h
bgrl2_hg_hypergraph.cxx    bgrl2_hg_hypergraph.h

)

vxl_add_library(LIBRARY_NAME bgrl2 LIBRARY_SOURCES ${bgrl2_sources})

target_link_libraries( bgrl2 ${VXL_LIB_PREFIX}vbl )

#Algorithms on graphs
add_subdirectory(algo)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
