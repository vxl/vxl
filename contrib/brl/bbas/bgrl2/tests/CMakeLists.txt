# This bbas/bgrl2/tests/CMakeLists.txt

include_directories(${BRL_INCLUDE_DIR}/bbas )

set( bgrl2_test_sources
  test_driver.cxx

  test_node.h    test_node_sptr.h
  test_edge.h    test_edge_sptr.h
  test_graph.h   test_graph_sptr.h

  graph_test.cxx
)

aux_source_directory(Templates bgrl2_test_sources)

add_executable( bgrl2_test_all ${bgrl2_test_sources})
target_link_libraries( bgrl2_test_all bgrl2 ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vbl )

add_test( NAME bgrl2_test_graph COMMAND $<TARGET_FILE:bgrl2_test_all> graph_test )

add_executable( bgrl2_test_include test_include.cxx)
target_link_libraries( bgrl2_test_include bgrl2)
add_executable( bgrl2_test_template_include test_template_include.cxx)
target_link_libraries( bgrl2_test_template_include bgrl2)
