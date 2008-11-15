# This bbas/bgrl2/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

SET( bgrl2_test_sources
  test_driver.cxx
  
  test_node.h    test_node_sptr.h
  test_edge.h    test_edge_sptr.h
  test_graph.h   test_graph_sptr.h
  
  graph_test.cxx
)

AUX_SOURCE_DIRECTORY(Templates bgrl2_test_sources)

ADD_EXECUTABLE ( bgrl2_test_all ${bgrl2_test_sources})

TARGET_LINK_LIBRARIES( bgrl2_test_all bgrl2 testlib )

ADD_TEST( bgrl2_test_graph   bgrl2_test_all graph_test )

#this is for test_includes
ADD_EXECUTABLE( bgrl2_test_include test_include.cxx)
TARGET_LINK_LIBRARIES( bgrl2_test_include bgrl2)
